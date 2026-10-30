# Proposal for lead-only diagnosis. No product loading, child, environment write,
# Start/Kill, provider-env enumeration or JSON. Run at most two reviewed modes.
param(
    [Parameter(Mandatory = $true)][ValidateSet('function', 'dot', 'return-as-is')][string]$Mode,
    [Parameter(Mandatory = $true)][string]$CaseDirectory
)
$ErrorActionPreference = 'Stop'
$initial = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
$initialNull = [object]::ReferenceEquals($initial, $null)
$initialLength = -1
if (-not $initialNull) { $initialLength = $initial.Length }
$initialKeep = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_KEEP', 'Process') -ceq 'owned'
$initialAbsent = [object]::ReferenceEquals([Environment]::GetEnvironmentVariable('ARTIST_NATIVE_ABSENT', 'Process'), $null)
# First observation precedes trace collection allocation or any function call.
Set-StrictMode -Version 2.0
$postStrict = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
$postStrictNull = [object]::ReferenceEquals($postStrict, $null)
$postStrictLength = -1
if (-not $postStrictNull) { $postStrictLength = $postStrict.Length }
$trace = [Collections.Generic.List[string]]::new()
$trace.Add('initial|null=' + $initialNull + '|length=' + $initialLength + '|keep=' + $initialKeep + '|absent=' + $initialAbsent)
$trace.Add('after-strict|null=' + $postStrictNull + '|length=' + $postStrictLength)
$root = [IO.Path]::GetFullPath($PSScriptRoot + '/../../../out/verification-synthetic/windows-return-boundary-fixtures/')
$root = $root.TrimEnd([char[]]@('\', '/')) + [IO.Path]::DirectorySeparatorChar
$case = [IO.Path]::GetFullPath($CaseDirectory)
if (-not $case.StartsWith($root, [StringComparison]::OrdinalIgnoreCase) -or
    -not [IO.Directory]::Exists($case) -or
    ([IO.File]::GetAttributes($case) -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
    throw 'HARNESS: use a fresh owned existing diagnostic directory, not a reparse point.'
}
$output = [IO.Path]::Combine($case, $Mode + '.trace')
if ([IO.File]::Exists($output) -or [IO.Directory]::Exists($output)) { throw 'HARNESS: never overwrite a prior trace.' }
# Validate destination before opening it. Marker guards below must leave traces;
# an unsafe/invalid output path must still fail closed, never write elsewhere.
$file = [IO.File]::Open($output, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
$writer = $null
$persisted = 0
try {
    $writer = [IO.StreamWriter]::new($file, [Text.UTF8Encoding]::new($false), 1024, $true)
    for ($line = $persisted; $line -lt $trace.Count; $line += 1) { $writer.WriteLine($trace[$line]) }
    $writer.Flush()
    $persisted = $trace.Count
    function Get-OwnedLiteral {
        $inside = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
        $insideNull = [object]::ReferenceEquals($inside, $null)
        $insideLength = -1
        if (-not $insideNull) { $insideLength = $inside.Length }
        $insideKeep = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_KEEP', 'Process') -ceq 'owned'
        $insideAbsent = [object]::ReferenceEquals([Environment]::GetEnvironmentVariable('ARTIST_NATIVE_ABSENT', 'Process'), $null)
        $script:trace.Add('callee-entry|null=' + $insideNull + '|length=' + $insideLength + '|keep=' + $insideKeep + '|absent=' + $insideAbsent)
        # Inline, not a trace function crossing the boundary under investigation.
        $last = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
        $lastNull = [object]::ReferenceEquals($last, $null)
        $lastLength = -1
        if (-not $lastNull) { $lastLength = $last.Length }
        $script:trace.Add('callee-before-return|null=' + $lastNull + '|length=' + $lastLength)
        return 'owned literal'
    }
    $body = ${function:Get-OwnedLiteral}
    $before = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
    $beforeNull = [object]::ReferenceEquals($before, $null)
    $beforeLength = -1
    if (-not $beforeNull) { $beforeLength = $before.Length }
    $beforeKeep = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_KEEP', 'Process') -ceq 'owned'
    $beforeAbsent = [object]::ReferenceEquals([Environment]::GetEnvironmentVariable('ARTIST_NATIVE_ABSENT', 'Process'), $null)
    $trace.Add('caller-before|null=' + $beforeNull + '|length=' + $beforeLength + '|keep=' + $beforeKeep + '|absent=' + $beforeAbsent)
    for ($line = $persisted; $line -lt $trace.Count; $line += 1) { $writer.WriteLine($trace[$line]) }
    $writer.Flush()
    $persisted = $trace.Count
    # Both snapshots are persisted BEFORE rejecting a marker precondition. Never
    # invoke the target/witness on a bad initial OR caller-before environment.
    if ($initialNull -or $initialLength -ne 0 -or -not $initialKeep -or -not $initialAbsent -or
        $beforeNull -or $beforeLength -ne 0 -or -not $beforeKeep -or -not $beforeAbsent) {
        $trace.Add('experiment-not-run|synthetic-marker-precondition-failed')
        throw 'HARNESS: sanitized synthetic-marker precondition failed; no experiment performed; inspect owned trace.'
    }
    switch ($Mode) {
        'function' { $returned = Get-OwnedLiteral }
        'dot' { $returned = . Get-OwnedLiteral }
        'return-as-is' { $returned = $body.InvokeReturnAsIs() }
    }
    $after = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_EMPTY', 'Process')
    $afterNull = [object]::ReferenceEquals($after, $null)
    $afterLength = -1
    if (-not $afterNull) { $afterLength = $after.Length }
    $afterKeep = [Environment]::GetEnvironmentVariable('ARTIST_NATIVE_KEEP', 'Process') -ceq 'owned'
    $afterAbsent = [object]::ReferenceEquals([Environment]::GetEnvironmentVariable('ARTIST_NATIVE_ABSENT', 'Process'), $null)
    $trace.Add('caller-after|null=' + $afterNull + '|length=' + $afterLength + '|keep=' + $afterKeep + '|absent=' + $afterAbsent)
    $trace.Add('literal-equal=' + ($returned -ceq 'owned literal'))
}
finally {
    # Raw tags/booleans/lengths, including errors before a target call. No JSON,
    # trace helper function or environment restoration. Retain partial IO proof.
    try {
        if ($null -ne $writer) {
            for ($line = $persisted; $line -lt $trace.Count; $line += 1) { $writer.WriteLine($trace[$line]) }
            $writer.Flush()
        }
    }
    finally {
        try { if ($null -ne $writer) { $writer.Dispose() } }
        finally { $file.Dispose() }
    }
}
