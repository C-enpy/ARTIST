# TEST-OWNED proposed P0–P5 runner. Lead review before execution.
# No full helper, cmd, vcvars, executable, native capture, Start/Kill/WaitForExit.
param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$fixtureInput = Get-Content -LiteralPath $InputFile -Raw | ConvertFrom-Json
$fixtureCase = [IO.Path]::GetFullPath([string]$fixtureInput.case)
$approved = [IO.Path]::GetFullPath($PSScriptRoot + '/../../../out/verification-synthetic/windows-native-binding/').TrimEnd('\') + '\'
if (-not $fixtureCase.StartsWith($approved, [StringComparison]::OrdinalIgnoreCase) -or
    [IO.Path]::GetFullPath($InputFile) -ine ($fixtureCase + '\input.json') -or
    [IO.Path]::GetFullPath($OutputFile) -ine ($fixtureCase + '\result.json')) {
    throw 'HARNESS: exact owned case/input/output required.'
}

# This object cannot start a process; its only lifecycle method is Dispose.
class ArtistBindingMemoryProcess {
    [object]$StartInfo
    [int]$Number
    [int]$DisposeCalls = 0
    [bool]$DisposeThrows = $false
    [bool]$HasExited = $false
    [int]$ExitCode = 0
    [void]Dispose() {
        $this.DisposeCalls += 1
        if ($this.DisposeThrows) { throw 'FixtureDisposeSecondary: owned cleanup failed.' }
    }
}

function Get-BindingFixtureState {
    return $global:ARTIST_NATIVE_BINDING_TEST_STATE_V1
}
function Copy-BindingObject($Object) {
    $map = @{}
    foreach ($property in $Object.PSObject.Properties) { $map[$property.Name] = $property.Value }
    return $map
}
function Assert-BindingOwnedPath([string]$Path) {
    $state = Get-BindingFixtureState
    if (-not [IO.Path]::IsPathRooted($Path) -or
        -not [IO.Path]::GetFullPath($Path).StartsWith($state.case_directory + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw 'HARNESS: fake effect/observation outside owned case.'
    }
}
function Get-BindingHash([string]$Text) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return ([BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes($Text)))).Replace('-', '').ToLowerInvariant() }
    finally { $sha.Dispose() }
}
function Assert-BindingSource([string]$Text, [bool]$DefinitionsOnly) {
    $tokens = $null; $errors = $null
    $ast = [Management.Automation.Language.Parser]::ParseInput($Text, [ref]$tokens, [ref]$errors)
    if ($Text.Length -gt 65536 -or @($errors).Count -ne 0 -or $Text -match '(?im)^\s*#requires\b') {
        throw 'HARNESS: source syntax/bounds changed.'
    }
    if ($DefinitionsOnly) {
        foreach ($node in $ast.EndBlock.Statements) {
            if ($node -isnot [Management.Automation.Language.FunctionDefinitionAst]) { throw 'HARNESS: binding must contain definitions only.' }
        }
    }
    $allowed = @('Join-Path', 'Split-Path', 'Test-Path', 'Get-Item', 'Get-ChildItem',
        'Get-Content', 'Set-Content', 'ConvertTo-Json', 'ConvertFrom-Json', 'New-Object',
        'Write-Output', 'Write-Error', 'Out-Null', 'Get-Variable', 'Set-Variable', 'Remove-Variable',
        'Get-VerificationAbsolutePath', 'Assert-VerificationRegularPath',
        'Select-VerificationEnvironment', 'Write-VerificationToolchainWitness',
        'Read-VerificationPayloadFile', 'Resolve-VerificationPhaseFlags', 'Resolve-VerificationInputs',
        'ConvertTo-VerificationWindowsArgument', 'Get-VerificationProcessEnvironment',
        'Set-VerificationProcessEnvironment', 'Get-VerificationMsvcEnvironment', 'Invoke-VerificationChild',
        'New-VerificationNativeStartInfo', 'Invoke-VerificationNativeProcess',
        'Enter-VerificationContext', 'Get-VerificationContext', 'Exit-VerificationContext',
        'Set-VerificationCommandRole', 'Get-VerificationExporterSource', 'Initialize-VerificationTooling',
        'Get-VerificationTooling', 'Assert-VerificationPreparedTree', 'Read-VerificationEnvironmentLeaf',
        'Write-VerificationEnvironmentLeaf', 'Read-VerificationMsvcEnvironmentLeaf', 'Invoke-VerificationProcessLeaf',
        'New-VerificationProcessObjectLeaf', 'Read-VerificationClockLeaf',
        'Read-VerificationHostObservationLeaf', 'Read-VerificationToolMetadataLeaf')
    foreach ($definition in $ast.FindAll({ param($n) $n -is [Management.Automation.Language.FunctionDefinitionAst] }, $true)) {
        # Names belong to this separately hash-admitted source, not arbitrary
        # commands supplied by a case. Every new source pin still needs review.
        $allowed += $definition.Name
    }
    foreach ($node in $ast.FindAll({ param($n) $true }, $true)) {
        if ($node -is [Management.Automation.Language.CommandAst]) {
            if ($node.InvocationOperator -ne [Management.Automation.Language.TokenKind]::Unknown -or
                $allowed -cnotcontains $node.GetCommandName() -or
                @('Start-Process', 'Invoke-Expression', 'Add-Type', 'cmd', 'conan', 'cmake', 'ctest', 'python', 'pip', 'ninja') -contains $node.GetCommandName()) {
                # Diagnostic data only: never echo Extent.Text, arguments or an
                # evaluated command/environment value. Non-identifier names are
                # deliberately replaced by a constant rather than printed.
                $diagnosticName = $node.GetCommandName()
                if ($null -eq $diagnosticName -or $diagnosticName -cnotmatch '^[A-Za-z][A-Za-z0-9_.-]{0,127}$') {
                    $diagnosticName = '<dynamic-or-nonidentifier>'
                }
                throw ('HARNESS: direct external/evaluated command forbidden; name=' + $diagnosticName +
                    '; operator=' + [string]$node.InvocationOperator)
            }
            if (@('Get-Variable', 'Set-Variable', 'Remove-Variable') -contains $node.GetCommandName()) {
                for ($index = 0; $index -lt $node.CommandElements.Count; $index += 1) {
                    $part = $node.CommandElements[$index]
                    if ($part -is [Management.Automation.Language.CommandParameterAst] -and $part.ParameterName -ieq 'Scope') {
                        $scopeValue = $part.Argument
                        if ($null -eq $scopeValue -and $index + 1 -lt $node.CommandElements.Count) { $scopeValue = $node.CommandElements[$index + 1] }
                        if ($scopeValue -isnot [Management.Automation.Language.StringConstantExpressionAst] -or
                            @('Script', 'Local') -notcontains $scopeValue.Value) {
                            throw 'HARNESS: forbidden production variable scope; use local/script literals.'
                        }
                    }
                }
            }
        }
        if ($node -is [Management.Automation.Language.VariableExpressionAst] -and
            ($node.VariablePath.UserPath -match '^(global|env):')) { throw 'HARNESS: production global/OS environment access forbidden.' }
        if ($node -is [Management.Automation.Language.InvokeMemberExpressionAst] -and
            ($node.Member.Extent.Text -match '^(Start|Kill|WaitForExit|SetEnvironmentVariable)$')) {
            throw 'HARNESS: native lifecycle/OS environment mutation forbidden.'
        }
        if ($node -is [Management.Automation.Language.InvokeMemberExpressionAst] -and
            $node.Expression.Extent.Text -match '^\[scriptblock\]$') { throw 'HARNESS: dynamic source evaluation forbidden.' }
        if ($node -is [Management.Automation.Language.RedirectionAst] -or
            $node -is [Management.Automation.Language.UsingStatementAst]) { throw 'HARNESS: unapproved source construct.' }
    }
    return $ast
}
function Read-BindingHostObservation {
    $state = Get-BindingFixtureState
    $state.observations.Add('host')
    return @{ environment = (Copy-BindingObject $state.input.parent); process_executable = [string]$state.input.paths.host }
}
function Read-BindingToolMetadata([string]$Program) {
    Assert-BindingOwnedPath $Program
    $state = Get-BindingFixtureState
    $known = Copy-BindingObject $state.input.paths
    $name = $null
    foreach ($key in $known.PSBase.Keys) { if ([IO.Path]::GetFullPath($known[$key]) -ieq [IO.Path]::GetFullPath($Program)) { $name = $key } }
    if ($null -eq $name) { throw 'HARNESS: metadata requested for unknown fake file.' }
    $versions = @{ python = '3.10.11'; bootstrap_python = '3.10.11'; pip = '24.0'; conan = '2.12.2'; ninja = '1.13.0'
        cmake = '3.31.7'; cl = '14.44.35207'; host = '5.1'; cmd = '10.0.26100.1'; vswhere = '3.1.7' }
    $state.observations.Add('metadata:' + $name)
    return @{ version = $versions[$name] }
}
function Read-BindingClock {
    $state = Get-BindingFixtureState
    if ($state.clock_index -ge @($state.input.ticks).Count) { throw 'HARNESS: finite fake clock exhausted.' }
    $tick = [long]$state.input.ticks[$state.clock_index]
    $state.clock_index += 1
    $state.clock_reads.Add($tick)
    return $tick
}
function New-BindingMemoryProcess($StartInfo) {
    $state = Get-BindingFixtureState
    Assert-BindingOwnedPath $StartInfo.FileName
    Assert-BindingOwnedPath $StartInfo.WorkingDirectory
    if ($StartInfo.UseShellExecute -or -not $StartInfo.RedirectStandardOutput -or -not $StartInfo.RedirectStandardError) {
        throw 'HARNESS: factory received unsafe StartInfo.'
    }
    $process = [ArtistBindingMemoryProcess]::new()
    $process.StartInfo = $StartInfo
    $process.Number = $state.processes.Count + 1
    $state.processes.Add($process)
    return $process
}
function Invoke-BindingMemoryCapture($Process, [string]$LogDirectory, [int]$TimeoutMilliseconds) {
    $state = Get-BindingFixtureState
    Assert-BindingOwnedPath $LogDirectory
    if ($Process -isnot [ArtistBindingMemoryProcess] -or $TimeoutMilliseconds -lt 1 -or $TimeoutMilliseconds -gt 3600000 -or
        $state.capture_index -ge @($state.input.outcomes).Count) { throw 'HARNESS: invalid/unplanned memory capture.' }
    $role = [IO.Path]::GetFileName($LogDirectory)
    $allowed = @('python-venv', 'python-ensurepip', 'python-pip', 'vswhere', 'msvc-env', 'conan-install', 'cmake-configure', 'cmake-build')
    if ($allowed -cnotcontains $role) { throw 'HARNESS: unknown fake process role.' }
    $out = $LogDirectory + '\stdout.bin'; $err = $LogDirectory + '\stderr.bin'
    if ([IO.File]::Exists($out) -or [IO.File]::Exists($err)) { throw 'HARNESS: fake capture collision; old proof kept.' }
    $outcome = $state.input.outcomes[$state.capture_index]
    $state.capture_index += 1
    $environment = @{}
    $synthetic = Copy-BindingObject $state.input.parent
    foreach ($property in $state.input.msvc.PSObject.Properties) { $synthetic[$property.Name] = $property.Value }
    $synthetic['CONAN_HOME'] = $state.case_directory + '\source tree with spaces\out\conan2'
    $synthetic['CMAKE_BUILD_PARALLEL_LEVEL'] = '3'
    foreach ($key in $Process.StartInfo.EnvironmentVariables.Keys) {
        $value = $Process.StartInfo.EnvironmentVariables[$key]
        # Do not serialize a source bug's real OS secrets into fixture evidence.
        # Unknown names/values fail before recording, with a value-free message.
        if (-not $synthetic.ContainsKey($key)) { throw 'HARNESS: non-synthetic child environment key.' }
        if ($key -ieq 'PATH') {
            foreach ($segment in $value.Split(';')) { if ($segment.Length -gt 0) { Assert-BindingOwnedPath $segment } }
        } elseif ($key -ieq 'CONAN_HOME') {
            if ([IO.Path]::GetFullPath($value) -ine [IO.Path]::GetFullPath($synthetic[$key])) { throw 'HARNESS: non-synthetic child environment path.' }
        } elseif ($key -ieq 'VSINSTALLDIR') {
            # Production publishes the selected normalized installation, not the
            # raw vcvars spelling. Admit only that exact owned synthetic value.
            $selectedInstallation = [string]$state.input.expected_witness.visual_studio_installation
            Assert-BindingOwnedPath $selectedInstallation
            if (-not [StringComparer]::Ordinal.Equals([string]$value, $selectedInstallation)) {
                throw 'HARNESS: non-synthetic child environment value; key=VSINSTALLDIR.'
            }
        } elseif (-not [StringComparer]::OrdinalIgnoreCase.Equals([string]$value, [string]$synthetic[$key])) {
            throw 'HARNESS: non-synthetic child environment value.'
        }
        $environment[$key] = $value
    }
    $state.captures.Add(@{ role = $role; program = $Process.StartInfo.FileName
        command_line = $Process.StartInfo.Arguments; cwd = $Process.StartInfo.WorkingDirectory
        environment = $environment; log_directory = $LogDirectory; timeout_milliseconds = $TimeoutMilliseconds
        process_number = $Process.Number })
    $stdout = [Convert]::FromBase64String([string]$outcome.stdout_base64)
    if ($role -ceq 'vswhere') { $stdout = [Text.Encoding]::UTF8.GetBytes([string]$state.input.expected_witness.visual_studio_installation + [Environment]::NewLine) }
    foreach ($pair in @(@{ path = $out; bytes = $stdout }, @{ path = $err; bytes = [Convert]::FromBase64String([string]$outcome.stderr_base64) })) {
        $stream = [IO.File]::Open($pair.path, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
        try { $stream.Write($pair.bytes, 0, $pair.bytes.Length) } finally { $stream.Dispose() }
    }
    if (-not $outcome.throws -and $outcome.code -eq 0) {
        # These are FILE EFFECTS of a fake external process, never validators.
        # Cache text/placeholder files come solely from explicit synthetic input.
        if ($role -ceq 'python-venv' -or $role -ceq 'python-ensurepip') {
            foreach ($name in @('python', 'pip')) {
                $path = [string]$state.input.paths.$name; Assert-BindingOwnedPath $path
                [void][IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($path))
                [IO.File]::WriteAllBytes($path, [Text.Encoding]::UTF8.GetBytes('TEST DATA ONLY'))
            }
        }
        if ($role -ceq 'python-pip') {
            foreach ($name in @('conan', 'ninja')) {
                $path = [string]$state.input.paths.$name; Assert-BindingOwnedPath $path
                [IO.File]::WriteAllBytes($path, [Text.Encoding]::UTF8.GetBytes('TEST DATA ONLY'))
            }
        }
        if ($role -ceq 'conan-install' -or $role -ceq 'cmake-configure') {
            $tree = [string]$state.input.expected_witness.build_directory; Assert-BindingOwnedPath $tree
            [void][IO.Directory]::CreateDirectory($tree)
            if ($role -ceq 'conan-install') { [IO.File]::WriteAllText($tree + '\conan_toolchain.cmake', '# synthetic data') }
            else {
                [IO.File]::WriteAllText($tree + '\CMakeCache.txt', [string]$state.input.cache_text)
                [IO.File]::WriteAllText($tree + '\build.ninja', '# synthetic data')
            }
        }
    }
    if ($role -ceq 'msvc-env' -and -not $outcome.throws -and $outcome.code -eq 0) {
        # Actual producer exporter body, not a replacement filter/validator.
        $target = [IO.Path]::GetDirectoryName($LogDirectory) + '\_transport\msvc-env.json'
        Assert-BindingOwnedPath $target
        $exportFile = [IO.Path]::GetDirectoryName($target) + '\export-msvc.ps1'
        $cmdFile = [IO.Path]::GetDirectoryName($target) + '\load-msvc.cmd'
        if ((Get-BindingHash ([IO.File]::ReadAllText($exportFile))) -cne [string]$state.input.exporter_sha256) {
            throw 'FixtureExporterMismatch: emitted exporter differs from reviewed literal.'
        }
        $cmdText = [IO.File]::ReadAllText($cmdFile)
        if ($cmdText -match '(?im)^\s*set\s*$' -or
            -not $cmdText.Contains($exportFile) -or -not $cmdText.Contains($target) -or
            -not $cmdText.Contains('vcvarsall.bat') -or -not $cmdText.Contains('-vcvars_ver=14.44')) {
            throw 'FixtureExporterMismatch: fixed vcvars/export transport command changed.'
        }
        Write-VerificationMsvcExport (Copy-BindingObject $state.input.msvc) $target
        if ($state.input.fault -ceq 'transport-lock') {
            $locked = [IO.Path]::GetDirectoryName($target) + '\export-msvc.ps1'
            $state.handles.Add([IO.File]::Open($locked, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::Read))
        }
    }
    $Process.DisposeThrows = [bool]$outcome.dispose_throws
    $Process.HasExited = $null -ne $outcome.code
    if ($null -ne $outcome.code) { $Process.ExitCode = [int]$outcome.code }
    if ($outcome.throws) { throw 'FixtureCaptureFirstCause: partial bytes retained.' }
    if ($outcome.interrupted) { throw (New-Object OperationCanceledException 'FixtureCaptureInterrupted: partial bytes retained.') }
    return @{ returncode = $outcome.code; timed_out = [bool]$outcome.timed_out
        stdout_path = $out; stderr_path = $err }
}
function Invoke-BindingForbidden {
    $state = Get-BindingFixtureState
    $state.forbidden.Add('direct-process-command')
    throw 'HARNESS: forbidden real tool/process command.'
}

$state = @{ input = $fixtureInput; case_directory = $fixtureCase
    observations = (New-Object 'Collections.Generic.List[object]')
    captures = (New-Object 'Collections.Generic.List[object]')
    processes = (New-Object 'Collections.Generic.List[object]')
    clock_reads = (New-Object 'Collections.Generic.List[object]')
    forbidden = (New-Object 'Collections.Generic.List[object]')
    handles = (New-Object 'Collections.Generic.List[object]')
    clock_index = 0; capture_index = 0; value = $null; phase_error = $null; fixed_entry_checked = $false }
$ownedState = $false; $harnessError = $null; $exitCode = 0
# Never serialize this OS snapshot; only equality is published.
[Collections.IDictionary]$osBefore = [Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)
try {
    try {
        if ($null -ne (Get-Variable -Name ARTIST_NATIVE_BINDING_TEST_STATE_V1 -Scope Global -ErrorAction SilentlyContinue)) {
            throw 'HARNESS: isolated fixture state collision.'
        }
        Set-Variable -Name ARTIST_NATIVE_BINDING_TEST_STATE_V1 -Scope Global -Value $state -ErrorAction Stop
        $ownedState = $true
        $pin = Get-Content -LiteralPath ($PSScriptRoot + '/native_binding_admission.json') -Raw | ConvertFrom-Json
        foreach ($name in @('parameters', 'wrappers', 'data', 'root', 'entry')) {
            if ((Get-BindingHash ([string]$fixtureInput.fragments.$name)) -cne [string]$pin.fragments.$name) {
                throw 'HARNESS: fragment identity not reviewed; no evaluation.'
            }
        }
        [void](Assert-BindingSource ([string]$fixtureInput.fragments.root) $false)
        [void](Assert-BindingSource ([string]$fixtureInput.fragments.entry) $false)
        $isBaseline = ([string]$fixtureInput.action).StartsWith('baseline-')
        if ($fixtureInput.action -ceq 'baseline-audit') {
            [void](Assert-BindingSource ([string]$fixtureInput.mutation) $false)
            throw 'HARNESS: baseline mutation unexpectedly passed audit.'
        }
        if (-not $isBaseline -and ($null -eq $pin.fragments.binding -or $null -eq $pin.exporter_sha256 -or
            (Get-BindingHash ([string]$fixtureInput.fragments.binding)) -cne $pin.fragments.binding)) {
            throw 'HARNESS/INCONCLUSIVE: real binding/exporter absent or not reviewed; not RED.'
        }
        if (-not $isBaseline) {
            $bindingAst = Assert-BindingSource ([string]$fixtureInput.fragments.binding) $true
            if (@($bindingAst.EndBlock.Statements | Where-Object { $_.Name -ceq 'Write-VerificationMsvcExport' }).Count -ne 0) {
                throw 'HARNESS: exporter body belongs only to its fixed child script, no shadowing helper definition.'
            }
            $required = @('Enter-VerificationContext', 'Get-VerificationContext', 'Exit-VerificationContext',
                'Set-VerificationCommandRole', 'Get-VerificationExporterSource', 'Initialize-VerificationTooling',
                'Get-VerificationTooling', 'Assert-VerificationPreparedTree', 'Read-VerificationEnvironmentLeaf',
                'Write-VerificationEnvironmentLeaf', 'Read-VerificationMsvcEnvironmentLeaf', 'Invoke-VerificationProcessLeaf',
                'New-VerificationProcessObjectLeaf', 'Read-VerificationClockLeaf',
                'Read-VerificationHostObservationLeaf', 'Read-VerificationToolMetadataLeaf')
            foreach ($name in $required) {
                if (@($bindingAst.EndBlock.Statements | Where-Object { $_.Name -ceq $name }).Count -ne 1) {
                    throw 'HARNESS/INCONCLUSIVE: real binding function population missing; not RED.'
                }
            }
            $getter = @($bindingAst.EndBlock.Statements | Where-Object { $_.Name -ceq 'Get-VerificationExporterSource' })[0]
            $getterBody = @($getter.Body.EndBlock.Statements)
            if ($getterBody.Count -ne 1) { throw 'HARNESS: exporter getter must be one fixed literal.' }
            $pipeline = $getterBody[0]
            if ($pipeline -is [Management.Automation.Language.ReturnStatementAst]) { $pipeline = $pipeline.Pipeline }
            if ($pipeline -isnot [Management.Automation.Language.PipelineAst] -or $pipeline.PipelineElements.Count -ne 1 -or
                $pipeline.PipelineElements[0] -isnot [Management.Automation.Language.CommandExpressionAst] -or
                $pipeline.PipelineElements[0].Expression -isnot [Management.Automation.Language.StringConstantExpressionAst] -or
                (Get-BindingHash $pipeline.PipelineElements[0].Expression.Value) -cne $pin.exporter_sha256) {
                throw 'HARNESS: exporter getter is not the reviewed literal; no invocation.'
            }
        }
        $directory = $fixtureCase + '\source tree with spaces\scripts'
        [void][IO.Directory]::CreateDirectory($directory)
        $program = $directory + '\native-binding-fragment.ps1'
        $newline = [Environment]::NewLine
        $body = @'
$s = Get-BindingFixtureState
$action = [string]$s.input.action
foreach ($alias in (@{
    'New-VerificationProcessObjectLeaf' = 'New-BindingMemoryProcess'
    'Read-VerificationClockLeaf' = 'Read-BindingClock'
    'Read-VerificationHostObservationLeaf' = 'Read-BindingHostObservation'
    'Read-VerificationToolMetadataLeaf' = 'Read-BindingToolMetadata'
    'Invoke-VerificationNativeProcess' = 'Invoke-BindingMemoryCapture'
}).GetEnumerator()) { Set-Alias -Name $alias.Key -Value $alias.Value -Scope Script }
foreach ($name in @('Start-Process', 'Invoke-Expression', 'cmd', 'python', 'conan', 'cmake', 'ctest', 'pip', 'ninja')) {
    Set-Alias -Name $name -Value Invoke-BindingForbidden -Scope Script
}
'@
        $exportSetup = @'
$exporter = Get-VerificationExporterSource
if ((Get-BindingHash $exporter) -cne $s.input.exporter_sha256) { throw 'HARNESS: exporter literal not reviewed.' }
$exportAst = Assert-BindingSource $exporter $false
$definitions = @($exportAst.EndBlock.Statements | Where-Object { $_ -is [Management.Automation.Language.FunctionDefinitionAst] })
if ($definitions.Count -ne 1 -or $definitions[0].Name -cne 'Write-VerificationMsvcExport' -or
    @($exportAst.EndBlock.Statements).Count -ne 2 -or
    $definitions[0].Extent.Text -match '\[Environment\]') { throw 'HARNESS: fixed exporter entry/definition shape changed.' }
$entry = $exportAst.EndBlock.Statements[1]
$entryText = [Text.RegularExpressions.Regex]::Replace($entry.Extent.Text, '\s+', ' ').Trim()
$acceptedEntries = @(
    'Write-VerificationMsvcExport ([Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)) $OutputFile',
    'Write-VerificationMsvcExport -Snapshot ([Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)) -OutputFile $OutputFile')
if ($entry -isnot [Management.Automation.Language.PipelineAst] -or
    $acceptedEntries -cnotcontains $entryText -or $null -eq $exportAst.ParamBlock -or
    $exportAst.ParamBlock.Parameters.Count -ne 1 -or
    $exportAst.ParamBlock.Parameters[0].Name.VariablePath.UserPath -cne 'OutputFile') {
    throw 'HARNESS: exporter entry must call the real body with the exact Process snapshot and output parameter.'
}
if (@($exportAst.FindAll({ param($n) $n -is [Management.Automation.Language.ExpandableStringExpressionAst] }, $true)).Count -ne 0) {
    throw 'HARNESS: expandable exporter source forbidden.'
}
. ([scriptblock]::Create($definitions[0].Extent.Text))
$s.fixed_entry_checked = $true
'@
        $probeActions = @'
$logRoot = [string]$VerificationLogDirectory
if ($action.StartsWith('baseline-')) {
    if ($action -ceq 'baseline-clock') {
        $values = @(); foreach ($unused in $s.input.ticks) { $values += Read-BindingClock }
        $s.value = @{ ticks = $values }
    } else {
        $logs = $logRoot + '\conan-install'; [void][IO.Directory]::CreateDirectory($logs)
        if ($action -ceq 'baseline-collision') { [IO.File]::WriteAllBytes($logs + '\stdout.bin', [Text.Encoding]::UTF8.GetBytes('old proof')) }
        $line = (ConvertTo-VerificationWindowsArgument 'a b') + ' ' + (ConvertTo-VerificationWindowsArgument '') + ' ' +
            (ConvertTo-VerificationWindowsArgument 'semi;literal') + ' ' + (ConvertTo-VerificationWindowsArgument 'tail\')
        $info = New-VerificationNativeStartInfo ([string]$s.input.paths.conan) $line $root (Copy-BindingObject $s.input.parent)
        $process = New-BindingMemoryProcess $info
        $nativeResult = $null; $firstError = $null
        try { $nativeResult = Invoke-BindingMemoryCapture $process $logs 1234 }
        catch { $firstError = $_.Exception.Message }
        finally { try { $process.Dispose() } catch { if ($null -eq $firstError) { $firstError = $_.Exception.Message } } }
        $s.value = @{ returncode = $null; error = $firstError }
        if ($null -ne $nativeResult) { $s.value.returncode = $nativeResult.returncode }
    }
} else {
    # Admission uses REAL flags/payload/type/path functions. Root came from the
    # actual source lines in this probe file, never from test state.
    $effectiveFlags = @{} + $PSBoundParameters
    $phase = Resolve-VerificationPhaseFlags $effectiveFlags
    $decoded = Read-VerificationPayloadFile $VerificationInputFile $VerificationBundleRoot
    $values = Resolve-VerificationInputs $decoded $root $phase $Configuration
    $admission = @{ root = $root; phase = $phase; configuration = $Configuration
        bundle_root = $VerificationBundleRoot; build_directory = $values.build_directory
        log_directory = $VerificationLogDirectory; jobs = $Jobs; timeout_milliseconds = 10000
        python = $Python; visual_studio_installation = $VisualStudioPath }
    if ($action -ceq 'context-missing') {
        $failed = $false; $cause = $null
        try { Read-VerificationEnvironmentLeaf | Out-Null } catch { $failed = $true; $cause = $_.Exception.Message }
        $s.value = @{ contract_satisfied = $failed; context_error = $cause }
    } else {
        Enter-VerificationContext $admission
        try {
            switch ($action) {
                'context-map' {
                    $copy = Read-VerificationEnvironmentLeaf
                    $copy['SystemRoot'] = 'detached change'
                    $detached = (Read-VerificationEnvironmentLeaf)['SystemRoot'] -ceq [string]$s.input.parent.SystemRoot
                    Write-VerificationEnvironmentLeaf 'TEMP' ''
                    Write-VerificationEnvironmentLeaf 'KEEP' 'temporary'
                    Write-VerificationEnvironmentLeaf 'KEEP' $null
                    $after = Read-VerificationEnvironmentLeaf
                    $s.value = @{ detached = $detached; empty = $after['TEMP']; removed_present = $after.ContainsKey('KEEP') }
                }
                'context-reentrant' {
                    $failed = $false; $cause = $null
                    try { Enter-VerificationContext $admission } catch { $failed = $true; $cause = $_.Exception.Message }
                    $s.value = @{ contract_satisfied = $failed; context_error = $cause }
                }
                'context-independent' {
                    Write-VerificationEnvironmentLeaf 'SystemRoot' 'first invocation'
                    Exit-VerificationContext
                    Enter-VerificationContext $admission
                    $s.value = @{ contract_satisfied = ((Read-VerificationEnvironmentLeaf)['SystemRoot'] -ceq [string]$s.input.parent.SystemRoot) }
                }
                'exporter' {
                    $transport = $logRoot + '\_transport'; [void][IO.Directory]::CreateDirectory($transport)
                    $target = $transport + '\msvc-env.json'
                    if ($s.input.exporter_case -ceq 'collision') { [IO.File]::WriteAllBytes($target, [Text.Encoding]::UTF8.GetBytes('old proof')) }
                    $snapshot = Copy-BindingObject $s.input.msvc; $snapshot['TEMP'] = ''
                    $failed = $false
                    try { Write-VerificationMsvcExport $snapshot $target } catch { $failed = $true }
                    $s.value = @{ failed = $failed; fixed_entry_checked = $s.fixed_entry_checked }
                }
                default {
                    $tools = Get-VerificationTooling $root
                    $steps = @('conan-install')
                    if ($action -ceq 'invoke-budget') { $steps = @('conan-install', 'cmake-configure', 'python-pip') }
                    $failed = $false; $firstError = $null; $code = $null
                    foreach ($role in $steps) {
                        try {
                            Set-VerificationCommandRole $role
                            $program = [string]$s.input.paths.conan
                            if ($role -ceq 'cmake-configure') { $program = [string]$s.input.paths.cmake }
                            if ($role -ceq 'python-pip') { $program = [string]$s.input.paths.python }
                            $code = Invoke-VerificationChild $program @('a b', '', 'semi;literal', 'tail\')
                        } catch { $failed = $true; $firstError = $_.Exception.Message; break }
                    }
                    $s.value = @{ returncode = $code; failed = $failed; error = $firstError; expired_refused = $failed }
                }
            }
        } finally { Exit-VerificationContext }
        $absent = $false; try { Get-VerificationContext | Out-Null } catch { $absent = $true }
        if ($null -ne $s.value) { $s.value.context_absent_after_exit = $absent }
    }
}
'@
        $text = [string]$fixtureInput.fragments.parameters + $newline +
            [string]$fixtureInput.fragments.wrappers + $newline + [string]$fixtureInput.fragments.data + $newline
        if (-not $isBaseline) { $text += [string]$fixtureInput.fragments.binding + $newline }
        $text += $body + $newline
        if (-not $isBaseline) {
            # Pass only the reviewed exporter hash, never executable payload code.
            $fixtureInput | Add-Member -MemberType NoteProperty -Name exporter_sha256 -Value $pin.exporter_sha256
            $text += $exportSetup + $newline
        }
        if ($fixtureInput.action -ceq 'entry') {
            $entryFinally = @'
} finally {
    $__artistBindingEntryState = Get-BindingFixtureState
    $stillActive = $true; $contextError = $null
    try { Get-VerificationContext | Out-Null } catch {
        $contextError = $_.Exception.Message
        if ($contextError.Contains('InvalidVerificationContext')) { $stillActive = $false }
    }
    $__artistBindingEntryState.value = @{ context_absent_after_entry = -not $stillActive; context_observation_error = $contextError }
    # An observed leak is retained as FAIL data before test-owned best-effort
    # cleanup. This never turns the source's missing finally into a success.
    if ($stillActive) { try { Exit-VerificationContext } catch { $__artistBindingEntryState.value.context_cleanup_error = $_.Exception.Message } }
}
'@
            $text += 'try {' + $newline + [string]$fixtureInput.fragments.entry + $newline +
                "throw 'HARNESS: real entry fell through legacy'" + $newline + $entryFinally
        } else { $text += [string]$fixtureInput.fragments.root + $newline + $probeActions }
        $tokens = $null; $errors = $null
        [void][Management.Automation.Language.Parser]::ParseInput($text, [ref]$tokens, [ref]$errors)
        if (@($errors).Count -ne 0) { throw 'HARNESS: composed fragment syntax failed; not RED.' }
        [IO.File]::WriteAllText($program, $text, (New-Object Text.UTF8Encoding($false)))
        $flags = Copy-BindingObject $fixtureInput.flags
        $global:LASTEXITCODE = 0
        try { & $program @flags; $exitCode = [int]$LASTEXITCODE }
        catch {
            if ($_.Exception.Message.StartsWith('HARNESS') -or $_.Exception -is [Management.Automation.CommandNotFoundException]) { throw }
            $state.phase_error = $_.Exception.Message; $exitCode = 1
        }
    } catch { $harnessError = $_.Exception.Message; $exitCode = 96 }
} finally {
    try {
        [Collections.IDictionary]$osAfter = [Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)
        $same = $osBefore.Count -eq $osAfter.Count
        foreach ($key in $osBefore.Keys) {
            if (-not $osAfter.Contains($key) -or -not [StringComparer]::Ordinal.Equals($osBefore[$key], $osAfter[$key])) { $same = $false }
        }
        $processes = @()
        foreach ($process in $state.processes) { $processes += @{ number = $process.Number; dispose_calls = $process.DisposeCalls } }
        $report = @{ fixture_version = 1; harness_error = $harnessError; phase_error = $state.phase_error
            value = $state.value; captures = @($state.captures.ToArray()); processes = $processes
            observations = @($state.observations.ToArray()); clock_reads = @($state.clock_reads.ToArray())
            forbidden = @($state.forbidden.ToArray()); os_environment_unchanged = $same }
    } finally {
        try { foreach ($handle in $state.handles) { $handle.Dispose() } }
        finally { if ($ownedState) { Remove-Variable -Name ARTIST_NATIVE_BINDING_TEST_STATE_V1 -Scope Global -ErrorAction Stop } }
    }
    $report.state_removed = ($null -eq (Get-Variable -Name ARTIST_NATIVE_BINDING_TEST_STATE_V1 -Scope Global -ErrorAction SilentlyContinue))
    $report | ConvertTo-Json -Depth 24 | Set-Content -LiteralPath $OutputFile -Encoding UTF8
}
exit $exitCode
