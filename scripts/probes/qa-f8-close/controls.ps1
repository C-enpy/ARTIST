param(
    [Parameter(Mandatory=$true)][string]$Exe,
    [Parameter(Mandatory=$true)][string]$Root
)
$ErrorActionPreference = 'Stop'
# Runner lead only. No timeout/kill wrapper; unexpected hang retains ownership.
if ($Root -notmatch '^[A-Za-z]:[\\/]' -or (Test-Path -LiteralPath $Root)) {
    throw 'Controls root must be absolute and absent'
}
if ($Exe -notmatch '^[A-Za-z]:[\\/]' -or -not (Test-Path -LiteralPath $Exe -PathType Leaf)) {
    throw 'Executable must be an absolute existing file'
}
[IO.Directory]::CreateDirectory($Root) | Out-Null
$sentinel = Join-Path $Root 'neighbor.keep'
[IO.File]::WriteAllText($sentinel, 'PC1-CONTROLS-NEIGHBOR')
$hash = (Get-FileHash -LiteralPath $sentinel -Algorithm SHA256).Hash
$cases = @(
    @{Name='unknown'; Args=@('--unknown','x')},
    @{Name='relative'; Args=@('--root','relative','--run-id','controls')},
    @{Name='duplicate'; Args=@('--root',(Join-Path $Root 'not-created'),'--run-id','controls','--run-id','twice')},
    @{Name='existing'; Args=@('--root',$Root,'--run-id','controls')},
    @{Name='child-incomplete'; Args=@('--internal-child','--root',$Root,'--run-id','controls')}
)
foreach ($case in $cases) {
    $argv = $case.Args
    & $Exe @argv
    $code = $LASTEXITCODE
    if ($code -ne 20) { throw "Control $($case.Name): expected20, got$code; STOP" }
    $entries = @(Get-ChildItem -LiteralPath $Root -Force)
    if ($entries.Count -ne 1 -or $entries[0].Name -ne 'neighbor.keep' -or
        (Get-FileHash -LiteralPath $sentinel -Algorithm SHA256).Hash -ne $hash) {
        throw "Control $($case.Name): side effect; STOP"
    }
    Write-Output "PC1_CONTROL name=$($case.Name) exit=$code PASS"
}
# Owned artifacts are intentionally retained for lead inspection.
