# Owned test program. Never evaluate the complete build helper or run vcvars.
param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile,
    [Parameter(Mandatory = $true)][string]$DispatchFixtureFile
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$fixturePayload = Get-Content -LiteralPath $InputFile -Raw | ConvertFrom-Json
# Fixed files and owned output directory, checked before any source evaluation.
$approvedRoot = [IO.Path]::GetFullPath($PSScriptRoot + '/../../../out/verification-synthetic/windows-wrapper-fixtures/')
$approvedRoot = $approvedRoot.TrimEnd([char[]]@('\', '/')) + [IO.Path]::DirectorySeparatorChar
$caseDirectory = [IO.Path]::GetFullPath([string]$fixturePayload.case_directory)
if (-not $caseDirectory.StartsWith($approvedRoot, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'HARNESS: case directory is outside the owned evidence root.'
}
$fixedFiles = @{ 'input.json' = $InputFile; 'result.json' = $OutputFile; 'child-result.json' = $fixturePayload.child_result }
foreach ($name in $fixedFiles.Keys) {
    if ([IO.Path]::GetFullPath([string]$fixedFiles[$name]) -ine [IO.Path]::GetFullPath($caseDirectory + '/' + $name)) {
        throw 'HARNESS: fixture file path is not its fixed owned path.'
    }
}
if ([IO.Path]::GetFullPath($DispatchFixtureFile) -ine [IO.Path]::GetFullPath($PSScriptRoot + '/dispatch_fixture.ps1') -or
    [IO.Path]::GetFullPath([string]$fixturePayload.child) -ine [IO.Path]::GetFullPath($PSScriptRoot + '/wrapper_child.py')) {
    throw 'HARNESS: source files are not the fixed test-owned files.'
}
if ([IO.Path]::GetFileName([string]$fixturePayload.python) -ine 'python.exe' -or
    (Get-FileHash -LiteralPath $fixturePayload.python -Algorithm SHA256).Hash -ine $fixturePayload.python_sha256 -or
    (Get-FileHash -LiteralPath $fixturePayload.child -Algorithm SHA256).Hash -ine $fixturePayload.child_sha256) {
    throw 'HARNESS: Python or owned child identity changed.'
}
$fixtureEvents = New-Object 'System.Collections.Generic.List[object]'
$fixtureEnvironment = @{}
$fixtureMsvc = @{}
$fixtureValues = @{}
foreach ($property in $fixturePayload.environment.PSObject.Properties) {
    $fixtureEnvironment[$property.Name] = $property.Value
}
foreach ($property in $fixturePayload.msvc.PSObject.Properties) {
    $fixtureMsvc[$property.Name] = $property.Value
}
foreach ($property in $fixturePayload.values.PSObject.Properties) {
    $fixtureValues[$property.Name] = $property.Value
}
$fixtureResult = $null
$fixtureError = $null
$fixtureHarnessError = $null
$fixtureExit = 0
$fixtureProcessIndex = 0

function Invoke-ForbiddenFixtureOperation {
    # An audit mutant can only reach this owned sentinel, never a real tool,
    # even if its audit assertion is accidentally broken.
    throw 'HARNESS: disallowed owned sentinel was evaluated.'
}

function Read-VerificationEnvironmentLeaf {
    # Intentionally shared: snapshot wrapper must detach its result itself.
    $script:fixtureEvents.Add(@{ kind = 'read-environment' })
    return $script:fixtureEnvironment
}

function Write-VerificationEnvironmentLeaf([string]$Name, $Value) {
    $script:fixtureEvents.Add(@{ kind = 'write-environment'; name = $Name; value = $Value })
    if ($null -eq $Value) { $script:fixtureEnvironment.Remove($Name) }
    else { $script:fixtureEnvironment[$Name] = [string]$Value }
}

function Read-VerificationMsvcEnvironmentLeaf {
    $script:fixtureEvents.Add(@{ kind = 'read-msvc-fixture' })
    return $script:fixtureMsvc
}

function Invoke-VerificationProcessLeaf([string]$Program, [string]$CommandLine) {
    $script:fixtureEvents.Add(@{ kind = 'process'; program = $Program; command_line = $CommandLine })
    if ($script:fixturePayload.action -eq 'compose') {
        # Composition never starts anything: even conan/cmake are recorded data.
        if ($Program -notin @('conan', 'cmake', [string]$script:fixtureValues.executable)) {
            throw 'HARNESS: unexpected record-only program.'
        }
        if ($script:fixtureProcessIndex -ge @($script:fixturePayload.codes).Count) {
            throw 'HARNESS: unexpected extra process request.'
        }
        $code = [int]$script:fixturePayload.codes[$script:fixtureProcessIndex]
        $script:fixtureProcessIndex += 1
        return $code
    }
    # Only this exact interpreter and fixed fake-script prefix may start.
    # Unknown program or prefix is rejected, never delegated to a real tool.
    if ($Program -cne [string]$script:fixturePayload.python) {
        throw 'HARNESS: process is not the owned Python fake.'
    }
    if ($script:fixtureProcessIndex -ne 0) { throw 'HARNESS: only one owned child is permitted per case.' }
    $script:fixtureProcessIndex += 1
    $prefix = '"-I" "-S" "-B" "' + $script:fixturePayload.child + '" "' +
        $script:fixturePayload.child_result + '" "' + $script:fixturePayload.child_code + '"'
    if ($CommandLine -cne $prefix -and -not $CommandLine.StartsWith($prefix + ' ')) {
        throw 'HARNESS: process prefix is not the owned finite fake.'
    }
    $start = New-Object System.Diagnostics.ProcessStartInfo
    $start.FileName = [string]$script:fixturePayload.python
    $start.Arguments = $CommandLine
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true
    $start.WorkingDirectory = [string]$script:fixturePayload.case_directory
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $start
    try {
        [void]$process.Start()
        if (-not $process.WaitForExit(5000)) {
            $process.Kill()
            $process.WaitForExit()
            throw 'HARNESS: owned fake child timed out.'
        }
        $stdout = $process.StandardOutput.ReadToEnd()
        $stderr = $process.StandardError.ReadToEnd()
        [IO.File]::WriteAllText($script:fixturePayload.case_directory + '/child.stdout.log', $stdout)
        [IO.File]::WriteAllText($script:fixturePayload.case_directory + '/child.stderr.log', $stderr)
        return $process.ExitCode
    } finally { $process.Dispose() }
}

function Assert-WrapperSource($Source) {
    # Closed API audit for reviewed source, not a general PowerShell sandbox.
    if ($Source.Length -gt 32768 -or $Source -match '(?im)^\s*#requires\b') {
        throw 'Wrapper source exceeds bounds or contains #requires.'
    }
    $tokens = $null
    $errors = $null
    $ast = [System.Management.Automation.Language.Parser]::ParseInput($Source, [ref]$tokens, [ref]$errors)
    if (@($errors).Count -ne 0) { throw 'Wrapper parse error.' }
    $commands = @{
        'Resolve-VerificationInputs' = @()
        'ConvertTo-VerificationWindowsArgument' = @()
        'Get-VerificationProcessEnvironment' = @('Read-VerificationEnvironmentLeaf')
        'Set-VerificationProcessEnvironment' = @('Read-VerificationEnvironmentLeaf', 'Write-VerificationEnvironmentLeaf')
        'Get-VerificationMsvcEnvironment' = @('Read-VerificationMsvcEnvironmentLeaf')
        'Invoke-VerificationChild' = @('ConvertTo-VerificationWindowsArgument', 'Invoke-VerificationProcessLeaf')
    }
    $variables = @('Values', 'Root', 'Phase', 'Configuration', 'Value', 'Program', 'Arguments',
        'names', 'name', 'current', 'copy', 'result', 'expected', 'directory', 'targets',
        'target', 'executable', 'arguments', 'argument', 'total', 'index', 'character',
        'slashes', 'encoded', 'commandLine', 'metadata', 'code', 'true', 'false', 'null')
    $statements = @($ast.EndBlock.Statements)
    if ($statements.Count -ne $commands.Count) { throw 'Unexpected wrapper population.' }
    $seen = @()
    foreach ($function in $statements) {
        if ($function -isnot [System.Management.Automation.Language.FunctionDefinitionAst] -or
            -not $commands.ContainsKey($function.Name) -or $seen -contains $function.Name) {
            throw 'Only the six distinct wrapper definitions may be extracted.'
        }
        $seen += $function.Name
        $nodes = @($function.FindAll({ param($node) $true }, $true))
        if ($nodes.Count -gt 2048) { throw 'Wrapper AST exceeds bounds.' }
        foreach ($node in $nodes) {
            if ($node -is [System.Management.Automation.Language.CommandAst]) {
                if ($node.InvocationOperator -ne [System.Management.Automation.Language.TokenKind]::Unknown -or
                    $commands[$function.Name] -notcontains $node.GetCommandName()) {
                    throw 'Wrapper command is outside its primitive-effect allowlist.'
                }
            }
            if ($node -is [System.Management.Automation.Language.VariableExpressionAst] -and
                $variables -notcontains $node.VariablePath.UserPath) { throw 'Wrapper variable is not allowed.' }
            if ($node -is [System.Management.Automation.Language.TypeConstraintAst] -and
                $node.TypeName.FullName -notin @('string', 'string[]', 'hashtable', 'array', 'int', 'char')) {
                throw 'Wrapper data type is not allowed.'
            }
            if ($node -is [System.Management.Automation.Language.TypeExpressionAst] -and
                $node.TypeName.FullName -notin @('System.IO.Path', 'string', 'string[]', 'hashtable', 'array', 'int', 'char')) {
                throw 'Wrapper static type is not allowed.'
            }
            if ($node -is [System.Management.Automation.Language.InvokeMemberExpressionAst]) {
                if (-not $node.Static -or $node.Expression.Extent.Text -cne '[System.IO.Path]' -or
                    $node.Member.Extent.Text -cne 'GetFullPath') {
                    throw 'Only pure Path.GetFullPath is allowed as a wrapper method.'
                }
            }
            elseif ($node -is [System.Management.Automation.Language.MemberExpressionAst] -and
                $node.Member.Extent.Text -notin @('Keys', 'Count', 'Length')) {
                # Approved exception: only the inner PSBase receiver of these
                # exact raw-key reads, in the validated-map setter, nothing else.
                $access = $node.Parent
                if ($function.Name -cne 'Set-VerificationProcessEnvironment' -or
                    $node.Member.Extent.Text -cne 'PSBase' -or
                    $access -isnot [System.Management.Automation.Language.MemberExpressionAst] -or
                    $access -is [System.Management.Automation.Language.InvokeMemberExpressionAst] -or
                    $access.Expression -ne $node -or
                    @('$current.PSBase.Keys', '$Values.PSBase.Keys') -cnotcontains $access.Extent.Text) {
                    throw 'Only data Keys/Count/Length properties and approved setter raw-key reads are allowed.'
                }
                # Reject writes even through an index/parenthesized LHS. Reading
                # the same chain on an assignment RHS (e.g. to $names) is valid.
                $context = $access.Parent
                while ($null -ne $context -and $context -ne $function) {
                    if ($context -is [System.Management.Automation.Language.AssignmentStatementAst] -and
                        $context.Left.Extent.StartOffset -le $access.Extent.StartOffset -and
                        $context.Left.Extent.EndOffset -ge $access.Extent.EndOffset) {
                        throw 'Approved raw-key chains are read-only, not assignment targets.'
                    }
                    if ($context -is [System.Management.Automation.Language.UnaryExpressionAst] -and
                        $context.TokenKind.ToString() -in @('PlusPlus', 'MinusMinus', 'PostfixPlusPlus', 'PostfixMinusMinus')) {
                        throw 'Approved raw-key chains are read-only, not increment targets.'
                    }
                    $context = $context.Parent
                }
            }
            if (($node -is [System.Management.Automation.Language.FunctionDefinitionAst] -and $node -ne $function) -or
                $node -is [System.Management.Automation.Language.TypeDefinitionAst] -or
                $node -is [System.Management.Automation.Language.UsingStatementAst] -or
                $node -is [System.Management.Automation.Language.DataStatementAst] -or
                $node -is [System.Management.Automation.Language.TrapStatementAst] -or
                $node -is [System.Management.Automation.Language.ScriptBlockExpressionAst] -or
                $node -is [System.Management.Automation.Language.RedirectionAst]) { throw 'Unsupported wrapper construct.' }
        }
    }
}

try {
    try { Assert-WrapperSource ([string]$fixturePayload.wrappers) }
    catch { $fixtureHarnessError = $_.Exception.Message; $fixtureExit = 96 }
    if (-not $fixtureHarnessError) {
        # Dot-source only the audited six definitions, never the original file.
        . ([scriptblock]::Create([string]$fixturePayload.wrappers))
        try {
            switch ([string]$fixturePayload.action) {
                'resolve' {
                    $fixtureResult = Resolve-VerificationInputs $fixtureValues $fixturePayload.root `
                        $fixturePayload.phase 'Release'
                }
                'snapshot' {
                    $fixtureResult = Get-VerificationProcessEnvironment
                    $fixtureResult['PATH'] = 'changed returned snapshot'
                }
                'restore' { Set-VerificationProcessEnvironment $fixtureValues }
                'msvc' { $fixtureResult = Get-VerificationMsvcEnvironment }
                'quote' {
                    $encoded = @('"-I"', '"-S"', '"-B"', ('"' + $fixturePayload.child + '"'),
                        ('"' + $fixturePayload.child_result + '"'), ('"' + $fixturePayload.child_code + '"'))
                    foreach ($argument in $fixturePayload.arguments) {
                        $encoded += ConvertTo-VerificationWindowsArgument $argument
                    }
                    $fixtureExit = Invoke-VerificationProcessLeaf $fixturePayload.python ($encoded -join ' ')
                }
                'child' {
                    $arguments = @('-I', '-S', '-B', [string]$fixturePayload.child, [string]$fixturePayload.child_result,
                        [string]$fixturePayload.child_code) + @($fixturePayload.arguments)
                    $fixtureResult = Invoke-VerificationChild $fixturePayload.python $arguments
                    $fixtureExit = $fixtureResult
                }
                'selfcheck' {
                    $command = '"-I" "-S" "-B" "' + $fixturePayload.child + '" "' + $fixturePayload.child_result +
                        '" "17" "fixture-only"'
                    $fixtureExit = Invoke-VerificationProcessLeaf $fixturePayload.python $command
                }
                'compose' {
                    # Reuse the EXISTING dispatch audit unchanged, extracting just
                    # its function AST. Never evaluate its complete fixture file.
                    $tokens = $null; $errors = $null
                    $ast = [System.Management.Automation.Language.Parser]::ParseFile(
                        $DispatchFixtureFile, [ref]$tokens, [ref]$errors)
                    $audit = @($ast.FindAll({ param($node)
                        $node -is [System.Management.Automation.Language.FunctionDefinitionAst] -and
                        $node.Name -eq 'Assert-FixtureDispatch'
                    }, $true))
                    if (@($errors).Count -ne 0 -or $audit.Count -ne 1) { throw 'HARNESS: dispatch audit extraction failed.' }
                    . ([scriptblock]::Create($audit[0].Extent.Text))
                    Assert-FixtureDispatch ([string]$fixturePayload.dispatch)
                    $resolved = Resolve-VerificationInputs $fixtureValues $fixturePayload.root $fixturePayload.phase 'Release'
                    $root = [string]$fixturePayload.root
                    $VerificationBuildDirectory = $resolved['build_directory']
                    $VerificationTargets = $resolved['targets']
                    $VerificationExecutable = $resolved['executable']
                    $VerificationArguments = $resolved['arguments']
                    $AllowDependencyResolution = [bool]$fixturePayload.allow
                    $Configuration = 'Release'; $Jobs = 1; $VisualStudioPath = 'fixture VS'; $Python = 'unused'
                    $source = 'param([string]$VerificationPhase)' + [Environment]::NewLine +
                        $fixturePayload.dispatch + [Environment]::NewLine + "throw 'FIXTURE_LEGACY_FALLTHROUGH'"
                    & ([scriptblock]::Create($source)) -VerificationPhase ([string]$fixturePayload.phase)
                }
                default { throw 'HARNESS: unknown fixture action.' }
            }
        } catch { $fixtureError = $_.Exception.Message; $fixtureExit = 1 }
    }
} finally {
    @{
        fixture_version = 1; harness_error = $fixtureHarnessError; phase_error = $fixtureError
        result = $fixtureResult; events = @($fixtureEvents.ToArray()); environment_after = $fixtureEnvironment
    } | ConvertTo-Json -Depth 16 | Set-Content -LiteralPath $OutputFile -Encoding UTF8
}
exit $fixtureExit
