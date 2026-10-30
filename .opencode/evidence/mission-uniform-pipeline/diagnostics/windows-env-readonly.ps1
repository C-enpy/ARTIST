# Lead-only diagnostic. Fixed readonly .NET calls, no product, child or ENV write.
param([Parameter(Mandatory = $true)][string]$OutputFile)
$initial = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
$initialNull = [object]::ReferenceEquals($initial, $null)
$initialLength = -1
if (-not $initialNull) { $initialLength = $initial.Length }
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
if ([IO.File]::Exists($OutputFile)) { throw 'Diagnostic output already exists.' }
$trace = [Text.StringBuilder]::new()
[void]$trace.AppendLine('initial|null=' + $initialNull + '|length=' + $initialLength)
$method = [Environment].GetMethod('GetEnvironmentVariable', [type[]]@([string], [EnvironmentVariableTarget]))
if ($null -eq $method) { throw 'Exact public getter unavailable.' }
$typedGetter = [Delegate]::CreateDelegate([Func[string, EnvironmentVariableTarget, string]], $method)
$setup = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
[void]$trace.AppendLine('after-delegate-setup|null=' + [object]::ReferenceEquals($setup, $null))

# Repeated inline observations deliberately avoid a PS trace function.
$typedNull = [object]::ReferenceEquals($typedGetter.Invoke('ARTIST_NATIVE_EMPTY', [EnvironmentVariableTarget]::Process), $null)
$typedEmpty = [StringComparer]::Ordinal.Equals($typedGetter.Invoke('ARTIST_NATIVE_EMPTY', [EnvironmentVariableTarget]::Process), [string]::Empty)
$block = [Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)
$present = $block.Contains('ARTIST_NATIVE_EMPTY')
$blockNull = [object]::ReferenceEquals($block.get_Item('ARTIST_NATIVE_EMPTY'), $null)
$blockEmpty = [StringComparer]::Ordinal.Equals($block.get_Item('ARTIST_NATIVE_EMPTY'), [string]::Empty)
$keepOK = [StringComparer]::Ordinal.Equals($block.get_Item('ARTIST_NATIVE_KEEP'), 'owned')
$absentOK = -not $block.Contains('ARTIST_NATIVE_ABSENT')
$block = $null
$reread = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
[void]$trace.AppendLine('caller-before|typedNull=' + $typedNull + '|typedEmpty=' + $typedEmpty + '|present=' + $present + '|blockNull=' + $blockNull + '|blockEmpty=' + $blockEmpty + '|rereadNull=' + [object]::ReferenceEquals($reread, $null) + '|keep=' + $keepOK + '|absent=' + $absentOK)

function Get-OwnedLiteral {
    $assigned = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
    $typedNull = [object]::ReferenceEquals($script:typedGetter.Invoke('ARTIST_NATIVE_EMPTY', [EnvironmentVariableTarget]::Process), $null)
    $typedEmpty = [StringComparer]::Ordinal.Equals($script:typedGetter.Invoke('ARTIST_NATIVE_EMPTY', [EnvironmentVariableTarget]::Process), [string]::Empty)
    $block = [Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)
    $present = $block.Contains('ARTIST_NATIVE_EMPTY')
    $blockNull = [object]::ReferenceEquals($block.get_Item('ARTIST_NATIVE_EMPTY'), $null)
    $blockEmpty = [StringComparer]::Ordinal.Equals($block.get_Item('ARTIST_NATIVE_EMPTY'), [string]::Empty)
    $keepOK = [StringComparer]::Ordinal.Equals($block.get_Item('ARTIST_NATIVE_KEEP'), 'owned')
    $absentOK = -not $block.Contains('ARTIST_NATIVE_ABSENT')
    $block = $null
    $reread = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
    [void]$script:trace.AppendLine('callee-entry|assignedNull=' + [object]::ReferenceEquals($assigned, $null) + '|typedNull=' + $typedNull + '|typedEmpty=' + $typedEmpty + '|present=' + $present + '|blockNull=' + $blockNull + '|blockEmpty=' + $blockEmpty + '|rereadNull=' + [object]::ReferenceEquals($reread, $null) + '|keep=' + $keepOK + '|absent=' + $absentOK)
    return 'owned literal'
}
# Invalid initial/setup observations are persisted, not silently repaired.
# The preceding run showed the assigned initial getter contradicting both
# readonly observers. Keep that result in the trace; this follow-up admits the
# function only on agreement of typed getter AND key/value snapshot, not by
# coercing null to empty or changing a product assertion.
if ($typedNull -or -not $keepOK -or -not $absentOK -or -not $present -or $blockNull -or -not $blockEmpty -or -not $typedEmpty) {
    [void]$trace.AppendLine('PRECONDITION_INVALID|function-not-called')
} else {
    $returned = Get-OwnedLiteral
    $assigned = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
    $typedNull = [object]::ReferenceEquals($typedGetter.Invoke('ARTIST_NATIVE_EMPTY', [EnvironmentVariableTarget]::Process), $null)
    $typedEmpty = [StringComparer]::Ordinal.Equals($typedGetter.Invoke('ARTIST_NATIVE_EMPTY', [EnvironmentVariableTarget]::Process), [string]::Empty)
    $block = [Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)
    $present = $block.Contains('ARTIST_NATIVE_EMPTY')
    $blockNull = [object]::ReferenceEquals($block.get_Item('ARTIST_NATIVE_EMPTY'), $null)
    $blockEmpty = [StringComparer]::Ordinal.Equals($block.get_Item('ARTIST_NATIVE_EMPTY'), [string]::Empty)
    $keepOK = [StringComparer]::Ordinal.Equals($block.get_Item('ARTIST_NATIVE_KEEP'), 'owned')
    $absentOK = -not $block.Contains('ARTIST_NATIVE_ABSENT')
    $block = $null
    $reread = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
    [void]$trace.AppendLine('caller-after|assignedNull=' + [object]::ReferenceEquals($assigned, $null) + '|typedNull=' + $typedNull + '|typedEmpty=' + $typedEmpty + '|present=' + $present + '|blockNull=' + $blockNull + '|blockEmpty=' + $blockEmpty + '|rereadNull=' + [object]::ReferenceEquals($reread, $null) + '|keep=' + $keepOK + '|absent=' + $absentOK)
    [void]$trace.AppendLine('literal-equal=' + ($returned -ceq 'owned literal'))
}
$file = [IO.File]::Open($OutputFile, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
try {
    $bytes = ([Text.UTF8Encoding]::new($false)).GetBytes($trace.ToString())
    $file.Write($bytes, 0, $bytes.Length)
} finally { $file.Dispose() }
