param([ValidateSet('configure','build','discover','test','inventory','identity')][string]$Action,
      [string]$Name, [string]$Label='unit', [string]$Regex='')
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$canonical='C:/Users/djden/source/repos/ARTIST'
$build="$canonical/out/mission-f7/f2-build"
$tooling="$canonical/out/tooling"
$env:PATH="$tooling/build/Scripts;"+$env:PATH
$env:CONAN_HOME="$canonical/out/conan2"
$env:CMAKE_BUILD_PARALLEL_LEVEL='1'
$env:CTEST_OUTPUT_SIZE_PASSED='65536'
$env:CTEST_OUTPUT_SIZE_FAILED='65536'
$dump=& $env:ComSpec /d /c "`"$tooling/load-msvc.cmd`""
if($LASTEXITCODE -ne 0){throw 'vcvars failed'}
foreach($line in $dump){if($line -match '^([^=]+)=(.*)$'){[Environment]::SetEnvironmentVariable($Matches[1],$Matches[2],'Process')}}
Set-Location -LiteralPath $root
function Checked([string]$Program,[string[]]$ArgList){
    Write-Output ('COMMAND: '+$Program+' '+($ArgList -join ' '))
    & $Program @ArgList
    if($LASTEXITCODE -ne 0){throw "$Program exit=$LASTEXITCODE"}
}
switch($Action){
 'configure' {
    Checked cmake @('--version'); Checked conan @('--version'); Checked ninja @('--version')
    Write-Output "MSVC=$env:VCToolsVersion SDK=$env:WindowsSDKVersion ROOT=$root"
    $vs='C:/Program Files/Microsoft Visual Studio/18/Community'
    $profile="$root/conan/profiles/windows-msvc"
    Checked conan @('install',$root,'-pr:h',$profile,'-pr:b',$profile,'-s:h','build_type=Debug','-s:b','build_type=Debug','-c:h',"tools.microsoft.msbuild:installation_path=$vs",'-c:b',"tools.microsoft.msbuild:installation_path=$vs",'-c:h','tools.cmake.cmaketoolchain:user_presets=','-c:h','tools.build:jobs=1','-of',$build,'--build=missing')
    Checked cmake @('-S',$root,'-B',$build,'-G','Ninja',"-DCMAKE_TOOLCHAIN_FILE=$build/conan_toolchain.cmake",'-DCMAKE_BUILD_TYPE=Debug','-DBUILD_TESTING=ON','-DMOCK_GL=ON','-DCOVERAGE=OFF','-DARTIST_ENABLE_ASAN=OFF')
 }
 'build' { Checked cmake @('--build',$build,'--parallel','1') }
 'discover' { Checked ctest @('--test-dir',$build,'--show-only=json-v1') }
 'test' {
    $args=@('--test-dir',$build,'-j','1','-L',$Label,'--output-on-failure','--no-tests=error','--output-junit',"$PSScriptRoot/$Name.xml",'--test-output-size-passed','65536','--test-output-size-failed','65536')
    if($Regex){$args+=@('-R',$Regex)}
    Checked ctest $args
 }
 'inventory' {
    $p='ARTIST/include/graphic/opengl/pipeline/component/uniform/Setter.hpp'
    $lines=Get-Content -LiteralPath $p
    $specializations=0; $calls=0; $guards=0; $counts=0
    for($i=0;$i -lt $lines.Length;$i++){
       $l=$lines[$i]
       if($l -match 'struct OpenGLUniformSetter<'){ $specializations++; Write-Output "$($i+1): $($l.Trim())" }
       if($l -match 'glUniform\w+\('){$calls++; Write-Output "$($i+1): $($l.Trim())"}
       if($l -match 'requireSingleValue\(uniform\)'){$guards++}
       if($l -match 'glUniform\w+\(uniform.getUniformID\(\), 1,'){$counts++}
    }
    $reflectedCounts=(Select-String -LiteralPath $p -Pattern 'glUniform\w+\(uniform.getUniformID\(\), uniform.getGLSize\(\)').Count
    Write-Output "specializations=$specializations calls=$calls guards=$guards literalCounts=$counts glReflectedCounts=$reflectedCounts"
 }
 'identity' {
    git rev-parse HEAD; git status --short; git diff --stat
    $files=@(git ls-files ARTIST/include/graphic/opengl/context/UniformContext.hpp ARTIST/include/graphic/opengl/pipeline/component/uniform/Setter.hpp ARTIST/tests/src/graphic/pipeline/UniformTests.cpp)
    $files+=@(Get-ChildItem -Recurse ARTIST -File | Where-Object {$_.Name -like '*UniformCardinality*' -or $_.Name -like 'f2-*'} | ForEach-Object {$_.FullName})
    $files+=@('test-datas/shaders/vertex/good/f2-single-values.vert','test-datas/shaders/vertex/good/f2-fp64-single-values.vert')
    Get-FileHash -LiteralPath $files -Algorithm SHA256 | Format-List
 }
}
