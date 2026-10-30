# RETIRED as a standalone dispatch qualification program (QA15 G4).
# wrapper_fixture.ps1 still parses ONLY the verbatim Assert-FixtureDispatch AST.
# Never dot-source this file or invoke the real helper. No payload is read here.
# Replacement inventory and selected proofs: G2_G3_G4_MAPPING.md.
throw 'RETIRED: standalone dispatch fixture; see scripts/tests/verification_windows/G2_G3_G4_MAPPING.md. Run uses the direct adapter/runner, never helper-run.'

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
