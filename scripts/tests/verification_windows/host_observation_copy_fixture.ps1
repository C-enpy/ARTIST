# TEST-OWNED proposal. Lead review BEFORE execution. No real environment snapshot,
# process observation, native process, helper entry or full source function call.
param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$inputData = Get-Content -LiteralPath $InputFile -Raw | ConvertFrom-Json
$case = [IO.Path]::GetFullPath([string]$inputData.case)
$approved = [IO.Path]::GetFullPath($PSScriptRoot + '/../../../out/verification-synthetic/windows-native-binding/').TrimEnd('\') + '\'
if (-not $case.StartsWith($approved, [StringComparison]::OrdinalIgnoreCase) -or
    [IO.Path]::GetFullPath($InputFile) -ine ($case + '\input.json') -or
    [IO.Path]::GetFullPath($OutputFile) -ine ($case + '\result.json')) {
    throw 'HARNESS: exact owned copy-probe paths required.'
}
function Get-CopyProbeHash([string]$Text) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return ([BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes($Text)))).Replace('-', '').ToLowerInvariant() }
    finally { $sha.Dispose() }
}
function Get-CopyProbeFlatText([string]$Text) {
    return [Text.RegularExpressions.Regex]::Replace($Text, '\s+', ' ').Trim()
}
function Assert-CopyProbeFilter($Definition) {
    # These are the TWO real admitted pure functions, never a copied test filter.
    $variables = @('Values', 'selected', 'name', 'true', 'false', 'null')
    foreach ($node in $Definition.Body.FindAll({ param($n) $true }, $true)) {
        if ($node -is [Management.Automation.Language.CommandAst] -and
            ($node.InvocationOperator -ne [Management.Automation.Language.TokenKind]::Unknown -or
             $node.GetCommandName() -cne 'Get-VerificationEnvironmentNames')) {
            throw 'HARNESS: copy-probe filter command not admitted.'
        }
        if ($node -is [Management.Automation.Language.VariableExpressionAst] -and
            ($node.VariablePath.IsDriveQualified -or $variables -notcontains $node.VariablePath.UserPath)) {
            throw 'HARNESS: copy-probe filter variable not admitted.'
        }
        if ($node -is [Management.Automation.Language.TypeExpressionAst] -and
            @('string', 'char', 'hashtable') -notcontains $node.TypeName.FullName) {
            throw 'HARNESS: copy-probe filter type not admitted.'
        }
        if ($node -is [Management.Automation.Language.InvokeMemberExpressionAst] -and
            ($node.Static -or @('ContainsKey', 'IndexOf') -cnotcontains $node.Member.Value)) {
            throw 'HARNESS: copy-probe filter member not admitted.'
        }
        if ($node -is [Management.Automation.Language.FunctionDefinitionAst] -or
            $node -is [Management.Automation.Language.ScriptBlockExpressionAst] -or
            $node -is [Management.Automation.Language.RedirectionAst] -or
            $node -is [Management.Automation.Language.UsingStatementAst]) {
            throw 'HARNESS: copy-probe filter construct not admitted.'
        }
    }
}
$report = @{ format = 'artist-host-copy-probe-v1'; harness_error = $null; phase_error = $null
    source_loop = $null; source_copy = $null; filtered = $null; input_unchanged = $null
    selected_source_statements = 0 }
$exitCode = 96
try {
    $inputNames = @('format', 'action', 'case', 'binding', 'snapshot')
    if (@($inputData.PSObject.Properties).Count -ne $inputNames.Count) {
        throw 'HARNESS: copy-probe input population not admitted.'
    }
    foreach ($property in $inputData.PSObject.Properties) {
        if ($inputNames -cnotcontains $property.Name) { throw 'HARNESS: copy-probe input field not admitted.' }
    }
    if ([string]$inputData.action -cne 'source-copy' -or [int]$inputData.format -ne 1) {
        throw 'HARNESS: copy-probe action/format not admitted.'
    }
    $binding = [string]$inputData.binding
    $pin = Get-Content -LiteralPath ($PSScriptRoot + '/native_binding_admission.json') -Raw | ConvertFrom-Json
    if ($binding.Length -gt 65536 -or (Get-CopyProbeHash $binding) -cne [string]$pin.fragments.binding) {
        throw 'HARNESS: copy-probe binding identity not reviewed; no evaluation.'
    }
    $tokens = $null; $errors = $null
    $ast = [Management.Automation.Language.Parser]::ParseInput($binding, [ref]$tokens, [ref]$errors)
    if (@($errors).Count -ne 0 -or @($ast.EndBlock.Statements).Count -gt 64) {
        throw 'HARNESS: copy-probe binding syntax/population mismatch.'
    }
    foreach ($statement in $ast.EndBlock.Statements) {
        if ($statement -isnot [Management.Automation.Language.FunctionDefinitionAst]) {
            throw 'HARNESS: copy-probe binding must contain definitions only.'
        }
    }
    $hostDefinitions = @($ast.EndBlock.Statements | Where-Object { $_.Name -ceq 'Read-VerificationHostObservationLeaf' })
    if ($hostDefinitions.Count -ne 1) { throw 'HARNESS: copy-probe real host definition absent/ambiguous.' }
    $statements = @($hostDefinitions[0].Body.EndBlock.Statements)
    if ($statements.Count -ne 6 -or
        $statements[0] -isnot [Management.Automation.Language.AssignmentStatementAst] -or
        $statements[1] -isnot [Management.Automation.Language.AssignmentStatementAst] -or
        $statements[2] -isnot [Management.Automation.Language.ForEachStatementAst] -or
        $statements[3] -isnot [Management.Automation.Language.AssignmentStatementAst] -or
        $statements[4] -isnot [Management.Automation.Language.TryStatementAst] -or
        $statements[5] -isnot [Management.Automation.Language.ReturnStatementAst]) {
        throw 'HARNESS: copy-probe host statement structure changed.'
    }
    # The real GetEnvironmentVariables and GetCurrentProcess statements are
    # inspected as AST DATA ONLY. They are NEVER selected/evaluated/aliased.
    if ((Get-CopyProbeFlatText $statements[0].Extent.Text) -cne
        '[Collections.IDictionary]$block = [Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)' -or
        (Get-CopyProbeFlatText $statements[1].Extent.Text) -cne '$values = @{}') {
        throw 'HARNESS: copy-probe snapshot seam/initializer changed.'
    }
    $loopText = Get-CopyProbeFlatText $statements[2].Extent.Text
    $allowedLoops = @(
        'foreach ($name in $block.Keys) { $values[$name] = $block[$name] }',
        'foreach ($name in $block.PSBase.Keys) { $values[$name] = $block[$name] }')
    if ($allowedLoops -cnotcontains $loopText -or $statements[2].Extent.Text.Length -gt 256) {
        throw 'HARNESS: copy-probe loop outside reviewed read-only grammar.'
    }
    $pureDefinitions = @()
    foreach ($name in @('Get-VerificationEnvironmentNames', 'Select-VerificationEnvironment')) {
        $matches = @($ast.EndBlock.Statements | Where-Object { $_.Name -ceq $name })
        if ($matches.Count -ne 1 -or $matches[0].Extent.Text.Length -gt 4096) {
            throw 'HARNESS: copy-probe real filter definition absent/ambiguous/oversize.'
        }
        Assert-CopyProbeFilter $matches[0]
        $pureDefinitions += $matches[0].Extent.Text
    }
    [Collections.IDictionary]$snapshot = @{}
    foreach ($property in $inputData.snapshot.PSObject.Properties) {
        if (@('PATH', 'SystemRoot', 'TEMP', 'Keys', 'Count', 'ARTIST_NATIVE_SECRET_CANARY') -cnotcontains $property.Name -or
            $property.Value -isnot [string] -or $property.Value.Length -gt 1024 -or
            $property.Value.IndexOf([char]0) -ge 0) { throw 'HARNESS: synthetic snapshot fields/values not admitted.' }
        $snapshot[$property.Name] = $property.Value
    }
    $before = @{} + $snapshot
    $newline = [Environment]::NewLine
    # Two exact source statements, in original order. Only the input snapshot
    # and return boundary are test-owned. No whole host function is redefined.
    $copyProgram = 'param([Collections.IDictionary]$block)' + $newline +
        $statements[1].Extent.Text + $newline + $statements[2].Extent.Text + $newline + 'return $values'
    $filterProgram = ($pureDefinitions -join $newline) + $newline + 'Select-VerificationEnvironment $args[0]'
    foreach ($program in @($copyProgram, $filterProgram)) {
        $parseTokens = $null; $parseErrors = $null
        [void][Management.Automation.Language.Parser]::ParseInput($program, [ref]$parseTokens, [ref]$parseErrors)
        if (@($parseErrors).Count -ne 0) { throw 'HARNESS: copy-probe composed syntax invalid; not RED.' }
    }
    $report.source_loop = $loopText
    $report.selected_source_statements = 2
    try {
        $copied = & ([scriptblock]::Create($copyProgram)) $snapshot
        $report.source_copy = $copied
        $report.filtered = & ([scriptblock]::Create($filterProgram)) $copied
        $exitCode = 0
    } catch { $report.phase_error = $_.Exception.Message; $exitCode = 1 }
    $unchanged = $snapshot.PSBase.Count -eq $before.PSBase.Count
    foreach ($key in $before.PSBase.Keys) {
        if (-not $snapshot.Contains($key) -or $snapshot[$key] -cne $before[$key]) { $unchanged = $false }
    }
    $report.input_unchanged = $unchanged
} catch { $report.harness_error = $_.Exception.Message; $exitCode = 96 }
$bytes = [Text.Encoding]::UTF8.GetBytes(($report | ConvertTo-Json -Depth 8 -Compress))
$file = [IO.File]::Open($OutputFile, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
try { $file.Write($bytes, 0, $bytes.Length) } finally { $file.Dispose() }
exit $exitCode
