"""T2 identity snapshot (pool-6). Usage: python identity.py <output-file-name> <label>

Records HEAD, branch state, delta versus the fork base cb9f268 and, for every
tracked file of the T2 perimeter, the SHA256 of the raw bytes plus whether
those bytes match the blob recorded at HEAD (git hash-object --no-filters).
"""
import datetime
import hashlib
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
BASE = "cb9f2688dcb268a430c37cb66de3bd3aa31dd843"
FILES = [
    ".opencode/evidence/mission-gpu-qa/f7-qa-plan.md",
    ".opencode/evidence/mission-f7/preflight-P1-uniform.md",
    ".opencode/evidence/mission-f7/preflight-P2-attribute.md",
    ".opencode/evidence/mission-f7/preflight-P3-uniformreader.md",
    "docs/architecture/next-mission.md",
    "docs/development/known-incidents.md",
    "ARTIST/include/graphic/pipeline/Attribute.hpp",
    "ARTIST/include/graphic/context/AttributeContext.hpp",
    "ARTIST/include/graphic/opengl/context/AttributeContext.hpp",
    "ARTIST/include/graphic/opengl/pipeline/component/attribute/Setter.hpp",
    "ARTIST/include/graphic/opengl/pipeline/component/attribute/Binder.hpp",
    "ARTIST/include/graphic/opengl/pipeline/component/attribute/Unbinder.hpp",
    "ARTIST/include/graphic/opengl/validator/AttributeConcept.hpp",
    "ARTIST/include/graphic/validator/ComponentConcept.hpp",
    "ARTIST/include/graphic/context/PassContext.hpp",
    "ARTIST/include/graphic/pipeline/Pass.hpp",
    "ARTIST/include/graphic/pipeline/Uniform.hpp",
    "ARTIST/include/graphic/opengl/pipeline/component/pass/AttributeReader.hpp",
    "ARTIST/tests/mock/graphic/pipeline/MockAttribute.hpp",
    "ARTIST/tests/mock/graphic/pipeline/MockPass.hpp",
    "ARTIST/tests/mock/graphic/opengl/context/MockAttributeContext.hpp",
    "ARTIST/tests/mock/graphic/opengl/pipeline/component/attribute/MockSetter.hpp",
    "ARTIST/tests/mock/graphic/opengl/pipeline/component/attribute/MockBinder.hpp",
    "ARTIST/tests/mock/graphic/opengl/pipeline/component/attribute/MockUnbinder.hpp",
    "ARTIST/tests/src/graphic/pipeline/AttributeTests.cpp",
    "ARTIST/tests/src/graphic/pipeline/PassTests.cpp",
    "ARTIST/tests/src/graphic/opengl/pipeline/component/attribute/SetterTests.cpp",
    "ARTIST/tests/src/graphic/opengl/pipeline/component/attribute/BinderTests.cpp",
    "ARTIST/tests/src/graphic/opengl/pipeline/component/attribute/UnbinderTests.cpp",
    "ARTIST/tests/src/graphic/opengl/pipeline/component/pass/AttributeReaderTests.cpp",
    "ARTIST/component-tests/src/graphic/shader/PassTests.cpp",
    "ARTIST/component-tests/include/OpenGLComponentTests.hpp",
    "ARTIST/component-tests/CMakeLists.txt",
    "ARTIST/tests/CMakeLists.txt",
    "scripts/build-windows.ps1",
    "CMakePresets.json",
    "conanfile.txt",
]


def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True).stdout.strip()


extra = [line.split("\t")[-1] for line in git("diff", "--name-status", BASE).splitlines() if line]
extra += git("ls-files", "--others", "--exclude-standard").splitlines()
paths = FILES + sorted(path for path in set(extra) if path not in FILES)
head = git("rev-parse", "HEAD")
lines = [
    f"# {sys.argv[2]} identity (pool-6, T2) {datetime.datetime.now().astimezone().isoformat()}",
    f"HEAD {head}",
    f"branch-state {git('status', '--short', '--branch').splitlines()[0]}",
    f"base {BASE}",
    f"commits-since-base {git('rev-list', '--count', f'{BASE}..HEAD')}",
    f"worktree-delta-vs-HEAD-files {len([l for l in git('status', '--short', '--untracked-files=no').splitlines() if l])}",
    f"untracked {len([l for l in git('ls-files', '--others', '--exclude-standard').splitlines() if l])}",
    "# delta vs base (name-status, committed + working tree):",
]
lines += ["  " + line for line in git("diff", "--name-status", BASE).splitlines()] or ["  (none)"]
lines.append("# sha256  raw-bytes-match-HEAD-blob(git hash-object --no-filters)  path")
for path in paths:
    file = ROOT / path
    if not file.exists():
        lines.append(f"{'-' * 64}  absent  {path}")
        continue
    digest = hashlib.sha256(file.read_bytes()).hexdigest()
    at_head = git("rev-parse", f"HEAD:{path}")
    blob = git("hash-object", "--no-filters", path)
    match = "untracked" if not at_head or at_head.startswith("HEAD:") else str(blob == at_head)
    lines.append(f"{digest}  {match}  {path}")
(HERE / sys.argv[1]).write_text("\n".join(lines) + "\n", encoding="utf-8")
print("\n".join(lines))
