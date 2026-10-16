param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug',
    [ValidateRange(1, 64)][int]$Jobs = 1,
    [string]$VisualStudioPath,
    [string]$Python = 'python',
    [ValidateSet('all', 'unit', 'opengl')][string]$TestLabel = 'all',
    [switch]$AddressSanitizer,
    [switch]$ConfigureOnly,
    [switch]$AsanPreflightOnly
)

$ErrorActionPreference = 'Stop'
if ($AddressSanitizer -and $Configuration -ne 'Debug') {
    throw 'AddressSanitizer currently supports Debug only; use its separate build tree.'
}
if ($AsanPreflightOnly -and (-not $AddressSanitizer -or $ConfigureOnly)) {
    throw 'AsanPreflightOnly requires AddressSanitizer and cannot be combined with ConfigureOnly.'
}
$root = Split-Path $PSScriptRoot -Parent
$tooling = Join-Path $root 'out/tooling'
$venvPython = Join-Path $tooling 'build/Scripts/python.exe'
$preset = 'windows-msvc-' + $Configuration.ToLowerInvariant()
if ($AddressSanitizer) { $preset += '-asan' }
$buildDir = Join-Path $root "out/build/$preset"
$savedEnvironment = @{}
# Q1: the ASan policy of every instrumented child (GoogleTest discovery, the
# canary itself, CTest and its test processes) must not depend on the parent
# session. The module lives next to this script.
. (Join-Path $PSScriptRoot 'asan-env.ps1')

function Invoke-Checked([string]$Program, [string[]]$Arguments) {
    & $Program @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$Program failed with exit code $LASTEXITCODE" }
}

function Set-BuildEnvironment([string]$Name, [string]$Value) {
    if (-not $savedEnvironment.ContainsKey($Name)) {
        $savedEnvironment[$Name] = [Environment]::GetEnvironmentVariable($Name, 'Process')
    }
    [Environment]::SetEnvironmentVariable($Name, $Value, 'Process')
}

Push-Location $root
try {
    if (-not (Test-Path $venvPython)) {
        Invoke-Checked $Python @('-m', 'venv', (Join-Path $tooling 'build'))
    }
    if (-not (Test-Path (Join-Path $tooling 'build/Scripts/pip.exe'))) {
        Invoke-Checked $venvPython @('-m', 'ensurepip', '--upgrade')
    }
    Invoke-Checked $venvPython @('-m', 'pip', 'install', '--disable-pip-version-check', '-r', 'scripts/build-requirements.txt')

    if (-not $VisualStudioPath) {
        $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
        if (-not (Test-Path $vswhere)) { throw 'Visual Studio Installer / vswhere is required.' }
        $installations = & $vswhere -sort -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        foreach ($installation in $installations) {
            if (Test-Path (Join-Path $installation 'VC/Tools/MSVC/14.44.*')) {
                $VisualStudioPath = $installation
                break
            }
        }
    }
    if (-not $VisualStudioPath -or -not (Test-Path (Join-Path $VisualStudioPath 'VC/Tools/MSVC/14.44.*'))) {
        throw 'Install the MSVC v143 14.44 x64/x86 toolset and Windows SDK in Visual Studio Installer.'
    }

    $vcvars = Join-Path $VisualStudioPath 'VC/Auxiliary/Build/vcvarsall.bat'
    $envScript = Join-Path $tooling 'load-msvc.cmd'
    @('@echo off', "call `"$vcvars`" x64 -vcvars_ver=14.44 >nul", 'if errorlevel 1 exit /b %errorlevel%', 'set') |
        Set-Content -LiteralPath $envScript -Encoding Default
    $environmentLines = & $env:ComSpec /d /c "`"$envScript`""
    if ($LASTEXITCODE -ne 0) { throw 'vcvarsall failed.' }
    foreach ($line in $environmentLines) {
        if ($line -match '^([^=]+)=(.*)$') { Set-BuildEnvironment $Matches[1] $Matches[2] }
    }
    Set-BuildEnvironment 'PATH' ((Join-Path $tooling 'build/Scripts') + ';' + $env:PATH)
    Set-BuildEnvironment 'CONAN_HOME' (Join-Path $root 'out/conan2')
    Set-BuildEnvironment 'CMAKE_BUILD_PARALLEL_LEVEL' "$Jobs"

    Invoke-Checked 'cmake' @('--version')
    Invoke-Checked 'conan' @('--version')
    Invoke-Checked 'ninja' @('--version')
    Write-Output "MSVC $env:VCToolsVersion | Windows SDK $env:WindowsSDKVersion"

    if ($AddressSanitizer) {
        # Q1: fix the instrumented runtime policy for this whole session before
        # any child runs (build-time GoogleTest discovery included); the canary
        # re-applies the same values for its own probe runs.
        Set-AsanTestEnvironment
    }

    $profile = Join-Path $root 'conan/profiles/windows-msvc'
    Invoke-Checked 'conan' @('install', '.', '-pr:h', $profile, '-pr:b', $profile,
        '-s:h', "build_type=$Configuration", '-s:b', "build_type=$Configuration",
        '-c:h', "tools.microsoft.msbuild:installation_path=$VisualStudioPath",
        '-c:b', "tools.microsoft.msbuild:installation_path=$VisualStudioPath",
        # Versioned project presets already select each toolchain. Conan's auto-
        # include would introduce duplicate conan-debug presets for OFF/ON trees.
        '-c:h', 'tools.cmake.cmaketoolchain:user_presets=',
        '-c:h', "tools.build:jobs=$Jobs", '-of', $buildDir, '--build=missing')
    Invoke-Checked 'cmake' @('--fresh', '--preset', $preset)
    if ($ConfigureOnly) { return }
    if ($AddressSanitizer) {
        Invoke-Checked 'cmake' @('--build', '--preset', $preset, '--target', 'artist_asan_probe', '--parallel', "$Jobs")
        $preflightLogDir = Join-Path $buildDir ('asan-preflight/' + (Get-Date -Format 'yyyyMMdd-HHmmssfff'))
        Invoke-Checked $venvPython @('scripts/verify_asan.py', (Join-Path $buildDir 'artist_asan_probe.exe'),
            '--log-dir', $preflightLogDir)
        if ($AsanPreflightOnly) { return }
    }
    Invoke-Checked 'cmake' @('--build', '--preset', $preset, '--parallel', "$Jobs")
    $testArguments = @('--preset', $preset, '--output-junit', (Join-Path $buildDir 'ctest.xml'))
    if ($TestLabel -ne 'all') { $testArguments += @('-L', $TestLabel) }
    Invoke-Checked 'ctest' $testArguments
} finally {
    foreach ($name in $savedEnvironment.Keys) {
        [Environment]::SetEnvironmentVariable($name, $savedEnvironment[$name], 'Process')
    }
    Pop-Location
}
