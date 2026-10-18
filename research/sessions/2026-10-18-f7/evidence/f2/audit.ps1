$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
Set-Location -LiteralPath $root
$files=@(
 'ARTIST/include/graphic/opengl/context/UniformContext.hpp',
 'ARTIST/include/graphic/opengl/pipeline/component/uniform/Setter.hpp',
 'ARTIST/tests/src/graphic/pipeline/UniformTests.cpp',
 'ARTIST/tests/src/graphic/pipeline/UniformCardinalityTests.cpp',
 'ARTIST/component-tests/src/graphic/shader/UniformCardinalityComponentTests.cpp',
 'test-datas/shaders/vertex/good/f2-single-values.vert',
 'test-datas/shaders/vertex/good/f2-fp64-single-values.vert')
$hashes=@($files | ForEach-Object { @{path=$_;sha256=(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash.ToLower()} })
$source=Get-Content -LiteralPath $files[1] -Raw
$matches=[regex]::Matches($source,'struct OpenGLUniformSetter<([^>]+)>\s*\{(.*?)static constexpr GLenum glType', [System.Text.RegularExpressions.RegexOptions]::Singleline)
$inventory=@($matches | ForEach-Object {
 $body=$_.Groups[2].Value
 $gl=[regex]::Match($body,'glUniform\w+\([^;]+;').Value
 $guard=$body.IndexOf('detail::requireSingleValue(uniform);')
 $call=$body.IndexOf($gl)
 @{type=$_.Groups[1].Value; line=($source.Substring(0,$_.Index).Split("`n").Count);gl=$gl;guardBeforeGL=($guard -ge 0 -and $guard -lt $call);literalCount1=($gl -match '^glUniform\w+\(uniform.getUniformID\(\), 1,')}
})
if($inventory.Count -ne 34 -or @($inventory | Where-Object {$_.literalCount1}).Count -ne 30 -or @($inventory | Where-Object {-not $_.guardBeforeGL}).Count -ne 0){throw '34/30/0 invariant failed'}
$junit=@(Get-ChildItem -LiteralPath $PSScriptRoot -Filter '*.xml' | ForEach-Object {
 [xml]$xml=Get-Content -LiteralPath $_.FullName -Raw
 $cases=@($xml.SelectNodes('//testcase'))
 @{file=$_.Name; tests=$cases.Count; failures=@($xml.SelectNodes('//testcase/failure')).Count; errors=@($xml.SelectNodes('//testcase/error')).Count; skipped=@($xml.SelectNodes('//testcase/skipped')).Count; declaredTests=$xml.testsuite.tests;declaredFailures=$xml.testsuite.failures;timestamp=$xml.testsuite.timestamp}
})
$discovery=@{}
foreach($phase in 'red','final'){
 $text=Get-Content -LiteralPath "$PSScriptRoot/discovery-$phase.log" -Raw
 $json=$text.Substring($text.IndexOf('{')) | ConvertFrom-Json
 $units=@($json.tests | Where-Object {$_.name.StartsWith('unit.')}).Count
 $gls=@($json.tests | Where-Object {$_.name.StartsWith('opengl.')}).Count
 $discovery[$phase]=@{total=$json.tests.Count;unit=$units;opengl=$gls}
}
$artifacts=@(Get-ChildItem -LiteralPath $PSScriptRoot -File | Where-Object {$_.Name -ne 'manifest.json'} | ForEach-Object {
 @{path=$_.Name;bytes=$_.Length;modifiedUtc=$_.LastWriteTimeUtc.ToString('o');sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLower()}
})
$manifest=@{head=(git rev-parse HEAD);root=$root;utc=(Get-Date).ToUniversalTime().ToString('o');sources=$hashes;inventory=$inventory;discovery=$discovery;junit=$junit;artifacts=$artifacts}
$manifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath "$PSScriptRoot/manifest.json" -Encoding UTF8
Write-Output ($hashes | ConvertTo-Json)
Write-Output ($junit | ConvertTo-Json)
Write-Output ($discovery | ConvertTo-Json)
[xml]$native=Get-Content -LiteralPath "$PSScriptRoot/green-f2-native.xml" -Raw
foreach($case in $native.SelectNodes('//testcase')){
 Write-Output $case.name
 Write-Output (($case.'system-out' -split "`n" | Where-Object {$_ -match '\[F2|\[OpenGL\]'}) -join "`n")
}
Write-Output 'INVENTORY: 34 specializations / 30 literal counts / 0 missing guards'
