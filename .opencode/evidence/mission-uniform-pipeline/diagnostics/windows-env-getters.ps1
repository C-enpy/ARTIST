# Lead-owned diagnostic only; no helper, child, process start or environment write.
param(
    [ValidateSet('return', 'getters', 'dot')][string]$Mode,
    [Parameter(Mandatory = $true)][string]$OutputFile
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
if ([IO.File]::Exists($OutputFile)) { throw 'Diagnostic output already exists.' }

# Initial observation precedes allocations and function/pipeline calls.
$initial = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
$initialNull = [object]::ReferenceEquals($initial, $null)
$initialLength = -1
if (-not $initialNull) { $initialLength = $initial.Length }
$trace = New-Object Text.StringBuilder
[void]$trace.AppendLine('initial|null=' + $initialNull + '|length=' + $initialLength)
if ($Mode -eq 'return' -or $Mode -eq 'dot') {
    $direct = 'owned literal'
    $before = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
    [void]$trace.AppendLine('direct|null=' + [object]::ReferenceEquals($before, $null))
    function Get-OwnedLiteral {
        $inside = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
        [void]$script:trace.AppendLine('callee-entry|null=' + [object]::ReferenceEquals($inside, $null))
        $beforeReturn = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
        [void]$script:trace.AppendLine('callee-before-return|null=' + [object]::ReferenceEquals($beforeReturn, $null))
        return 'owned literal'
    }
    if ($Mode -eq 'dot') { $returned = . Get-OwnedLiteral }
    else { $returned = Get-OwnedLiteral }
    $after = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
    [void]$trace.AppendLine('function-return|null=' + [object]::ReferenceEquals($after, $null))
    [void]$trace.AppendLine('return-equal=' + ($direct -ceq $returned))
} else {
    $owned = New-Object Diagnostics.Process
    try {
        foreach ($property in @('Id', 'HasExited')) {
            try {
                $value = $owned.$property
                [void]$trace.AppendLine($property + '-adapter|null=' + [object]::ReferenceEquals($value, $null))
            } catch {
                [void]$trace.AppendLine($property + '-adapter|exception=' + $_.Exception.GetBaseException().GetType().FullName)
            }
            try {
                if ($property -eq 'Id') { $value = $owned.get_Id() }
                else { $value = $owned.get_HasExited() }
                [void]$trace.AppendLine($property + '-getter|null=' + [object]::ReferenceEquals($value, $null))
            } catch {
                [void]$trace.AppendLine($property + '-getter|exception=' + $_.Exception.GetBaseException().GetType().FullName)
            }
        }
    } finally { $owned.Dispose() }
}
$final = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
[void]$trace.AppendLine('final|null=' + [object]::ReferenceEquals($final, $null))
[void]$trace.AppendLine('keep-valid=' + ([Environment]::GetEnvironmentVariable('ARTIST_NATIVE_KEEP', 'Process') -ceq 'owned'))
[void]$trace.AppendLine('absent-valid=' + [object]::ReferenceEquals([Environment]::GetEnvironmentVariable('ARTIST_NATIVE_ABSENT', 'Process'), $null))
[IO.File]::WriteAllText($OutputFile, $trace.ToString(), (New-Object Text.UTF8Encoding($false)))
