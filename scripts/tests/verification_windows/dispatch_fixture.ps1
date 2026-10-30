# Test-owned program. Never dot-source or invoke build-windows.ps1 here.
# All dispatch effect boundaries below are record-only functions with no fallback.
param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$payload = Get-Content -LiteralPath $InputFile -Raw | ConvertFrom-Json
$dispatch = [string]$payload.dispatch
$fixtureEvents = New-Object 'System.Collections.Generic.List[object]'
$fixtureError = $null
$fixtureHarnessError = $null
$fixtureExit = 0
$fixtureChildIndex = 0
$fixtureEnvironment = @{}
foreach ($property in $payload.environment.PSObject.Properties) {
    $fixtureEnvironment[$property.Name] = [string]$property.Value
}

function Copy-FixtureMap($Values) {
    $copy = @{}
    foreach ($key in $Values.Keys) { $copy[$key] = $Values[$key] }
    return $copy
}

function Get-VerificationProcessEnvironment {
    $script:fixtureEvents.Add(@{ kind = 'snapshot' })
    return (Copy-FixtureMap $script:fixtureEnvironment)
}

function Get-VerificationMsvcEnvironment {
    $script:fixtureEvents.Add(@{ kind = 'msvc-fixture' })
    $values = @{}
    foreach ($property in $script:payload.msvc_environment.PSObject.Properties) {
        $values[$property.Name] = [string]$property.Value
    }
    return $values
}

function Set-VerificationProcessEnvironment($Values) {
    $script:fixtureEvents.Add(@{ kind = 'set-environment' })
    $script:fixtureEnvironment = Copy-FixtureMap $Values
}

function Invoke-VerificationChild([string]$Program, [string[]]$Arguments) {
    # Arguments are data: no &, Start-Process, cmd, shell or native executable.
    $script:fixtureEvents.Add(@{
        kind = 'child'
        program = $Program
        arguments = @($Arguments)
        environment = Copy-FixtureMap $script:fixtureEnvironment
    })
    if ($script:fixtureChildIndex -ge @($script:payload.child_codes).Count) {
        throw 'Fixture received an unexpected extra child request.'
    }
    $code = [int]$script:payload.child_codes[$script:fixtureChildIndex]
    $script:fixtureChildIndex += 1
    return $code
}

function Assert-FixtureDispatch($Source) {
    # A narrow audit for this reviewed fixture, NOT a general PowerShell sandbox.
    # Parse as data first; create an executable scriptblock only after approval.
    if ($Source.Length -gt 32768 -or $Source -match '(?im)^\s*#requires\b') {
        throw 'Dispatch exceeds fixture bounds or contains #requires.'
    }
    $tokens = $null
    $parseErrors = $null
    $ast = [System.Management.Automation.Language.Parser]::ParseInput(
        $Source, [ref]$tokens, [ref]$parseErrors)
    if (@($parseErrors).Count -ne 0) { throw 'Dispatch parse error in fixture input.' }
    $nodes = @($ast.FindAll({ param($node) $true }, $true))
    if ($nodes.Count -gt 2048) { throw 'Dispatch AST exceeds fixture bounds.' }
    $commands = @('Get-VerificationProcessEnvironment', 'Get-VerificationMsvcEnvironment',
        'Set-VerificationProcessEnvironment', 'Invoke-VerificationChild')
    $variables = @('PSBoundParameters', 'VerificationPhase', 'AllowDependencyResolution',
        'Configuration', 'VerificationTargets', 'VerificationExecutable',
        'VerificationArguments', 'VerificationBuildDirectory', 'VisualStudioPath',
        'Python', 'Jobs', 'root', 'originalEnvironment', 'msvcEnvironment',
        'childCode', 'returnCode', 'preset', 'profile', 'buildDir', 'arguments',
        'program', 'target', 'true', 'false', 'null')
    foreach ($node in $nodes) {
        if ($node -is [System.Management.Automation.Language.CommandAst]) {
            if ($node.InvocationOperator -ne [System.Management.Automation.Language.TokenKind]::Unknown -or
                $commands -notcontains $node.GetCommandName()) {
                throw 'Dispatch command is outside the record-only fixture allowlist.'
            }
        }
        if ($node -is [System.Management.Automation.Language.VariableExpressionAst]) {
            if ($variables -notcontains $node.VariablePath.UserPath) {
                throw 'Dispatch variable is outside the fixture allowlist.'
            }
        }
        if ($node -is [System.Management.Automation.Language.MemberExpressionAst]) {
            if ($node.Extent.Text -cne '$PSBoundParameters.ContainsKey(''VerificationPhase'')') {
                throw 'Dispatch member access is outside the fixture allowlist.'
            }
        }
        if ($node -is [System.Management.Automation.Language.TypeExpressionAst] -or
            $node -is [System.Management.Automation.Language.TypeConstraintAst] -or
            $node -is [System.Management.Automation.Language.ConvertExpressionAst] -or
            $node -is [System.Management.Automation.Language.FunctionDefinitionAst] -or
            $node -is [System.Management.Automation.Language.TypeDefinitionAst] -or
            $node -is [System.Management.Automation.Language.UsingStatementAst] -or
            $node -is [System.Management.Automation.Language.DataStatementAst] -or
            $node -is [System.Management.Automation.Language.TrapStatementAst] -or
            $node -is [System.Management.Automation.Language.ScriptBlockExpressionAst] -or
            $node -is [System.Management.Automation.Language.RedirectionAst]) {
            throw 'Dispatch contains an unsupported executable construct.'
        }
    }
}

try {
    try { Assert-FixtureDispatch $dispatch }
    catch {
        $fixtureHarnessError = $_.Exception.Message
        $fixtureExit = 96
    }
    if (-not $fixtureHarnessError) {
        # These are fixture variables, not a copy of model.py or a core receipt.
        # No actual toolchain, device, filesystem workload or process env is read.
        $AllowDependencyResolution = [bool]$payload.allow_dependency_resolution
        $Configuration = [string]$payload.configuration
        $VerificationTargets = @($payload.targets)
        $VerificationExecutable = [string]$payload.executable
        $VerificationArguments = @($payload.arguments)
        $VerificationBuildDirectory = [string]$payload.build_directory
        $VisualStudioPath = 'C:/fixture Visual Studio'
        $Python = 'fixture-python-never-executed'
        $Jobs = 1
        $root = 'C:/fixture source with spaces'
        # Appending a harmless sentinel detects accidental fallthrough. An early
        # return in the dispatch exits this block, a throw/exit also skips it.
        # Bind the local automatic PSBoundParameters in the invoked block itself,
        # rather than assuming it inherits a parent script's bound parameters.
        $withSentinel = [scriptblock]::Create(
            'param([string]$VerificationPhase)' + [Environment]::NewLine +
            $dispatch + [Environment]::NewLine + "throw 'FIXTURE_LEGACY_FALLTHROUGH'")
        try { & $withSentinel -VerificationPhase ([string]$payload.phase) }
        catch {
            $fixtureError = $_.Exception.Message
            $fixtureExit = 1
        }
    }
} finally {
    # Also runs for exit <child-code> from dispatch; preserve the raw process code.
    @{
        fixture_version = 1
        harness_error = $fixtureHarnessError
        phase_error = $fixtureError
        events = @($fixtureEvents.ToArray())
        environment_after = $fixtureEnvironment
    } | ConvertTo-Json -Depth 16 | Set-Content -LiteralPath $OutputFile -Encoding UTF8
}
exit $fixtureExit
