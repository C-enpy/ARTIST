@echo off
setlocal
set "SRC=C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-artist-mcp-lifecycle-mission-5caa4a18f43401c7#eiyn0g-f1-cpp-author"
set "BUILD=C:/Users/djden/source/repos/ARTIST/out/mission-f7/f1-build"
set "EVIDENCE=%SRC%/out/mission-f7/f1"
call "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Auxiliary/Build/vcvarsall.bat" x64 -vcvars_ver=14.44 >nul
if errorlevel 1 exit /b %errorlevel%
set "PATH=C:/Users/djden/source/repos/ARTIST/out/tooling/build/Scripts;C:/Program Files/CMake/bin;%PATH%"
set "CONAN_HOME=C:/Users/djden/source/repos/ARTIST/out/conan2"
set "CMAKE_BUILD_PARALLEL_LEVEL=1"
if "%~1"=="baseline" goto baseline
if "%~1"=="red" goto red
if "%~1"=="green" goto green
exit /b 64
:baseline
echo MSVC %VCToolsVersion% WindowsSDK %WindowsSDKVersion% > "%EVIDENCE%/environment.log"
cmake --version >> "%EVIDENCE%/environment.log" 2>&1
conan --version >> "%EVIDENCE%/environment.log" 2>&1
ninja --version >> "%EVIDENCE%/environment.log" 2>&1
conan install "%SRC%" -pr:h "%SRC%/conan/profiles/windows-msvc" -pr:b "%SRC%/conan/profiles/windows-msvc" -s:h build_type=Debug -s:b build_type=Debug -c:h "tools.microsoft.msbuild:installation_path=C:/Program Files/Microsoft Visual Studio/18/Community" -c:b "tools.microsoft.msbuild:installation_path=C:/Program Files/Microsoft Visual Studio/18/Community" -c:h tools.cmake.cmaketoolchain:user_presets= -c:h tools.build:jobs=1 -of "%BUILD%" --build=missing > "%EVIDENCE%/baseline-conan.log" 2>&1
if errorlevel 1 exit /b %errorlevel%
cmake -S "%SRC%" -B "%BUILD%" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="%BUILD%/conan_toolchain.cmake" -DBUILD_TESTING=ON -DMOCK_GL=ON -DCOVERAGE=OFF -DARTIST_ENABLE_ASAN=OFF > "%EVIDENCE%/baseline-configure.log" 2>&1
if errorlevel 1 exit /b %errorlevel%
cmake --build "%BUILD%" --target ArtistLib_tests --parallel 1 > "%EVIDENCE%/baseline-build.log" 2>&1
if errorlevel 1 exit /b %errorlevel%
ctest --test-dir "%BUILD%" -L unit --show-only=json-v1 > "%EVIDENCE%/baseline-discovery.json" 2>&1
exit /b %errorlevel%
:red
cmake -S "%SRC%" -B "%BUILD%" > "%EVIDENCE%/red-configure.log" 2>&1
if errorlevel 1 exit /b %errorlevel%
cmake --build "%BUILD%" --target ArtistLib_tests --parallel 1 > "%EVIDENCE%/red-build.log" 2>&1
if errorlevel 1 exit /b %errorlevel%
ctest --test-dir "%BUILD%" -L unit --show-only=json-v1 > "%EVIDENCE%/red-discovery.json" 2>&1
if errorlevel 1 exit /b %errorlevel%
ctest --test-dir "%BUILD%" -L unit -R AttributeContextTests --parallel 1 --output-on-failure --no-tests=error --test-output-size-failed 65536 --test-output-size-passed 65536 --output-junit "%EVIDENCE%/red-junit.xml" > "%EVIDENCE%/red-tests.log" 2>&1
exit /b %errorlevel%
:green
cmake --build "%BUILD%" --target ArtistLib_tests --parallel 1 > "%EVIDENCE%/green-build.log" 2>&1
if errorlevel 1 exit /b %errorlevel%
ctest --test-dir "%BUILD%" -L unit --show-only=json-v1 > "%EVIDENCE%/green-discovery.json" 2>&1
if errorlevel 1 exit /b %errorlevel%
ctest --test-dir "%BUILD%" -L unit -R AttributeContextTests --parallel 1 --output-on-failure --no-tests=error --test-output-size-failed 65536 --test-output-size-passed 65536 --output-junit "%EVIDENCE%/green-targeted-junit.xml" > "%EVIDENCE%/green-targeted.log" 2>&1
if errorlevel 1 exit /b %errorlevel%
ctest --test-dir "%BUILD%" -L unit --parallel 1 --output-on-failure --no-tests=error --test-output-size-failed 65536 --test-output-size-passed 65536 --output-junit "%EVIDENCE%/green-unit-junit.xml" > "%EVIDENCE%/green-unit.log" 2>&1
exit /b %errorlevel%
