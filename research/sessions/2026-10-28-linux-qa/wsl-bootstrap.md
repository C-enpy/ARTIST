# Ubuntu WSL2 qualification — pool-2, 2026-10-28

## Exercised matrix

- Windows host: WSL 2.6.1.0, kernel 6.6.87.2-1, WSLg 1.0.66.
- New distribution `ARTIST-Ubuntu`: Ubuntu 24.04.5 LTS, x86_64, WSL2.
- GCC/G++ 14.2.0 (`14.2.0-4ubuntu2~24.04.1`), libstdc++6 and libstdc++-14-dev same package version; `__GLIBCXX__=20240908`, `_GLIBCXX_USE_CXX11_ABI=1`, target `x86_64-linux-gnu`, glibc 2.39.
- Python 3.12.3, CMake 3.28.3, Conan 2.12.2, pip Ninja 1.13.0 (`1.13.0.git.kitware.jobserver-pipe-1` executable version).
- Existing C++23 compile/link probe: PASS with `stdc++exp`; no production build or CTest run claimed.

## Isolation and identity

Before mutation: only `docker-desktop` existed (Running, WSL2); I: had 1162.86 GiB free, `I:\WSL\ARTIST-Ubuntu` did not exist. `wsl --help` advertised all installation options used, and `wsl --list --online` listed Ubuntu-24.04.

Executed installation, exit zero, no elevation or restart required:

```powershell
wsl --install Ubuntu-24.04 --name ARTIST-Ubuntu --location I:\WSL\ARTIST-Ubuntu --version 2 --no-launch
```

Registry `HKCU\Software\Microsoft\Windows\CurrentVersion\Lxss` confirms new distribution `BasePath=I:\WSL\ARTIST-Ubuntu`, `Version=2`. `ext4.vhdx` exists there (2327838720 bytes at initial verification). Existing docker-desktop remains registered at `E:\DockerDesktop\DockerDesktopWSL\main`; no terminate/shutdown/unregister/migration performed. WSL warned sparse VHD support was disabled for corruption risk; no unsafe override applied.

Created unprivileged Linux user `artist` (no password/sudo privileges assigned). Invoke explicitly with `wsl -d ARTIST-Ubuntu -u artist --cd /home/artist` (default user not changed).

Checkout `/home/artist/src/ARTIST` is detached at **9bf085075e44cd299e80aca9128af4fd0f7c7777**, clean by `git status --porcelain`. Transfer used `git bundle create ... HEAD` from the assigned clean worktree, then clone, explicit detached checkout and remote removal. No uncommitted Windows change assumed transferred. Windows paths were transfer inputs only. Source, pip cache/venv, future Conan cache and probe build are on Linux ext4 `/dev/sdf`, not drvfs. A newer integration candidate must be transferred explicitly by the next owner.

## Prerequisites actually installed

New distribution root only; Ubuntu repositories, no PPA and no global compiler alternatives changes:

```sh
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y g++-14 libstdc++-14-dev cmake python3-venv pkg-config git ca-certificates libgl1-mesa-dev libglu1-mesa-dev libx11-xcb-dev xvfb libgl1-mesa-dri mesa-utils
useradd --create-home --shell /bin/bash artist
```

Then as artist in checkout:

```sh
python3 -m venv out/tooling/build
out/tooling/build/bin/pip install -r scripts/build-requirements.txt
export PATH="$PWD/out/tooling/build/bin:$PATH"
```

The requirements remain exactly Conan2.12.2/Ninja1.13.0. Transitive pip packages are not fully locked by this existing file.

## Exact qualification command and first cause

Standalone owned probe source `out/probe-source/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.25)
project(ArtistCxx23Qualification LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(/home/artist/src/ARTIST/cmake/CheckCxx23.cmake)
artist_check_cxx23(ARTIST_STACKTRACE_LIBRARY)
message(STATUS "ARTIST_STACKTRACE_LIBRARY=${ARTIST_STACKTRACE_LIBRARY}")
```

Executed once as artist in `/home/artist/src/ARTIST`:

```sh
cmake -S out/probe-source -B out/probe-linux-gcc14-x64-debug -G Ninja -DCMAKE_CXX_COMPILER=/usr/bin/g++-14 -DCMAKE_BUILD_TYPE=Debug >out/logs/cxx23-probe.log 2>&1
```

Exit0; native probe fails to link `std::__stacktrace_impl::_S_current`, then the existing automatic `-lstdc++exp` probe succeeds. This is an expected support-library dependency, not missing C++23 functionality or a suppressed assertion. No code/toolchain workaround needed. Context Mode examined `CMakeConfigureLog.yaml` lines293–322 for the causal linker failure and successful command.

Logs remain in the Linux checkout (`out/logs/cxx23-probe.log`, `out/probe-linux-gcc14-x64-debug/CMakeFiles/CMakeConfigureLog.yaml`), copied also to the assigned Windows worktree `out/wsl-bootstrap/`.

## Handoff limits

CMake, presets, Conan requirements/generators and CI inspected before commands. CI uses GCC14 and Conan2 before CMake; Linux configure preset exists, Linux build/test presets do not. This task only qualifies the standalone existing STL probe: it has no external dependencies and does not configure the production project. Production Conan installation, valid project compile_commands.json, targets/build, CTest discovery/execution, ASan, GPU renderer/context and pixel oracles remain **INCONCLUSIVE/not executed here**, owned by subsequent integration work. No fabricated include paths or Windows build cache reuse. Do not point clangd/Serena at this empty standalone-probe compilation database. Existing `CMakeDeps`/`CMakeToolchain` install-before-configure integration must be used for that next stage. CI access not exercised or implied by WSL availability.

Localisation: toolchain/configuration files only; no C++ symbol changes. Impact: new isolated WSL resource and this report; no production source modified. MCP catalogue temporarily refreshed after moving to assigned worktree, then Context Mode worked there without reconfiguration. Initial batch shell lacked git PATH and WSL UTF16 output was unreadable; corrected with explicit executable paths/UTF16 decoding, no system remedy.
