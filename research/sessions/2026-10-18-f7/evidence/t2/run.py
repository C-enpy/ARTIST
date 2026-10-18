"""T2 local runner (pool-6 worktree, not versioned). Same procedure as T1 (pool-5).

Loads MSVC x64 toolset 14.44 and reuses IN PLACE (no copy) the main checkout's
tooling venv (conan/ninja) and Conan 2 home, under the T2 build lease.
Usage: python run.py <log-name> <command...>
Logs: <worktree>/out/mission-f7/t2/logs/<log-name>.log
"""
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

sys.stdout.reconfigure(errors="replace")
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
MAIN = Path("C:/Users/djden/source/repos/ARTIST")

result = subprocess.run(["cmd.exe", "/d", "/c", str(HERE / "msvc-env.cmd")],
                        cwd=ROOT, capture_output=True, text=True, errors="replace", timeout=60)
if result.returncode:
    sys.exit("msvc-env failed: " + result.stdout + result.stderr)
env = {key.upper(): value for key, value in os.environ.items()}
for line in result.stdout.splitlines():
    if "=" in line:
        key, value = line.split("=", 1)
        if key and not key.startswith("="):
            env[key.upper()] = value
env["PATH"] = str(MAIN / "out/tooling/build/Scripts") + ";" + env["PATH"]
env["CONAN_HOME"] = str(MAIN / "out/conan2")
env["CMAKE_BUILD_PARALLEL_LEVEL"] = "1"
env["VSLANG"] = "1033"

command = sys.argv[2:]
command[0] = shutil.which(command[0], path=env["PATH"]) or command[0]
log = HERE / "logs" / (sys.argv[1] + ".log")
log.parent.mkdir(parents=True, exist_ok=True)
head = subprocess.run(["git", "rev-parse", "HEAD"], cwd=ROOT, capture_output=True, text=True).stdout.strip()
dirty = subprocess.run(["git", "status", "--short"], cwd=ROOT, capture_output=True, text=True).stdout.strip()
print("COMMAND", subprocess.list2cmdline(command), flush=True)
start = time.monotonic()
with log.open("w", encoding="utf-8") as output:
    output.write(f"# cwd={ROOT}\n# HEAD={head}\n# worktree-status={dirty.replace(chr(10), ' | ') or 'clean'}\n"
                 f"# CONAN_HOME={env['CONAN_HOME']}\n"
                 f"# VCToolsVersion={env.get('VCTOOLSVERSION')} WindowsSDKVersion={env.get('WINDOWSSDKVERSION')}\n"
                 f"# command={subprocess.list2cmdline(command)}\n")
    output.flush()
    result = subprocess.run(command, cwd=ROOT, env=env, stdout=output, stderr=subprocess.STDOUT, timeout=5400)
elapsed = round(time.monotonic() - start, 1)
with log.open("a", encoding="utf-8") as output:
    output.write(f"# exit={result.returncode} seconds={elapsed}\n")
lines = log.read_text(encoding="utf-8", errors="replace").splitlines()
print("EXIT", result.returncode, "SECONDS", elapsed, "LOG", log)
keep = set(range(min(7, len(lines)))) | set(range(max(0, len(lines) - 25), len(lines)))
hits = 0
for index, line in enumerate(lines):
    low = line.lower()
    if ("error" in low or "failed" in low or "static assertion" in low) and hits < 15:
        keep.update(range(max(0, index - 1), min(len(lines), index + 3)))
        hits += 1
for index in sorted(keep):
    print(f"{index + 1}: {lines[index]}")
sys.exit(result.returncode)
