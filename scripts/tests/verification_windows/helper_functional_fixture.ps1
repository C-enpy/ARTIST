# TEST-OWNED. Never execute or dot-source the full build helper. Lead runner only.
# One interpreter, no child process. All tool/env effects below are record-only.
param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$fixtureInput = Get-Content -LiteralPath $InputFile -Raw | ConvertFrom-Json
$fixtureCase = [IO.Path]::GetFullPath([string]$fixtureInput.case_directory)
$fixtureApproved = [IO.Path]::GetFullPath(
    $PSScriptRoot + '/../../../out/verification-synthetic/windows-helper-functional/').TrimEnd('\') + '\'
if (-not $fixtureCase.StartsWith($fixtureApproved, [StringComparison]::OrdinalIgnoreCase) -or
    [IO.Path]::GetFullPath($InputFile) -ine ($fixtureCase + '\input.json') -or
    [IO.Path]::GetFullPath($OutputFile) -ine ($fixtureCase + '\result.json')) {
    throw 'HARNESS: owned input/output paths required.'
}
$fixtureError = $null
$fixtureHarnessError = $null
$fixtureExit = 0
$fixtureBaseline = $null
$fixtureOwnsState = $false

function Copy-FixtureObject($Object) {
    $map = @{}
    foreach ($property in $Object.PSObject.Properties) { $map[$property.Name] = $property.Value }
    return $map
}
function Copy-FixtureMap([hashtable]$Map) { return (@{} + $Map) }
function Get-FixtureState {
    # A function invoked from another .ps1 sees that file's $script: scope.
    # Only this explicitly named TEST-OWNED table crosses those script scopes.
    # No root/tooling/venv variable is stored here or injected into a fragment.
    $state = $global:ARTIST_HELPER_FUNCTIONAL_FIXTURE_STATE_V1
    if ($state -isnot [hashtable]) { throw 'HARNESS: fixture state binding missing.' }
    return $state
}
function Get-FixtureHash([string]$Text) {
    $algorithm = [Security.Cryptography.SHA256]::Create()
    try {
        return ([BitConverter]::ToString($algorithm.ComputeHash(
            [Text.Encoding]::UTF8.GetBytes($Text)))).Replace('-', '').ToLowerInvariant()
    } finally { $algorithm.Dispose() }
}
function Assert-FixtureOwnedPath([string]$Path) {
    $state = Get-FixtureState
    if (-not [IO.Path]::IsPathRooted($Path) -or
        -not [IO.Path]::GetFullPath($Path).StartsWith($state.case_directory + '\',
            [StringComparison]::OrdinalIgnoreCase)) { throw 'HARNESS: fixture path escaped case.' }
}
function Assert-FixtureAst([string]$Text, [string]$Kind) {
    $tokens = $null; $errors = $null
    $ast = [Management.Automation.Language.Parser]::ParseInput($Text, [ref]$tokens, [ref]$errors)
    $bound = 32768
    if ($Kind -ceq 'excluded-binding') { $bound = 65536 }
    if ($Text.Length -gt $bound -or @($errors).Count -ne 0 -or
        $Text -match '(?im)^\s*#requires\b') { throw 'HARNESS: fragment syntax/bounds mismatch.' }
    if ($Kind -ceq 'excluded-binding') {
        # The whole hash-admitted BINDING is parsed as DATA only. Excluding it
        # cannot hide executable initialization or move it before admission.
        foreach ($statement in $ast.EndBlock.Statements) {
            if ($statement -isnot [Management.Automation.Language.FunctionDefinitionAst]) {
                throw 'HARNESS: excluded binding is not definitions-only.'
            }
        }
        return
    }
    if ($Kind -eq 'definitions') {
        $expected = @('Resolve-VerificationInputs', 'ConvertTo-VerificationWindowsArgument',
            'Get-VerificationProcessEnvironment', 'Set-VerificationProcessEnvironment',
            'Get-VerificationMsvcEnvironment', 'Invoke-VerificationChild',
            'Read-VerificationPayloadFile', 'Resolve-VerificationPhaseFlags',
            'Get-VerificationAbsolutePath', 'Assert-VerificationRegularPath',
            'Get-VerificationEnvironmentNames', 'Select-VerificationEnvironment')
        $seen = @()
        foreach ($statement in $ast.EndBlock.Statements) {
            if ($statement -isnot [Management.Automation.Language.FunctionDefinitionAst] -or
                $expected -cnotcontains $statement.Name -or $seen -ccontains $statement.Name) {
                throw 'HARNESS: definitions population mismatch.'
            }
            $seen += $statement.Name
        }
        if ($seen.Count -ne $expected.Count) { throw 'HARNESS: incomplete definitions.' }
        return
    }
    if ($Kind -eq 'parameters') {
        if ($null -eq $ast.ParamBlock -or @($ast.EndBlock.Statements).Count -ne 0 -or
            $ast.ParamBlock.Parameters.Count -ne 14) { throw 'HARNESS: parameter population mismatch.' }
        return
    }
    $allowedCommands = @('Split-Path', 'Join-Path', 'Resolve-VerificationPhaseFlags',
        'Read-VerificationPayloadFile', 'Resolve-VerificationInputs',
        'Get-VerificationProcessEnvironment', 'Set-VerificationProcessEnvironment',
        'Get-VerificationMsvcEnvironment', 'Initialize-VerificationTooling',
        'Get-VerificationTooling', 'Assert-VerificationPreparedTree', 'Invoke-VerificationChild',
        'Get-VerificationAbsolutePath', 'Assert-VerificationRegularPath', 'Select-VerificationEnvironment',
        'Enter-VerificationContext', 'Exit-VerificationContext', 'Set-VerificationCommandRole',
        'Write-VerificationToolchainWitness')
    $allowedVariables = @('PSBoundParameters', 'PSScriptRoot', 'VerificationPhase',
        'AllowDependencyResolution', 'Configuration', 'Jobs', 'Python', 'VisualStudioPath',
        'VerificationInputFile', 'VerificationBundleRoot', 'VerificationLogDirectory',
        'VerificationTimeoutSeconds', 'VerificationBuildDirectory', 'VerificationTargets',
        'VerificationExecutable', 'VerificationArguments', 'root', 'tooling', 'venvPython',
        'originalEnvironment', 'msvcEnvironment', 'profile', 'preset', 'arguments',
        'childCode', 'flags', 'phase', 'decoded', 'validated', 'tools', 'name',
        'toolEnvironment', 'selected', 'true', 'false', 'null',
        'verificationRequested', 'verificationFlags', 'admittedPhase', 'bundlePath', 'phaseLog',
        'verificationValues', 'preparedWitness', 'admission', 'args', 'verificationEnvironmentAttempted')
    $nodes = @($ast.FindAll({ param($node) $true }, $true))
    if ($nodes.Count -gt 4096) { throw 'HARNESS: entry AST bounds exceeded.' }
    foreach ($node in $nodes) {
        if ($node -is [Management.Automation.Language.CommandAst] -and
            ($node.InvocationOperator -ne [Management.Automation.Language.TokenKind]::Unknown -or
             $allowedCommands -cnotcontains $node.GetCommandName())) {
            throw 'HARNESS: command outside record-only entry allowlist.'
        }
        if ($node -is [Management.Automation.Language.VariableExpressionAst] -and
            ($node.VariablePath.IsDriveQualified -or
             $node.VariablePath.UserPath -match '^(global|script|env):' -or
             $allowedVariables -notcontains $node.VariablePath.UserPath)) {
            throw 'HARNESS: qualified variable/process environment forbidden.'
        }
        if ($node -is [Management.Automation.Language.InvokeMemberExpressionAst]) {
            $knownBoundParameters = @('$PSBoundParameters.ContainsKey(''VerificationPhase'')', '$PSBoundParameters.ContainsKey($name)')
            $knownPathCombine = $node.Static -and
                $node.Expression -is [Management.Automation.Language.TypeExpressionAst] -and
                $node.Expression.TypeName.FullName -ceq 'IO.Path' -and
                $node.Member -is [Management.Automation.Language.StringConstantExpressionAst] -and
                $node.Member.Value -ceq 'Combine'
            if ($knownBoundParameters -cnotcontains $node.Extent.Text -and -not $knownPathCombine) {
                throw 'HARNESS: entry member invocation not reviewed.'
            }
        }
        if (($node -is [Management.Automation.Language.TypeExpressionAst] -and $node.TypeName.FullName -cne 'IO.Path') -or
            $node -is [Management.Automation.Language.FunctionDefinitionAst] -or
            $node -is [Management.Automation.Language.ScriptBlockExpressionAst] -or
            $node -is [Management.Automation.Language.RedirectionAst] -or
            $node -is [Management.Automation.Language.UsingStatementAst] -or
            $node -is [Management.Automation.Language.TrapStatementAst]) {
            throw 'HARNESS: executable construct outside reviewed entry.'
        }
    }
}

# These functions do NOT call tools, inspect installed tooling, mutate process
# environment or create processes. Explicit prototypes require lead ratification.
# Context/role/witness are UPPER-BOUNDARY DATA DOUBLES ONLY. No source context
# validity, non-reentrance, native result or artifact publication is proven here.
function Enter-VerificationContext([hashtable]$Admission) {
    $state = Get-FixtureState
    $state.context_active = $true
}
function Get-VerificationContext {
    $state = Get-FixtureState
    return @{ active = $state.context_active }
}
function Exit-VerificationContext {
    $state = Get-FixtureState
    $state.context_active = $false
}
function Set-VerificationCommandRole([string]$Role) {
    $state = Get-FixtureState
    $state.role = $Role
}
function Write-VerificationToolchainWitness {
    $state = Get-FixtureState
    $state.witness_requested = $true # NO file/receipt and NO success validation.
}
function Initialize-VerificationTooling([string]$Root, [string]$Python) {
    $state = Get-FixtureState
    $state.events.Add(@{ kind = 'prepare-tooling'; root = $Root; python = $Python })
    return (Get-FixtureTools $Root)
}
function Get-VerificationTooling([string]$Root) {
    $state = Get-FixtureState
    $state.events.Add(@{ kind = 'existing-tooling'; root = $Root })
    if (-not $state.input.tooling_exists) { throw 'MissingVerificationTooling: no bootstrap permitted.' }
    return (Get-FixtureTools $Root)
}
function Get-FixtureTools([string]$Root) {
    $state = Get-FixtureState
    return @{ conan = (Join-Path $Root 'out/tooling/build/Scripts/conan.exe')
        # CMake is NOT in build-requirements.txt: simulate an existing external
        # installation resolved by the tooling leaf, never invent a pip install.
        cmake = (Join-Path $state.case_directory 'fake CMake installation/bin/cmake.exe')
        ninja = (Join-Path $Root 'out/tooling/build/Scripts/ninja.exe') }
}
function Assert-VerificationPreparedTree([string]$BuildDirectory, [string]$Configuration) {
    # Read-only fake metadata observation, distinct from an environment/tool effect.
    $state = Get-FixtureState
    if (-not $state.input.prepared) { throw 'InvalidVerificationPreparedTree: not prepared.' }
    if ($Configuration -cne $state.input.prepared_configuration) {
        throw 'InvalidVerificationPreparedTree: wrong prepared configuration.'
    }
}
function Read-VerificationEnvironmentLeaf {
    $state = Get-FixtureState
    return (Copy-FixtureMap $state.environment)
}
function Write-VerificationEnvironmentLeaf([string]$Name, $Value) {
    $state = Get-FixtureState
    $state.events.Add(@{ kind = 'env-write'; name = $Name; value = $Value })
    if ($null -eq $Value) { $state.environment.Remove($Name) }
    else { $state.environment[$Name] = $Value }
}
function Read-VerificationMsvcEnvironmentLeaf {
    $state = Get-FixtureState
    $state.events.Add(@{ kind = 'msvc' })
    if ($state.input.msvc_throws) { throw 'FixtureMsvcException: preserved first cause.' }
    return (Copy-FixtureObject $state.input.msvc_environment)
}
function Invoke-FixtureChild([string]$Program, [string[]]$Arguments) {
    # $root/$tooling/$venvPython MUST resolve from the extracted source script.
    # They are intentionally NOT defined in the fixture runner/caller scope.
    $state = Get-FixtureState
    $tools = Get-FixtureTools $root
    if (@($tools.conan, $tools.cmake) -inotcontains $Program) {
        throw 'HARNESS: child must name an owned absolute fake tool.'
    }
    if ($state.child_index -ge @($state.input.children).Count) {
        throw 'HARNESS: extra unplanned fake child.'
    }
    $outcome = $state.input.children[$state.child_index]
    $state.child_index += 1
    $state.events.Add(@{ kind = 'child'; program = $Program; arguments = @($Arguments)
        root = $root; tooling = $tooling; venv_python = $venvPython
        log_directory = $VerificationLogDirectory; timeout_seconds = $VerificationTimeoutSeconds
        environment = (Copy-FixtureMap $state.environment)
        raw_stdout = [string]$outcome.stdout; raw_stderr = [string]$outcome.stderr
        raw_code = $outcome.code; throws = [bool]$outcome.throws })
    if ($outcome.throws) { throw 'FixtureChildException: preserved first cause.' }
    return [int]$outcome.code
}
function Invoke-FixtureForbidden {
    $state = Get-FixtureState
    $state.events.Add(@{ kind = 'FORBIDDEN' })
    throw 'HARNESS: actual tool/process command reached forbidden sentinel.'
}
# Defense in depth only; AST+pins are the admission gate, not these aliases.
foreach ($name in @('conan', 'cmake', 'ctest', 'ninja', 'python', 'pip', 'cmd',
    'Start-Process', 'Invoke-Expression')) {
    Set-Alias -Name $name -Value Invoke-FixtureForbidden -Scope Script
}

$fixtureState = @{
    input = $fixtureInput; case_directory = $fixtureCase
    events = (New-Object 'Collections.Generic.List[object]')
    environment = (Copy-FixtureObject $fixtureInput.environment); child_index = 0
    context_active = $false; role = ''; witness_requested = $false
}
try {
    try {
        # Collision is a harness error, never overwrite someone else's state.
        # The process is the single owned, isolated PowerShell interpreter.
        if ($null -ne (Get-Variable -Name ARTIST_HELPER_FUNCTIONAL_FIXTURE_STATE_V1 -Scope Global -ErrorAction SilentlyContinue)) {
            throw 'HARNESS: fixture state name already exists.'
        }
        Set-Variable -Name ARTIST_HELPER_FUNCTIONAL_FIXTURE_STATE_V1 -Scope Global -Value $fixtureState -ErrorAction Stop
        $fixtureOwnsState = $true
        $pin = Get-Content -LiteralPath ($PSScriptRoot + '/helper_fragments_red.json') -Raw | ConvertFrom-Json
        if ((Get-FixtureHash ([string]$fixtureInput.binding_source)) -cne [string]$pin.binding_sha256) {
            throw 'HARNESS: excluded binding pin mismatch; no evaluation.'
        }
        Assert-FixtureAst ([string]$fixtureInput.binding_source) 'excluded-binding'
        foreach ($name in @('parameters', 'definitions', 'entry', 'root_binding')) {
            $fragment = [string]$fixtureInput.fragments.$name
            if ((Get-FixtureHash $fragment) -cne [string]$pin.fragments.$name) {
                throw 'HARNESS: fragment pin mismatch; no evaluation.'
            }
            $kind = 'entry'; if ($name -eq 'definitions' -or $name -eq 'parameters') { $kind = $name }
            Assert-FixtureAst $fragment $kind
        }
        if ($fixtureInput.mode -ceq 'audit') {
            # Mutant source is parsed ONLY. Even an allowed command is NEVER
            # evaluated in audit mode. Review pins still admit all real fragments.
            Assert-FixtureAst ([string]$fixtureInput.audit_source) 'entry'
        } else {
        $flags = Copy-FixtureObject $fixtureInput.flags
        # Even negative test data paths stay inside the fixture's owned case.
        foreach ($name in @('VerificationInputFile', 'VerificationBundleRoot', 'VerificationLogDirectory')) {
            if ($flags.ContainsKey($name) -and -not [string]::IsNullOrWhiteSpace([string]$flags[$name])) {
                Assert-FixtureOwnedPath ([string]$flags[$name])
            }
        }
        # Load only twelve reviewed definitions (eight old + four pure SOURCE
        # helpers); NEVER evaluate the excluded binding/native start/capture.
        . ([scriptblock]::Create([string]$fixtureInput.fragments.definitions))
        # Replace ONLY the upper effect boundary after loading source wrappers.
        Set-Alias -Name Invoke-VerificationChild -Value Invoke-FixtureChild -Scope Script
        $programDirectory = $fixtureCase + '\source tree with spaces\scripts'
        [void][IO.Directory]::CreateDirectory($programDirectory)
        $program = $programDirectory + '\entry-fragment.ps1'
        $newline = [Environment]::NewLine
        $text = [string]$fixtureInput.fragments.parameters + $newline +
            '$ErrorActionPreference = ''Stop''' + $newline +
            [string]$fixtureInput.fragments.entry + $newline + "throw 'FIXTURE_LEGACY_FALLTHROUGH'"
        $parseTokens = $null; $parseErrors = $null
        [void][Management.Automation.Language.Parser]::ParseInput($text, [ref]$parseTokens, [ref]$parseErrors)
        if (@($parseErrors).Count -ne 0) { throw 'HARNESS: composed entry parse failed.' }
        [IO.File]::WriteAllText($program, $text, (New-Object Text.UTF8Encoding($false)))
        if ($fixtureInput.mode -ceq 'baseline') {
            # A different child script scope; NEVER seeds variables for entry.
            $probe = $programDirectory + '\root-probe.ps1'
            $probeSource = [string]$fixtureInput.fragments.parameters + $newline +
                [string]$fixtureInput.fragments.root_binding + $newline +
                '$probeTools = Get-VerificationTooling $root' + $newline +
                '$probeCode = $null; $probeError = $null' + $newline +
                'try { $probeCode = Invoke-VerificationChild $probeTools.cmake @(''--fixture-only'', ''literal;'') }' + $newline +
                'catch { $probeError = $_.Exception.Message }' + $newline +
                'return @{ root = $root; tooling = $tooling; venv_python = $venvPython; code = $probeCode; error = $probeError }'
            [IO.File]::WriteAllText($probe, $probeSource, (New-Object Text.UTF8Encoding($false)))
            $fixtureBaseline = & $probe @flags
            # Exercise fake leaves independently of production, not a fake GREEN helper.
            $fakeTools = Initialize-VerificationTooling $fixtureBaseline.root 'fixture-python'
            $existingTools = Get-VerificationTooling $fixtureBaseline.root
            $fixtureBaseline.tools = $fakeTools
            $fixtureBaseline.existing_tools = $existingTools
            $fixtureBaseline.metadata = Get-VerificationMsvcEnvironment
            $fixtureBaseline.phase = Resolve-VerificationPhaseFlags $flags
            $baselineDecoded = Read-VerificationPayloadFile $flags.VerificationInputFile $flags.VerificationBundleRoot
            $fixtureBaseline.inputs = Resolve-VerificationInputs $baselineDecoded $fixtureBaseline.root $flags.VerificationPhase $flags.Configuration
            Assert-VerificationPreparedTree $fixtureBaseline.inputs.build_directory $flags.Configuration
            $before = Get-VerificationProcessEnvironment
            Set-VerificationProcessEnvironment @{ KEEP = ''; NEW = 'temporary' }
            Set-VerificationProcessEnvironment $before
        } elseif ($fixtureInput.mode -ceq 'phase') {
            # A test-owned fragment FILE gives PSScriptRoot its real semantics.
            # NO & helper, dot-source helper, expression argument or native child.
            $global:LASTEXITCODE = 0
            try { & $program @flags; $fixtureExit = [int]$LASTEXITCODE }
            catch {
                if ($_.Exception.Message.StartsWith('HARNESS:') -or
                    $_.Exception -is [Management.Automation.CommandNotFoundException]) { throw }
                $fixtureError = $_.Exception.Message; $fixtureExit = 1
            }
        } else { throw 'HARNESS: unknown fixture mode.' }
        }
    } catch {
        $fixtureHarnessError = $_.Exception.Message
        $fixtureExit = 96
    }
} finally {
    try {
        # Snapshot BEFORE removing the owned binding. These are only synthetic
        # values, not a process/global environment dump. No leaf is called later.
        $fixtureReport = @{
            fixture_version = 1; harness_error = $fixtureHarnessError; phase_error = $fixtureError
            baseline = $fixtureBaseline; events = @($fixtureState.events.ToArray())
            environment_after = (Copy-FixtureMap $fixtureState.environment)
        }
    } finally {
        # Also runs for a source exit/nonzero/exception or failed snapshot. Never
        # remove a pre-existing binding after the explicit collision refusal.
        if ($fixtureOwnsState) {
            Remove-Variable -Name ARTIST_HELPER_FUNCTIONAL_FIXTURE_STATE_V1 -Scope Global -ErrorAction Stop
            $fixtureOwnsState = $false
        }
    }
    $fixtureReport.fixture_state_removed = ($null -eq (Get-Variable -Name ARTIST_HELPER_FUNCTIONAL_FIXTURE_STATE_V1 -Scope Global -ErrorAction SilentlyContinue))
    $fixtureReport | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $OutputFile -Encoding UTF8
}
exit $fixtureExit
