"""Test-owned extraction only. Importing this module never reads or starts tools.

Source pins are explicit review gates, not automatically refreshed snapshots.
No core/adapter import and no fallback to the complete helper is permitted.
"""

import hashlib
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]
HELPER = ROOT / "scripts/build-windows.ps1"
PIN = Path(__file__).with_name("helper_fragments_red.json")
FIXTURE = Path(__file__).with_name("helper_functional_fixture.ps1")
ROOT_BINDING = (
    "$root = Split-Path $PSScriptRoot -Parent\n"
    "$tooling = Join-Path $root 'out/tooling'\n"
    "$venvPython = Join-Path $tooling 'build/Scripts/python.exe'"
)


def sha(text):
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def marked(source, name):
    begin, end = f"# BEGIN ARTIST_VERIFICATION_{name}", f"# END ARTIST_VERIFICATION_{name}"
    if source.count(begin) != 1 or source.count(end) != 1:
        raise RuntimeError(f"HARNESS: {name} markers must be unique")
    start, stop = source.index(begin) + len(begin), source.index(end)
    if start >= stop:
        raise RuntimeError(f"HARNESS: {name} markers out of order")
    return source[start:stop]


def extract(source, *, check_pin=True):
    """Keep executable source order; exclude only the admitted definition region."""
    if len(source) > 262144 or not source.startswith("param("):
        raise RuntimeError("HARNESS: helper population/bounds mismatch")
    boundary = "$ErrorActionPreference = 'Stop'"
    if source.count(boundary) != 1:
        raise RuntimeError("HARNESS: parameter boundary must be unique")
    parameters = source[:source.index(boundary)]
    if len(parameters) > 4096:
        raise RuntimeError("HARNESS: parameter fragment too large")
    wrappers = marked(source, "WRAPPERS")
    native = marked(source, "NATIVE")
    # Read-only payload/flags functions only. Never load StartInfo/capture code.
    names = (
        "function Read-VerificationPayloadFile(",
        "function Resolve-VerificationPhaseFlags(",
        "function New-VerificationNativeStartInfo(",
    )
    if any(native.count(name) != 1 for name in names):
        raise RuntimeError("HARNESS: native data-function boundaries not unique")
    a, b, c = (native.index(name) for name in names)
    if not a < b < c:
        raise RuntimeError("HARNESS: native data-function order mismatch")
    marked(source, "DISPATCH")  # Validate before slicing guard + dispatch together.
    begin = source.index("# END ARTIST_VERIFICATION_NATIVE") + len(
        "# END ARTIST_VERIFICATION_NATIVE"
    )
    stop = source.index("# END ARTIST_VERIFICATION_DISPATCH")
    if begin >= stop:
        raise RuntimeError("HARNESS: entry boundaries out of order")
    binding = marked(source, "BINDING")
    binding_begin = source.index("# BEGIN ARTIST_VERIFICATION_BINDING")
    binding_end = source.index("# END ARTIST_VERIFICATION_BINDING")
    if not begin <= binding_begin < binding_end < stop or len(binding) > 65536:
        raise RuntimeError("HARNESS: binding definitions must precede source dispatch within bounds")
    if any(line.strip() and not line.lstrip().startswith("#") for line in source[begin:binding_begin].splitlines()):
        raise RuntimeError("HARNESS: executable entry before definitions; extraction must not hoist")
    entry = source[begin:stop]
    entry = entry.replace("# BEGIN ARTIST_VERIFICATION_BINDING" + binding + "# END ARTIST_VERIFICATION_BINDING", "", 1)
    # The excluded region is also delivered as DATA for a hash/population AST
    # check by the fixture, never evaluated. Load only these four pure bodies.
    pure = ""
    for header, following in (
        ("function Get-VerificationAbsolutePath(", "function Assert-VerificationRegularPath("),
        ("function Assert-VerificationRegularPath(", "function New-VerificationOwnedDirectory("),
        ("function Get-VerificationEnvironmentNames {", "function Select-VerificationEnvironment("),
        ("function Select-VerificationEnvironment(", "function Read-VerificationHostObservationLeaf {"),
    ):
        if binding.count(header) != 1 or binding.count(following) != 1:
            raise RuntimeError("HARNESS: pure source helper boundaries must be unique")
        start, end = binding.index(header), binding.index(following)
        if start >= end:
            raise RuntimeError("HARNESS: pure source helper order changed")
        pure += binding[start:end]
    if source.count(ROOT_BINDING) != 1:
        raise RuntimeError("HARNESS: actual root/tooling binding is not unique or changed")
    root_offset = source.index(ROOT_BINDING)
    if root_offset < begin:
        raise RuntimeError("HARNESS: root moved outside reviewed entry boundary")
    # At RED these lines follow dispatch; keep them AFTER it, before sentinel.
    if root_offset >= stop:
        entry += "\n" + ROOT_BINDING + "\n"
    fragments = {
        "parameters": parameters,
        "definitions": wrappers + "\n" + native[a:c] + "\n" + pure,
        "entry": entry,
        "root_binding": ROOT_BINDING,
    }
    if any(len(value) > 32768 for value in fragments.values()):
        raise RuntimeError("HARNESS: fragment exceeds review bounds")
    if check_pin:
        pin = json.loads(PIN.read_text(encoding="utf-8"))
        actual = {name: sha(value) for name, value in fragments.items()}
        if pin.get("source_sha256") != sha(source) or pin.get("binding_sha256") != sha(binding) or pin.get("fragments") != actual:
            raise RuntimeError("HARNESS: changed helper/fragment pin; separate lead review required")
    return fragments
