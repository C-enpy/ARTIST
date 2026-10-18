param([string]$Stage, [string]$Command)
$ErrorActionPreference = 'Stop'
$root = (Get-Location).Path
$tools = 'C:\Users\djden\source\repos\ARTIST\out\tooling\build\Scripts'
$vs = 'C:\Program Files\Microsoft Visual Studio\2022\Community'
$envScript = Join-Path $root 'out/mission-f7/te/load-msvc.cmd'
@('@echo off', "call `"$vs\VC\Auxiliary\Build\vcvarsall.bat`" x64 -vcvars_ver=14.44 >nul", 'if errorlevel 1 exit /b %errorlevel%', 'set') | Set-Content -LiteralPath $envScript -Encoding Default
$envLines = & $env:ComSpec /d /c "`"$envScript`""
if ($LASTEXITCODE) { throw 'vcvarsall failed' }
foreach ($line in $envLines) { if ($line -match '^([^=]+)=(.*)$') { [Environment]::SetEnvironmentVariable($Matches[1], $Matches[2], 'Process') } }
$env:PATH = "$tools;$env:PATH"
$env:CONAN_HOME = 'C:\Users\djden\source\repos\ARTIST\out\conan2'
$env:CMAKE_BUILD_PARALLEL_LEVEL = '1'
$log = Join-Path $root "out/mission-f7/te/$Stage.log"
"COMMAND: $Command`nCWD: $root`nMSVC: $env:VCToolsVersion SDK: $env:WindowsSDKVersion" | Out-File -LiteralPath $log -Encoding utf8
$ErrorActionPreference = 'Continue'
& ([scriptblock]::Create($Command)) 2>&1 | Out-File -LiteralPath $log -Encoding utf8 -Append
$result = $LASTEXITCODE
"EXIT_CODE=$result" | Out-File -LiteralPath $log -Encoding utf8 -Append
Write-Output "STAGE=$Stage EXIT=$result LOG=$log"
exit $result
