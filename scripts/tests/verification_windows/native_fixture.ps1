# Consolidated fixture proposal. Lead must review source pin + OS boundaries.
# Never run/dot-source the complete build helper, cmd/vcvars or a build tool.
param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$payload = Get-Content -LiteralPath $InputFile -Raw | ConvertFrom-Json
$case = [IO.Path]::GetFullPath([string]$payload.case_directory)
$approvedRoot = [IO.Path]::GetFullPath($PSScriptRoot + '/../../../out/verification-synthetic/windows-native-fixtures/')
$approvedRoot = $approvedRoot.TrimEnd([char[]]@('\', '/')) + [IO.Path]::DirectorySeparatorChar
if (-not $case.StartsWith($approvedRoot, [StringComparison]::OrdinalIgnoreCase) -or
    [IO.Path]::GetFullPath($InputFile) -ine ($case + '\input.json') -or
    [IO.Path]::GetFullPath($OutputFile) -ine ($case + '\result.json')) {
    throw 'HARNESS: files outside their owned native fixture paths.'
}
$child = [IO.Path]::GetFullPath($PSScriptRoot + '/wrapper_child.py')
if ([IO.Path]::GetFullPath([string]$payload.child) -ine $child -or
    [IO.Path]::GetFileName([string]$payload.python) -ine 'python.exe' -or
    (Get-FileHash -LiteralPath $payload.python -Algorithm SHA256).Hash -ine $payload.python_sha256 -or
    (Get-FileHash -LiteralPath $child -Algorithm SHA256).Hash -ine $payload.child_sha256) {
    throw 'HARNESS: owned Python/child identity mismatch.'
}
$nativeSource = [string]$payload.native_source
$pin = (Get-Content -LiteralPath ($PSScriptRoot + '/native_source.sha256') -Raw).Trim()
$sha = [Security.Cryptography.SHA256]::Create()
try {
    $hash = ([BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes($nativeSource)))).Replace('-', '').ToLowerInvariant()
} finally { $sha.Dispose() }
$result = $null; $errorText = $null; $harnessError = $null; $exitCode = 0
$ownedProcess = $null
$ownedAliveAfterCall = $false
$parentNames = @('ARTIST_NATIVE_KEEP', 'ARTIST_NATIVE_EMPTY', 'ARTIST_NATIVE_DROP', 'ARTIST_NATIVE_ONLY')
$expectedParent = @{ ARTIST_NATIVE_KEEP = 'parent'; ARTIST_NATIVE_EMPTY = ''
    ARTIST_NATIVE_DROP = 'parent-only'; ARTIST_NATIVE_ONLY = $null }
$psParent = @{}
$parentAfter = @{}
$assignedParentBefore = @{}
$parentObservation = @{ before = @{}; after = @{} }
$typedParentGetter = $null
foreach ($name in $parentNames) {
    # Characterization only: this assigned getter was shown unstable by the
    # lead. Keep its observation visible, never use it as the parent oracle.
    $assignedParentBefore[$name] = [Environment]::GetEnvironmentVariable($name, 'Process')
}

function Copy-DataMap($Object) {
    $map = @{}
    foreach ($property in $Object.PSObject.Properties) { $map[$property.Name] = $property.Value }
    return $map
}

function Assert-OwnedStartInfo($Info, [string]$CommandLine) {
    if ($Info -isnot [Diagnostics.ProcessStartInfo] -or
        $Info.FileName -cne [string]$script:payload.python -or
        $Info.Arguments -cne $CommandLine -or $Info.WorkingDirectory -ine $script:case -or
        $Info.UseShellExecute -or -not $Info.CreateNoWindow -or
        -not $Info.RedirectStandardOutput -or -not $Info.RedirectStandardError) {
        throw 'HARNESS: start info does not describe the one approved fake.'
    }
    $prefix = '"-I" "-S" "-B" "' + $script:child + '" "__artist_native_fixture_v1__" "' +
        $script:case + '\native-case.json"'
    if ($CommandLine -cne $prefix -and -not $CommandLine.StartsWith($prefix + ' ')) {
        throw 'HARNESS: command prefix is not the finite fake.'
    }
    $expected = Copy-DataMap $script:payload.child_environment
    if ($Info.EnvironmentVariables.Count -ne $expected.Count) {
        throw 'HARNESS: inherited/unexpected child environment entry.'
    }
    foreach ($key in $expected.Keys) {
        if (-not $Info.EnvironmentVariables.ContainsKey($key) -or
            $Info.EnvironmentVariables[$key] -cne [string]$expected[$key]) {
            throw 'HARNESS: explicit child environment differs from its sanitized map.'
        }
    }
}

function New-OwnedFixtureStartInfo {
    $info = New-Object Diagnostics.ProcessStartInfo
    $info.FileName = $script:payload.python; $info.Arguments = $script:payload.command_line
    $info.WorkingDirectory = $script:case; $info.UseShellExecute = $false
    $info.CreateNoWindow = $true
    $info.RedirectStandardOutput = $true; $info.RedirectStandardError = $true
    $info.EnvironmentVariables.Clear()
    foreach ($property in $script:payload.child_environment.PSObject.Properties) {
        $info.EnvironmentVariables[$property.Name] = [string]$property.Value
    }
    return $info
}

try {
    # Exact public readonly getter, no script delegate/body or arbitrary symbol.
    # Fresh Process snapshot extraction is authoritative. Both getter forms
    # remain diagnostics: their disagreement alone is not an ENV precondition
    # failure. Even a rejected pin retains the four-marker comparison.
    $getterMethod = [Environment].GetMethod('GetEnvironmentVariable', [type[]]@([string], [EnvironmentVariableTarget]))
    if ($null -eq $getterMethod) { throw 'HARNESS: exact typed parent getter is unavailable.' }
    $typedParentGetter = [Delegate]::CreateDelegate([Func[string, EnvironmentVariableTarget, string]], $getterMethod)
    [Collections.IDictionary]$beforeBlock = [Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)
    foreach ($name in $parentNames) {
        $present = $beforeBlock.Contains($name)
        $value = $null
        if ($present) { $value = $beforeBlock.get_Item($name) }
        $psParent[$name] = $value # no [string] cast: null absence is not empty
        $typedNull = [object]::ReferenceEquals($typedParentGetter.Invoke($name, [EnvironmentVariableTarget]::Process), $null)
        $typedEmpty = [StringComparer]::Ordinal.Equals($typedParentGetter.Invoke($name, [EnvironmentVariableTarget]::Process), [string]::Empty)
        $typedMatches = [StringComparer]::Ordinal.Equals($typedParentGetter.Invoke($name, [EnvironmentVariableTarget]::Process), $value)
        $blockNull = [object]::ReferenceEquals($value, $null)
        $blockEmpty = [StringComparer]::Ordinal.Equals($value, [string]::Empty)
        $sanitizedInitial = ($present -eq ($name -cne 'ARTIST_NATIVE_ONLY')) -and
            [StringComparer]::Ordinal.Equals($value, $expectedParent[$name])
        $assignedLength = -1
        if ($assignedParentBefore[$name] -is [string]) { $assignedLength = $assignedParentBefore[$name].Length }
        $parentObservation.before[$name] = @{
            assigned_null = [object]::ReferenceEquals($assignedParentBefore[$name], $null)
            assigned_length = $assignedLength
            assigned_matches_snapshot = [StringComparer]::Ordinal.Equals($assignedParentBefore[$name], $value)
            present = $present; snapshot_null = $blockNull; snapshot_empty = $blockEmpty
            typed_null = $typedNull; typed_empty = $typedEmpty; typed_matches_snapshot = $typedMatches
            sanitized_initial = $sanitizedInitial
        }
        if ($present -and $value -isnot [string]) {
            throw 'HARNESS: parent-before snapshot contains a non-string value; no native function called.'
        }
        if (-not $sanitizedInitial) {
            throw 'HARNESS: sanitized parent marker precondition failed; no native function called.'
        }
    }
    $beforeBlock = $null # never serialize/enumerate the other environment entries
    if ($hash -cne $pin) { throw 'HARNESS: unreviewed native source pin.' }
    $tokens = $null; $errors = $null
    $ast = [Management.Automation.Language.Parser]::ParseInput($nativeSource, [ref]$tokens, [ref]$errors)
    $names = @('Read-VerificationPayloadFile', 'Resolve-VerificationPhaseFlags',
        'New-VerificationNativeStartInfo', 'Invoke-VerificationNativeProcess')
    $seen = @()
    if (@($errors).Count -ne 0 -or $nativeSource.Length -gt 65536 -or @($ast.EndBlock.Statements).Count -ne 4) {
        throw 'HARNESS: native extraction population/parse/bounds mismatch.'
    }
    foreach ($node in $ast.EndBlock.Statements) {
        if ($node -isnot [Management.Automation.Language.FunctionDefinitionAst] -or
            $names -cnotcontains $node.Name -or $seen -contains $node.Name) {
            throw 'HARNESS: only the four reviewed function definitions may load.'
        }
        $seen += $node.Name
    }
    # This pin is an explicit source-review gate, NOT a PowerShell sandbox.
    # A changed GREEN body needs a new separately reviewed pin before any run.
    . ([scriptblock]::Create($nativeSource))
    try {
        $values = Copy-DataMap $payload.flags
        switch ([string]$payload.action) {
            'flags' { $result = Resolve-VerificationPhaseFlags $values }
            'payload' {
                foreach ($path in @($payload.payload_file, $payload.bundle_root)) {
                    if (-not [IO.Path]::GetFullPath([string]$path).StartsWith($case + '\', [StringComparison]::OrdinalIgnoreCase)) {
                        throw 'HARNESS: test payload paths must stay inside this owned case.'
                    }
                }
                if ($payload.PSObject.Properties.Name -contains 'make_owned_junction' -and $payload.make_owned_junction) {
                    # NTFS junction, no cmd/mklink/privilege escalation. Both
                    # link and target are new fixture-owned directories.
                    $target = $case + '\junction-target'
                    [void](New-Item -ItemType Directory -Path $target)
                    [IO.File]::Copy([string]$payload.payload_file, $target + '\input.json', $false)
                    try {
                        [void](New-Item -ItemType Junction -Path ($case + '\bundle\link') -Target $target -ErrorAction Stop)
                    } catch { throw 'HARNESS: owned NTFS junction precondition unavailable.' }
                    $payload.payload_file = $case + '\bundle\link\input.json'
                }
                $result = Read-VerificationPayloadFile $payload.payload_file $payload.bundle_root
            }
            'start' {
                $envMap = Copy-DataMap $payload.child_environment
                $info = New-VerificationNativeStartInfo $payload.python $payload.command_line $case $envMap
                Assert-OwnedStartInfo $info $payload.command_line
                $result = @{ file_name = $info.FileName; arguments = $info.Arguments; environment = @{} }
                foreach ($name in $info.EnvironmentVariables.Keys) { $result.environment[$name] = $info.EnvironmentVariables[$name] }
            }
            'capture' {
                # Exercise capture independently of the new constructor stub.
                $info = New-OwnedFixtureStartInfo
                Assert-OwnedStartInfo $info $payload.command_line
                $ownedProcess = New-Object Diagnostics.Process
                $ownedProcess.StartInfo = $info
                $result = Invoke-VerificationNativeProcess $ownedProcess ($case + '\logs') $payload.timeout_milliseconds
            }
            'composed' {
                $phase = Resolve-VerificationPhaseFlags $values
                if ($phase -cne 'run') { throw 'HARNESS: composed native fixture is the owned run phase only.' }
                $decoded = Read-VerificationPayloadFile $payload.payload_file $payload.bundle_root
                if ($decoded -isnot [hashtable]) { throw 'HARNESS: payload decoder did not return the private map.' }
                $envMap = Copy-DataMap $payload.child_environment
                $info = New-VerificationNativeStartInfo $payload.python $payload.command_line $case $envMap
                Assert-OwnedStartInfo $info $payload.command_line
                $ownedProcess = New-Object Diagnostics.Process
                $ownedProcess.StartInfo = $info
                $result = Invoke-VerificationNativeProcess $ownedProcess ($case + '\logs') $payload.timeout_milliseconds
            }
            'limit' {
                # Constructor only, never launch the long boundary command.
                $envMap = Copy-DataMap $payload.child_environment
                $info = New-VerificationNativeStartInfo $payload.python $payload.command_line $case $envMap
                $result = @{ arguments = $info.Arguments }
            }
            'selfcheck' {
                # Trusted fixture only: independent proof that explicit empty/
                # missing variables and the owned finite receiver work on host.
                $info = New-OwnedFixtureStartInfo
                Assert-OwnedStartInfo $info $payload.command_line
                $process = New-Object Diagnostics.Process
                $process.StartInfo = $info
                try {
                    [void]$process.Start()
                    $stdout = $process.StandardOutput.ReadToEndAsync()
                    $stderr = $process.StandardError.ReadToEndAsync()
                    if (-not $process.WaitForExit(10000)) { throw 'HARNESS: selfcheck execution deadline expired.' }
                    if (-not $stdout.Wait(1000)) { throw 'HARNESS: selfcheck stdout drain deadline expired.' }
                    if (-not $stderr.Wait(1000)) { throw 'HARNESS: selfcheck stderr drain deadline expired.' }
                    $result = @{ returncode = $process.ExitCode }
                } catch {
                    throw ('HARNESS: selfcheck failed: ' + $_.Exception.Message)
                } finally {
                    try {
                        if (-not $process.get_HasExited()) {
                            # One kill request on this owned object only, no
                            # retry loop or arbitrary PID/tree termination.
                            $process.Kill()
                            if (-not $process.WaitForExit(2000)) {
                                throw 'HARNESS: selfcheck owned-child stop deadline expired.'
                            }
                        }
                    } catch {
                        if ($_.Exception.GetBaseException() -isnot [InvalidOperationException]) {
                            throw ('HARNESS: selfcheck cleanup failed: ' + $_.Exception.Message)
                        }
                    } finally { $process.Dispose() }
                }
            }
            default { throw 'HARNESS: unknown native fixture action.' }
        }
    } catch {
        if ($_.Exception.Message.StartsWith('HARNESS:')) {
            $harnessError = $_.Exception.Message; $exitCode = 96
        } else {
            $errorText = $_.Exception.Message; $exitCode = 1
        }
    }
} catch { $harnessError = $_.Exception.Message; $exitCode = 96 }
finally {
    # Source receives this owned object, not an arbitrary PID. Always reclaim
    # it if source capture throws/fails, including a RED refusal before Start.
    if ($null -ne $ownedProcess) {
        try {
            $ownedAliveAfterCall = -not $ownedProcess.get_HasExited()
            if ($ownedAliveAfterCall) {
                $ownedProcess.Kill()
                if (-not $ownedProcess.WaitForExit(2000)) {
                    $harnessError = 'HARNESS: owned-child cleanup deadline expired.'
                    $exitCode = 96
                }
            }
        } catch {
            # Property-get exceptions may be wrapped by PowerShell. An
            # unassociated Process is expected for a RED refusal/preflight.
            if ($_.Exception.GetBaseException() -isnot [InvalidOperationException]) {
                $harnessError = 'HARNESS: cleanup of the owned process failed.'
                $exitCode = 96
            }
        }
        finally { $ownedProcess.Dispose() }
    }
    try {
        $assignedParentAfter = @{}
        foreach ($name in $parentNames) {
            $assignedParentAfter[$name] = [Environment]::GetEnvironmentVariable($name, 'Process')
        }
        [Collections.IDictionary]$afterBlock = [Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)
        foreach ($name in $parentNames) {
            $present = $afterBlock.Contains($name)
            $value = $null
            if ($present) { $value = $afterBlock.get_Item($name) }
            $parentAfter[$name] = $value
            if ($null -eq $typedParentGetter) { throw 'HARNESS: typed parent-after observation is unavailable.' }
            $typedNull = [object]::ReferenceEquals($typedParentGetter.Invoke($name, [EnvironmentVariableTarget]::Process), $null)
            $typedEmpty = [StringComparer]::Ordinal.Equals($typedParentGetter.Invoke($name, [EnvironmentVariableTarget]::Process), [string]::Empty)
            $typedMatches = [StringComparer]::Ordinal.Equals($typedParentGetter.Invoke($name, [EnvironmentVariableTarget]::Process), $value)
            $blockNull = [object]::ReferenceEquals($value, $null)
            $blockEmpty = [StringComparer]::Ordinal.Equals($value, [string]::Empty)
            $assignedLength = -1
            if ($assignedParentAfter[$name] -is [string]) { $assignedLength = $assignedParentAfter[$name].Length }
            $parentObservation.after[$name] = @{
                assigned_null = [object]::ReferenceEquals($assignedParentAfter[$name], $null)
                assigned_length = $assignedLength
                assigned_matches_snapshot = [StringComparer]::Ordinal.Equals($assignedParentAfter[$name], $value)
                present = $present; snapshot_null = $blockNull; snapshot_empty = $blockEmpty
                typed_null = $typedNull; typed_empty = $typedEmpty; typed_matches_snapshot = $typedMatches
            }
            if ($present -and $value -isnot [string]) {
                throw 'HARNESS: parent-after snapshot contains a non-string value.'
            }
        }
        $afterBlock = $null
    } catch {
        if ($null -eq $harnessError) { $harnessError = 'HARNESS: parent observation failed; inspect parent_observation.' }
        else { $harnessError += '; HARNESS: parent observation also failed; inspect parent_observation.' }
        $exitCode = 96
    }
    @{ fixture_version = 1; result = $result; phase_error = $errorText; harness_error = $harnessError
       parent_before = $psParent; parent_after = $parentAfter; parent_observation = $parentObservation
       owned_alive_after_call = $ownedAliveAfterCall } |
        ConvertTo-Json -Depth 16 | Set-Content -LiteralPath $OutputFile -Encoding UTF8
}
exit $exitCode
