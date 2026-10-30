"""G1 same-bundle proposal; lead-only runner, never a native qualification.

Only the reviewed, installed memory driver is launched. No historical fixture or
complete helper is evaluated. RuntimeFixture supplies installation/root only.
All case trees are retained under out/verification-synthetic for lead evidence.
Importing this module installs nothing and launches nothing.
"""
from functools import partial
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile
from unittest.mock import patch

from verification import catalogue, evidence, planning
from verification.adapters.windows import WindowsAdapter
from verification.model import ProcessOutcome, Request
import runtime_fixtures

ROOT = Path(__file__).resolve().parents[3]
HISTORY = ROOT / 'scripts/tests/verification_windows'
PAYLOAD = 'raw/windows-helper-input.json'
WITNESS = 'logs/windows-helper-prepare/toolchain.json'
ROOT_LINES = ("$root = Split-Path $PSScriptRoot -Parent\n"
              "$tooling = Join-Path $root 'out/tooling'\n"
              "$venvPython = Join-Path $tooling 'build/Scripts/python.exe'")
PORTS = {
    'New-VerificationProcessObjectLeaf': 'New-G1MemoryProcess',
    'Read-VerificationClockLeaf': 'Read-G1Clock',
    'Read-VerificationHostObservationLeaf': 'Read-G1Host',
    'Read-VerificationToolMetadataLeaf': 'Read-G1Metadata',
    'Invoke-VerificationNativeProcess': 'Invoke-G1MemoryCapture',
}
# Exact reviewed population, not an allowlist extended from arbitrary source.
BINDING_NAMES = (
    'Get-VerificationContext', 'Copy-VerificationMap', 'Get-VerificationAbsolutePath',
    'Assert-VerificationRegularPath', 'New-VerificationOwnedDirectory',
    'Write-VerificationPrivateJson', 'Read-VerificationFlatStrings',
    'Get-VerificationEnvironmentNames', 'Select-VerificationEnvironment',
    'Read-VerificationHostObservationLeaf', 'Read-VerificationClockLeaf',
    'Enter-VerificationContext', 'Exit-VerificationContext',
    'Read-VerificationEnvironmentLeaf', 'Write-VerificationEnvironmentLeaf',
    'Set-VerificationCommandRole', 'New-VerificationProcessObjectLeaf',
    'Resolve-VerificationExecutable', 'Read-VerificationToolMetadataLeaf',
    'Get-VerificationTooling', 'Initialize-VerificationTooling',
    'Invoke-VerificationProcessLeaf', 'Get-VerificationExporterSource',
    'Get-VerificationToolsetDirectory', 'Read-VerificationMsvcEnvironmentLeaf',
    'Get-VerificationWitnessNames', 'Assert-VerificationPreparedTree',
    'Write-VerificationToolchainWitness',
)
WRAPPER_NAMES = ('Resolve-VerificationInputs', 'ConvertTo-VerificationWindowsArgument',
                 'Get-VerificationProcessEnvironment', 'Set-VerificationProcessEnvironment',
                 'Get-VerificationMsvcEnvironment', 'Invoke-VerificationChild')
DATA_NAMES = ('Read-VerificationPayloadFile', 'Resolve-VerificationPhaseFlags',
              'New-VerificationNativeStartInfo')
# Initialize-VerificationTooling really requests python-pip even when installed.
# Keep the observation explicit; never replace that production function to hide it.
PREPARE_ROLES = ('python-pip', 'vswhere', 'msvc-env', 'conan-install', 'cmake-configure')
BUILD_ROLES = ('msvc-env', 'cmake-build')


def digest(data):
    return hashlib.sha256(data).hexdigest()


def text_hash(text):
    return digest(text.encode('utf-8'))


def write_json(path, value):
    with path.open('x', encoding='utf-8', newline='\n') as stream:
        json.dump(value, stream, indent=2, sort_keys=True)


def marked(source, name):
    begin, end = '# BEGIN ARTIST_VERIFICATION_' + name, '# END ARTIST_VERIFICATION_' + name
    if source.count(begin) != 1 or source.count(end) != 1:
        raise RuntimeError('HARNESS/INCONCLUSIVE: marker population ' + name)
    a, b = source.index(begin) + len(begin), source.index(end)
    if a >= b:
        raise RuntimeError('HARNESS/INCONCLUSIVE: marker order ' + name)
    return source[a:b]


def admitted_program():
    """Same normalized extraction as historical admission; no repin/fallback.

    Also keep byte hashes separately: historical pins use UTF-8/BOM-free LF text.
    Only four binding leaves are removed, before any evaluation. The native
    process primitive is excluded at its original boundary. Five wrappers then
    replace exactly those five ports, preceding the unchanged real entry.
    """
    source_path = ROOT / 'scripts/build-windows.ps1'
    source = source_path.read_text(encoding='utf-8-sig')
    pin_path = HISTORY / 'native_binding_admission.json'
    pin = json.loads(pin_path.read_text(encoding='utf-8'))
    boundary = "$ErrorActionPreference = 'Stop'"
    if (len(source) > 262144 or not source.startswith('param(')
            or source.count(boundary) != 1 or source.count(ROOT_LINES) != 1):
        raise RuntimeError('HARNESS/INCONCLUSIVE: source boundaries')
    native, binding = marked(source, 'NATIVE'), marked(source, 'BINDING')
    marked(source, 'DISPATCH')
    start = source.index('# END ARTIST_VERIFICATION_NATIVE') + len('# END ARTIST_VERIFICATION_NATIVE')
    end = source.index('# END ARTIST_VERIFICATION_DISPATCH')
    binding_start = source.index('# BEGIN ARTIST_VERIFICATION_BINDING')
    if not start <= binding_start < source.index('# END ARTIST_VERIFICATION_BINDING') < end:
        raise RuntimeError('HARNESS/INCONCLUSIVE: binding order')
    if any(line.strip() and not line.lstrip().startswith('#')
           for line in source[start:binding_start].splitlines()):
        raise RuntimeError('HARNESS/INCONCLUSIVE: effect before definitions')
    entry = source[start:end].replace(
        '# BEGIN ARTIST_VERIFICATION_BINDING' + binding + '# END ARTIST_VERIFICATION_BINDING', '', 1)
    if source.index(ROOT_LINES) >= end:
        entry += '\n' + ROOT_LINES + '\n'
    parts = {'parameters': source[:source.index(boundary)],
             'wrappers': marked(source, 'WRAPPERS'),
             'data': native[native.index('function Read-VerificationPayloadFile('):
                            native.index('function Invoke-VerificationNativeProcess(')],
             'root': ROOT_LINES, 'entry': entry, 'binding': binding}
    if (text_hash(source) != pin['source_sha256'] or text_hash(native) != pin['native_sha256']
            or text_hash(native) != (HISTORY / 'native_source.sha256').read_text().strip()
            or {key: text_hash(value) for key, value in parts.items()} != pin['fragments']):
        raise RuntimeError('HARNESS/INCONCLUSIVE: historical pin mismatch; never auto-repin')
    literal_start = binding.index("    return @'\n", binding.index('function Get-VerificationExporterSource')) + len("    return @'\n")
    literal_end = binding.index("\n'@", literal_start)
    exporter = binding[literal_start:literal_end]
    if text_hash(exporter) != pin['exporter_sha256']:
        raise RuntimeError('HARNESS/INCONCLUSIVE: exporter literal identity')
    exporter_start = exporter.index('function Write-VerificationMsvcExport(')
    exporter_end = exporter.index('\nWrite-VerificationMsvcExport -Snapshot ')
    exporter_body = exporter[exporter_start:exporter_end]
    safe_binding = binding
    for first, following in (
        ('Read-VerificationHostObservationLeaf', 'Read-VerificationClockLeaf'),
        ('Read-VerificationClockLeaf', 'Enter-VerificationContext'),
        ('New-VerificationProcessObjectLeaf', 'Resolve-VerificationExecutable'),
        ('Read-VerificationToolMetadataLeaf', 'Get-VerificationTooling'),
    ):
        a = safe_binding.index('function ' + first)
        b = safe_binding.index('function ' + following, a)
        safe_binding = safe_binding[:a] + safe_binding[b:]
    replacements = '''
function New-VerificationProcessObjectLeaf([Diagnostics.ProcessStartInfo]$StartInfo) { New-G1MemoryProcess $StartInfo }
function Read-VerificationClockLeaf { Read-G1Clock }
function Read-VerificationHostObservationLeaf { Read-G1Host }
function Read-VerificationToolMetadataLeaf([string]$Program) { Read-G1Metadata $Program }
function Invoke-VerificationNativeProcess($Process, [string]$LogDirectory, [int]$TimeoutMilliseconds) { Invoke-G1MemoryCapture $Process $LogDirectory $TimeoutMilliseconds }
'''
    program = (parts['parameters'] + "\n$ErrorActionPreference = 'Stop'\n" + parts['wrappers']
               + parts['data'] + safe_binding + replacements + parts['entry'])
    return dict(source=source, pin=pin, parts=parts, native=native, exporter=exporter,
                exporter_body=exporter_body, safe_binding=safe_binding,
                replacements=replacements, program=program,
                original_bytes_sha256=digest(source_path.read_bytes()))


def observe_launch(fixture, command, *, expected_hash=None):
    """Pure observation gate. No creation, repair, sink.command or recapture."""
    sink = fixture.sink
    if sink.bundle is None or sink.active_phase != command.phase:
        raise AssertionError('HARNESS: active phase lease absent')
    for path in (sink.bundle / '.phase.lock', fixture.build_root / '.verification-writer.lock'):
        if not path.is_file():
            raise AssertionError('HARNESS: writer lease absent')
    log = sink.bundle / ('logs/windows-helper-' + command.phase)
    if not log.is_dir() or tuple(log.iterdir()):
        raise AssertionError('HARNESS: core phase directory absent or already used')
    payload = sink.bundle / PAYLOAD
    raw = payload.read_bytes()
    actual = digest(raw)
    if actual != (expected_hash or sink.manifest['artifacts'].get(PAYLOAD)):
        raise AssertionError('HARNESS: closed payload hash diverged')
    # A Windows exclusive read also detects a still-open incompatible writer.
    fixture.assert_closed(payload)
    identity = json.loads((sink.bundle / 'identity-before.json').read_text(encoding='utf-8'))
    for relative, expected in fixture.program_identities.items():
        if identity['files'].get(relative) != expected:
            raise AssertionError('HARNESS: program absent from real snapshot: ' + relative)
    for relative, expected in fixture.inert_identities.items():
        path = fixture.root / relative
        fixture.assert_closed(path)
        if digest(path.read_bytes()) != expected:
            raise AssertionError('HARNESS: inert tool identity changed: ' + relative)
    return {'payload_sha256': actual, 'payload_mtime_ns': payload.stat().st_mtime_ns,
            'phase': command.phase, 'phase_lock': True, 'tree_lock': True}


class ClosedRunner:
    """One exact description -> one installed PS driver -> one ProcessOutcome.

    Subprocess capture is the sole primary capture, not another sink command.
    stdout/stderr of synthetic tools are written by the memory port, not here.
    """
    def __init__(self, fixture):
        self.fixture = fixture
        self.calls, self.completed, self.launches = [], [], []
        self.used = set()
        self.harness_failures = []

    def require_harness(self):
        if self.harness_failures:
            raise RuntimeError('HARNESS/INCONCLUSIVE: ' + ' | '.join(self.harness_failures))

    def admit(self, command):
        f = self.fixture
        if command.phase not in ('prepare', 'build') or command.phase in self.used:
            raise AssertionError('HARNESS: unknown or repeated driver invocation')
        expected = f.adapter._helper(f.plan, f.sink.bundle, command.phase)[0]
        if command != expected:
            raise AssertionError('HARNESS: command differs from real Windows description')
        if Path(command.cwd) != f.root or command.env_delta:
            raise AssertionError('HARNESS: cwd/environment mismatch')
        return observe_launch(f, command)

    def run(self, command):
        before = self.admit(command)
        self.used.add(command.phase)
        self.calls.append(command)
        f = self.fixture
        control = f.root / 'out/seam-control' / command.phase
        control.mkdir(exist_ok=False)
        input_file, result_file = control / 'input.json', control / 'result.json'
        write_json(input_file, dict(root=str(f.root), phase=command.phase,
            argv=list(command.argv[7:]), bundle=str(f.sink.bundle),
            payload_sha256=before['payload_sha256'], conan_code=f.conan_code,
            paths={key: str(value) for key, value in f.paths.items()},
            parent=f.parent, msvc=f.msvc, ticks=list(range(0, 100)),
            program_identities=f.program_identities, expected_functions=f.functions,
            inert_identities=f.inert_identities,
            exporter_sha256=f.admission['pin']['exporter_sha256']))
        argv = (str(f.powershell), '-NoProfile', '-NonInteractive', '-ExecutionPolicy', 'Bypass',
                '-File', str(f.driver), '-InputFile', str(input_file), '-OutputFile', str(result_file))
        self.launches.append(argv)
        timed_out = False
        try:
            child = subprocess.run(argv, cwd=f.root, stdin=subprocess.DEVNULL,
                                   stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                   shell=False, timeout=60, check=False)
            code, stdout, stderr = child.returncode, child.stdout, child.stderr
        except subprocess.TimeoutExpired as error:
            timed_out = True
            code, stdout, stderr = None, error.stdout or b'', error.stderr or b''
            self.harness_failures.append('owned driver timeout')
        except OSError:
            code, stdout, stderr = None, b'', b''
            self.harness_failures.append('owned driver could not start')
        # Preserve the same primary streams in control evidence, never recapture.
        (control / 'driver-stdout.bin').write_bytes(stdout)
        (control / 'driver-stderr.bin').write_bytes(stderr)
        report = None
        try:
            report = json.loads(result_file.read_text(encoding='utf-8-sig'))
            if report['harness_error']:
                self.harness_failures.append(report['harness_error'])
            if report['code'] != code:
                self.harness_failures.append('primary outcome/report disagreement')
        except (OSError, ValueError, KeyError, TypeError):
            self.harness_failures.append('owned driver report missing or malformed')
        try:
            observation = f.closed_observation(command.phase)
        except (OSError, AssertionError):
            observation = None
            self.harness_failures.append('outputs not closed/readable at runner return')
        self.completed.append(dict(before=before, files=observation, report=report,
                                   primary_returncode=code, timed_out=timed_out))
        # Even a driver/harness failure uses this SINGLE primary outcome. The
        # test reports HARNESS only AFTER the real collector/_finish has run.
        return ProcessOutcome(code, stdout.decode('utf-8', errors='surrogateescape'),
                              stderr.decode('utf-8', errors='surrogateescape'), timed_out, False)


class WindowsSeamFixture:
    def __init__(self, evidence_root, *, conan_code=0):
        if os.name != 'nt':
            raise RuntimeError('PRECONDITION/INCONCLUSIVE: Windows only, never a skip or RED')
        self.powershell = Path(os.environ['SystemRoot']) / 'System32/WindowsPowerShell/v1.0/powershell.exe'
        if not self.powershell.is_file():
            raise RuntimeError('PRECONDITION/INCONCLUSIVE: PowerShell absent')
        evidence_root.mkdir(parents=True, exist_ok=True)
        # Retain the ORIGINAL tree/bundle, never copy it into a success fixture.
        factory = partial(tempfile.TemporaryDirectory, dir=evidence_root, delete=False)
        with patch.object(runtime_fixtures.tempfile, 'TemporaryDirectory', factory):
            self.common = runtime_fixtures.RuntimeFixture()
        self.root = self.common.root
        self.plan = planning.make_plan(Request('windows', 'cpu-only', ('cpu.float-mirror',),
            'debug', allow_dependency_resolution=True), catalogue.list_scenarios())
        self.adapter = WindowsAdapter(self.root)
        self.build_root = self.root / 'out/build/windows-msvc-debug-verification'
        self.conan_code = conan_code
        self.admission = admitted_program()
        self.driver = self.root / 'scripts/tests/verification_seams/g1_windows_memory_driver.ps1'
        self.paths = {
            'python': self.root / 'out/tooling/build/Scripts/python.exe',
            'pip': self.root / 'out/tooling/build/Scripts/pip.exe',
            'conan': self.root / 'out/tooling/build/Scripts/conan.exe',
            'ninja': self.root / 'out/tooling/build/Scripts/ninja.exe',
            'cmake': self.root / 'out/synthetic CMake/bin/cmake.exe',
            'ctest': self.root / 'out/synthetic CMake/bin/ctest.exe',
            'vswhere': self.root / 'out/Program Files x86/Microsoft Visual Studio/Installer/vswhere.exe',
            'cl': self.root / 'out/Visual Studio/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe',
            'cmd': self.root / 'out/Windows/System32/cmd.exe',
            'host': self.root / 'out/Windows/System32/WindowsPowerShell/v1.0/powershell.exe',
        }
        for path in self.paths.values():
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b'G1 INERT IDENTITY ONLY; NEVER EXECUTE')
        self.vs = self.root / 'out/Visual Studio'
        vcvars = self.vs / 'VC/Auxiliary/Build/vcvarsall.bat'
        vcvars.parent.mkdir(parents=True, exist_ok=True)
        vcvars.write_bytes(b'G1 INERT DATA ONLY; NEVER CALL')
        self.inert_identities = {path.relative_to(self.root).as_posix(): digest(path.read_bytes())
                                 for path in (*self.paths.values(), vcvars)}
        self.parent = {'PATH': ';'.join(map(str, (self.paths['python'].parent, self.paths['cmake'].parent))),
                       'SystemRoot': str(self.root / 'out/Windows'),
                       'ProgramFiles(x86)': str(self.root / 'out/Program Files x86')}
        self.msvc = dict(self.parent, VSINSTALLDIR=str(self.vs), VCToolsVersion='14.44.35207',
                         VSCMD_ARG_TGT_ARCH='x64', VSCMD_ARG_HOST_ARCH='x64',
                         WindowsSDKVersion='10.0.26100.0')
        install = {
            'scripts/build-windows.ps1': self.admission['program'].encode(),
            'scripts/seam-original-build-windows.ps1.data': (ROOT / 'scripts/build-windows.ps1').read_bytes(),
            'scripts/tests/verification_seams/g1_windows_memory_driver.ps1': Path(__file__).with_name('g1_windows_memory_driver.ps1').read_bytes(),
            'scripts/tests/verification_seams/g1_windows_fixture.py': Path(__file__).read_bytes(),
            'scripts/tests/verification_seams/test_g1_windows_helper_seam.py': Path(__file__).with_name('test_g1_windows_helper_seam.py').read_bytes(),
            'scripts/seam/native_binding_admission.json': (HISTORY / 'native_binding_admission.json').read_bytes(),
            'scripts/seam/native_source.sha256': (HISTORY / 'native_source.sha256').read_bytes(),
            'scripts/seam/exporter-literal.data': self.admission['exporter'].encode(),
            'scripts/seam/exporter-body.ps1': self.admission['exporter_body'].encode(),
            'scripts/seam/safe-binding.data': self.admission['safe_binding'].encode(),
            'scripts/seam/replacements.data': self.admission['replacements'].encode(),
            'conan/profiles/windows-msvc': (ROOT / 'conan/profiles/windows-msvc').read_bytes(),
            'scripts/build-requirements.txt': (ROOT / 'scripts/build-requirements.txt').read_bytes(),
        }
        install.update({'scripts/seam/' + name + '.data': text.encode()
                        for name, text in self.admission['parts'].items()})
        self.program_identities = {}
        for relative, raw in install.items():
            path = self.root / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(raw)
            self.program_identities[relative] = digest(raw)
        self.functions = list((*WRAPPER_NAMES, *DATA_NAMES,
            *(name for name in BINDING_NAMES if name not in PORTS), *PORTS))
        (self.root / 'out/seam-control').mkdir()
        write_json(self.root / 'out/seam-control/installation.json', dict(
            author_candidate='d19c720638bc56eb46f2862874110d0e1b6f2852',
            root=str(self.root), original_bytes_sha256=self.admission['original_bytes_sha256'],
            normalized_source_sha256=self.admission['pin']['source_sha256'],
            historical_admission=self.admission['pin'], installed_programs=self.program_identities,
            installed_inert_programs=self.inert_identities,
            substitutions=PORTS, native_qualification='NON_EXECUTE'))
        self.runner = ClosedRunner(self)
        self.sink = evidence.BundleSink(self.root, self.root / 'out/verification', runner=self.runner,
            revision_reader=lambda root: {'head': '1' * 40,
                                         'delta': 'G1 synthetic installation; not a Git checkout'})

    @staticmethod
    def assert_closed(path):
        # No load of the inert binary: only exclusive OPEN_EXISTING file access.
        import ctypes
        from ctypes import wintypes
        kernel = ctypes.WinDLL('kernel32', use_last_error=True)
        kernel.CreateFileW.argtypes = (wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD,
                                      wintypes.LPVOID, wintypes.DWORD, wintypes.DWORD, wintypes.HANDLE)
        kernel.CreateFileW.restype = wintypes.HANDLE
        kernel.CloseHandle.argtypes = (wintypes.HANDLE,)
        handle = kernel.CreateFileW(str(path), 0x80000000, 0, None, 3, 0x80, None)
        if handle == wintypes.HANDLE(-1).value:
            raise AssertionError('HARNESS: output is not closed/exclusively readable: ' + str(path))
        if not kernel.CloseHandle(handle):
            raise AssertionError('HARNESS: exclusive observation handle did not close')

    def closed_observation(self, phase):
        root = self.sink.bundle / ('logs/windows-helper-' + phase)
        files = [self.sink.bundle / PAYLOAD, *sorted(p for p in root.rglob('*') if p.is_file())]
        if (self.sink.bundle / WITNESS).is_file():
            files.append(self.sink.bundle / WITNESS)
        result = {}
        for path in files:
            self.assert_closed(path)
            raw = path.read_bytes()
            result[path.relative_to(self.sink.bundle).as_posix()] = dict(
                sha256=digest(raw), size=len(raw), mtime_ns=path.stat().st_mtime_ns)
        return result

    def audit(self):
        control = self.root / 'out/seam-control/audit'
        control.mkdir()
        input_file, output_file = control / 'input.json', control / 'result.json'
        write_json(input_file, dict(root=str(self.root), phase='audit',
            program_identities=self.program_identities, expected_functions=self.functions,
            exporter_sha256=self.admission['pin']['exporter_sha256']))
        argv = (str(self.powershell), '-NoProfile', '-NonInteractive', '-ExecutionPolicy', 'Bypass',
                '-File', str(self.driver), '-InputFile', str(input_file), '-OutputFile', str(output_file))
        child = subprocess.run(argv, cwd=self.root, stdin=subprocess.DEVNULL,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                               shell=False, timeout=60, check=False)
        (control / 'stdout.bin').write_bytes(child.stdout)
        (control / 'stderr.bin').write_bytes(child.stderr)
        report = json.loads(output_file.read_text(encoding='utf-8-sig'))
        if child.returncode != 0 or report['harness_error']:
            raise RuntimeError('HARNESS/INCONCLUSIVE: AST baseline: ' + str(report['harness_error']))
        return report
