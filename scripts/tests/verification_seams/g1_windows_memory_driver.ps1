# G1 TEST-OWNED memory driver. Lead review before ANY execution.
# No executable, cmd/batch transport, complete helper, native process lifecycle,
# engine, CTest, dependency resolver or renderer is launched by this program.
# The only script invocation is the exact installed/pinned COMPOSED fragment.
param(
    [Parameter(Mandatory = $true)][string]$InputFile,
    [Parameter(Mandatory = $true)][string]$OutputFile
)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

class G1MemoryProcess {
    [object]$StartInfo
    [int]$Number
    [int]$DisposeCalls = 0
    [bool]$HasExited = $false
    [int]$ExitCode = 0
    [void]Dispose() { $this.DisposeCalls += 1 }
}

function Get-G1State { return $global:ARTIST_G1_MEMORY_STATE_V1 }
function Copy-G1Object($Value) {
    $result = @{}
    foreach ($p in $Value.PSObject.Properties) { $result[$p.Name] = $p.Value }
    return $result
}
function Get-G1Hash([byte[]]$Bytes) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return ([BitConverter]::ToString($sha.ComputeHash($Bytes))).Replace('-', '').ToLowerInvariant() }
    finally { $sha.Dispose() }
}
function Get-G1TextHash([string]$Text) { return (Get-G1Hash ([Text.Encoding]::UTF8.GetBytes($Text))) }
function Read-G1Text([string]$Path) {
    return ([IO.File]::ReadAllText($Path)).Replace("`r`n", "`n")
}
function Assert-G1Owned([string]$Path) {
    $s = Get-G1State
    $full = [IO.Path]::GetFullPath($Path)
    if (-not [IO.Path]::IsPathRooted($Path) -or
        ($full -ine $s.root -and -not $full.StartsWith($s.root + '\', [StringComparison]::OrdinalIgnoreCase))) {
        throw 'HARNESS: effect escaped exact installed root.'
    }
    # No reparse-point traversal, including owned aliases to another owner.
    $cursor = $full
    while ($cursor.Length -ge $s.root.Length) {
        if ([IO.File]::Exists($cursor) -or [IO.Directory]::Exists($cursor)) {
            if (([IO.File]::GetAttributes($cursor) -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
                throw 'HARNESS: reparse point in owned path.'
            }
        }
        if ($cursor -ieq $s.root) { break }
        $cursor = [IO.Path]::GetDirectoryName($cursor)
    }
}
function Write-G1Bytes([string]$Path, [byte[]]$Bytes) {
    Assert-G1Owned $Path
    # Never create/adopt the bundle, phase directory or role directory here.
    if (-not [IO.Directory]::Exists([IO.Path]::GetDirectoryName($Path))) {
        throw 'HARNESS: real role owner has not created its directory.'
    }
    $stream = [IO.File]::Open($Path, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::None)
    try { $stream.Write($Bytes, 0, $Bytes.Length) }
    finally { $stream.Dispose() }
    # Independently reopen exclusively AFTER disposal and record actual bytes.
    $stream = [IO.File]::Open($Path, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::None)
    try { if ($stream.Length -ne $Bytes.Length) { throw 'HARNESS: binary writer size mismatch.' } }
    finally { $stream.Dispose() }
    $s = Get-G1State
    $s.writers.Add(@{ path = $Path; sha256 = (Get-G1Hash ([IO.File]::ReadAllBytes($Path))); closed = $true })
}
function Parse-G1([string]$Text) {
    $tokens = $null; $errors = $null
    $ast = [Management.Automation.Language.Parser]::ParseInput($Text, [ref]$tokens, [ref]$errors)
    if ($Text.Length -gt 262144 -or @($errors).Count -ne 0 -or $Text -match '(?im)^\s*#requires\b') {
        throw 'HARNESS: reviewed program syntax/bounds.'
    }
    return $ast
}
function Get-G1Region([string]$Source, [string]$Name) {
    $begin = '# BEGIN ARTIST_VERIFICATION_' + $Name
    $end = '# END ARTIST_VERIFICATION_' + $Name
    if ($Source.IndexOf($begin) -lt 0 -or $Source.IndexOf($begin) -ne $Source.LastIndexOf($begin) -or
        $Source.IndexOf($end) -lt 0 -or $Source.IndexOf($end) -ne $Source.LastIndexOf($end)) {
        throw 'HARNESS: original region population.'
    }
    $a = $Source.IndexOf($begin) + $begin.Length; $b = $Source.IndexOf($end)
    if ($a -ge $b) { throw 'HARNESS: original region order.' }
    return $Source.Substring($a, $b - $a)
}
function Assert-G1Program {
    $s = Get-G1State
    $pin = Get-Content -LiteralPath ($s.root + '/scripts/seam/native_binding_admission.json') -Raw | ConvertFrom-Json
    $source = Read-G1Text ($s.root + '/scripts/seam-original-build-windows.ps1.data')
    if ((Get-G1TextHash $source) -cne $pin.source_sha256) { throw 'HARNESS: original historical source pin.' }
    $native = Get-G1Region $source 'NATIVE'
    if ((Get-G1TextHash $native) -cne $pin.native_sha256 -or
        (Read-G1Text ($s.root + '/scripts/seam/native_source.sha256')).Trim() -cne $pin.native_sha256) {
        throw 'HARNESS: historical native pin.'
    }
    $parts = @{}
    foreach ($name in @('parameters', 'wrappers', 'data', 'root', 'entry', 'binding')) {
        $parts[$name] = Read-G1Text ($s.root + '/scripts/seam/' + $name + '.data')
        if ((Get-G1TextHash $parts[$name]) -cne $pin.fragments.$name) { throw 'HARNESS: fragment pin.' }
    }
    if ($parts.binding -cne (Get-G1Region $source 'BINDING') -or
        $parts.wrappers -cne (Get-G1Region $source 'WRAPPERS')) { throw 'HARNESS: fragment origin.' }
    $bindingAst = Parse-G1 $parts.binding
    $bindingNames = @('Get-VerificationContext', 'Copy-VerificationMap', 'Get-VerificationAbsolutePath',
        'Assert-VerificationRegularPath', 'New-VerificationOwnedDirectory', 'Write-VerificationPrivateJson',
        'Read-VerificationFlatStrings', 'Get-VerificationEnvironmentNames', 'Select-VerificationEnvironment',
        'Read-VerificationHostObservationLeaf', 'Read-VerificationClockLeaf', 'Enter-VerificationContext',
        'Exit-VerificationContext', 'Read-VerificationEnvironmentLeaf', 'Write-VerificationEnvironmentLeaf',
        'Set-VerificationCommandRole', 'New-VerificationProcessObjectLeaf', 'Resolve-VerificationExecutable',
        'Read-VerificationToolMetadataLeaf', 'Get-VerificationTooling', 'Initialize-VerificationTooling',
        'Invoke-VerificationProcessLeaf', 'Get-VerificationExporterSource', 'Get-VerificationToolsetDirectory',
        'Read-VerificationMsvcEnvironmentLeaf', 'Get-VerificationWitnessNames', 'Assert-VerificationPreparedTree',
        'Write-VerificationToolchainWitness')
    $seen = @()
    foreach ($node in $bindingAst.EndBlock.Statements) {
        if ($node -isnot [Management.Automation.Language.FunctionDefinitionAst]) { throw 'HARNESS: binding is definitions only.' }
        $seen += $node.Name
    }
    if (($seen -join '|') -cne ($bindingNames -join '|')) { throw 'HARNESS: exact binding population/order.' }
    $getter = @($bindingAst.EndBlock.Statements | Where-Object { $_.Name -ceq 'Get-VerificationExporterSource' })[0]
    $return = @($getter.Body.EndBlock.Statements)
    if ($return.Count -ne 1 -or $return[0] -isnot [Management.Automation.Language.ReturnStatementAst]) {
        throw 'HARNESS: exporter getter return shape.'
    }
    $pipeline = $return[0].Pipeline
    if ($pipeline.PipelineElements.Count -ne 1 -or
        $pipeline.PipelineElements[0] -isnot [Management.Automation.Language.CommandExpressionAst] -or
        $pipeline.PipelineElements[0].Expression -isnot [Management.Automation.Language.StringConstantExpressionAst]) {
        throw 'HARNESS: exporter must be a fixed literal.'
    }
    $exporter = $pipeline.PipelineElements[0].Expression.Value
    if ((Get-G1TextHash $exporter) -cne $pin.exporter_sha256 -or
        $exporter -cne (Read-G1Text ($s.root + '/scripts/seam/exporter-literal.data'))) {
        throw 'HARNESS: real exporter literal pin.'
    }
    $exportAst = Parse-G1 $exporter
    $statements = @($exportAst.EndBlock.Statements)
    if ($null -eq $exportAst.ParamBlock -or $exportAst.ParamBlock.Parameters.Count -ne 1 -or
        $statements.Count -ne 2 -or $statements[0] -isnot [Management.Automation.Language.FunctionDefinitionAst] -or
        $statements[0].Name -cne 'Write-VerificationMsvcExport' -or
        $statements[1].Extent.Text -cne 'Write-VerificationMsvcExport -Snapshot ([Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)) -OutputFile $OutputFile' -or
        $statements[0].Extent.Text -cne (Read-G1Text ($s.root + '/scripts/seam/exporter-body.ps1'))) {
        throw 'HARNESS: exporter body/OS entry separation.'
    }
    # Independently remove exactly FOUR binding leaves from pinned AST offsets.
    # The fifth port (native capture) was never included in the data fragment.
    $safe = $parts.binding
    foreach ($name in @('Read-VerificationHostObservationLeaf', 'Read-VerificationClockLeaf',
            'New-VerificationProcessObjectLeaf', 'Read-VerificationToolMetadataLeaf')) {
        $nodes = @( (Parse-G1 $safe).EndBlock.Statements )
        $index = -1
        for ($i = 0; $i -lt $nodes.Count; $i += 1) { if ($nodes[$i].Name -ceq $name) { $index = $i } }
        if ($index -lt 0 -or $index + 1 -ge $nodes.Count) { throw 'HARNESS: leaf removal boundary.' }
        $a = $nodes[$index].Extent.StartOffset; $b = $nodes[$index + 1].Extent.StartOffset
        $safe = $safe.Substring(0, $a) + $safe.Substring($b)
    }
    if ($safe -cne (Read-G1Text ($s.root + '/scripts/seam/safe-binding.data'))) { throw 'HARNESS: safe binding derivation.' }
    $ports = @('New-VerificationProcessObjectLeaf', 'Read-VerificationClockLeaf',
        'Read-VerificationHostObservationLeaf', 'Read-VerificationToolMetadataLeaf', 'Invoke-VerificationNativeProcess')
    $replacements = Read-G1Text ($s.root + '/scripts/seam/replacements.data')
    $replacementsAst = Parse-G1 $replacements
    $replacementNames = @($replacementsAst.EndBlock.Statements | ForEach-Object { $_.Name })
    if (($replacementNames -join '|') -cne ($ports -join '|')) { throw 'HARNESS: exactly five replacement ports.' }
    $program = Read-G1Text ($s.root + '/scripts/build-windows.ps1')
    $expected = $parts.parameters + "`n`$ErrorActionPreference = 'Stop'`n" + $parts.wrappers + $parts.data + $safe + $replacements + $parts.entry
    if ($program -cne $expected) { throw 'HARNESS: composed program is not exact admitted fragments.' }
    $ast = Parse-G1 $program
    if ($null -eq $ast.ParamBlock -or $ast.ParamBlock.Parameters.Count -ne 14) { throw 'HARNESS: parameter population.' }
    $functions = @($ast.EndBlock.Statements | Where-Object { $_ -is [Management.Automation.Language.FunctionDefinitionAst] })
    $functionNames = @($functions | ForEach-Object { $_.Name })
    $expectedNames = @('Resolve-VerificationInputs', 'ConvertTo-VerificationWindowsArgument',
        'Get-VerificationProcessEnvironment', 'Set-VerificationProcessEnvironment', 'Get-VerificationMsvcEnvironment',
        'Invoke-VerificationChild', 'Read-VerificationPayloadFile', 'Resolve-VerificationPhaseFlags', 'New-VerificationNativeStartInfo')
    $expectedNames += @($bindingNames | Where-Object { $ports -cnotcontains $_ })
    $expectedNames += $ports
    if (($functionNames -join '|') -cne ($expectedNames -join '|') -or
        (@($s.input.expected_functions) -join '|') -cne ($expectedNames -join '|')) { throw 'HARNESS: composed function population/order.' }
    # Exact source command allowlist; no dynamic invocation, external tools,
    # lifecycle or OS environment calls can survive the five-port composition.
    $allowed = $expectedNames + @('Split-Path', 'Join-Path', 'Test-Path', 'Get-Item', 'Get-ChildItem',
        'Get-Content', 'Set-Content', 'ConvertTo-Json', 'ConvertFrom-Json', 'New-Object', 'Write-Output',
        'Write-Error', 'Out-Null', 'Get-Variable', 'Set-Variable', 'Remove-Variable',
        'Skip-PayloadWhitespace', 'Require-PayloadCharacter', 'Read-PayloadString', 'Read-PayloadStringArray',
        'New-G1MemoryProcess', 'Read-G1Clock', 'Read-G1Host', 'Read-G1Metadata', 'Invoke-G1MemoryCapture')
    foreach ($node in $ast.FindAll({ param($n) $true }, $true)) {
        if ($node -is [Management.Automation.Language.CommandAst] -and
            ($node.InvocationOperator -ne [Management.Automation.Language.TokenKind]::Unknown -or
             $allowed -cnotcontains $node.GetCommandName())) { throw 'HARNESS: source command outside closed allowlist.' }
        if ($node -is [Management.Automation.Language.InvokeMemberExpressionAst] -and
            ($node.Member.Extent.Text -match '^(Start|Kill|WaitForExit|GetEnvironmentVariables|SetEnvironmentVariable|GetCurrentProcess|GetVersionInfo)$' -or
             $node.Expression.Extent.Text -match '^\[scriptblock\]$')) { throw 'HARNESS: forbidden source primitive.' }
        if ($node -is [Management.Automation.Language.VariableExpressionAst] -and
            $node.VariablePath.UserPath -match '^(global|env):') { throw 'HARNESS: source ambient state forbidden.' }
        if ($node -is [Management.Automation.Language.RedirectionAst] -or
            $node -is [Management.Automation.Language.UsingStatementAst]) { throw 'HARNESS: source redirection/using forbidden.' }
    }
    $s.audit = @{ functions = $functionNames; exporter_entry_parsed_not_run = $true
        composed_sha256 = (Get-G1TextHash $program); historical_reference_head = $pin.reference_head }
    # Exporter definition alone, parsed and matched above. NEVER evaluate its OS entry.
    return $statements[0].Extent.Text
}

function Read-G1Clock {
    $s = Get-G1State
    if ($s.tick -ge @($s.input.ticks).Count) { throw 'HARNESS: finite memory clock exhausted.' }
    $result = [long]$s.input.ticks[$s.tick]; $s.tick += 1
    return $result
}
function Read-G1Host {
    $s = Get-G1State
    $s.host_reads += 1
    return @{ environment = (Copy-G1Object $s.input.parent); process_executable = [string]$s.input.paths.host }
}
function Read-G1Metadata([string]$Program) {
    $s = Get-G1State
    Assert-G1Owned $Program
    $versions = @{ python = '3.13.0'; pip = '24.0'; conan = '2.12.2'; ninja = '1.13.0';
        cmake = '3.31.7'; ctest = '3.31.7'; cl = '14.44.35207'; vswhere = '3.1.7'; cmd = '10.0.26100.1'; host = '5.1' }
    foreach ($p in $s.input.paths.PSObject.Properties) {
        if ($Program -ieq [string]$p.Value -and [IO.File]::Exists($Program)) {
            $s.metadata_reads.Add($p.Name)
            return @{ version = $versions[$p.Name] }
        }
    }
    throw 'HARNESS: metadata for uninstalled program.'
}
function New-G1MemoryProcess([Diagnostics.ProcessStartInfo]$StartInfo) {
    $s = Get-G1State
    Assert-G1Owned $StartInfo.FileName
    if ($StartInfo.WorkingDirectory -ine $s.root -or $StartInfo.UseShellExecute -or
        -not $StartInfo.RedirectStandardOutput -or -not $StartInfo.RedirectStandardError) {
        throw 'HARNESS: unsafe real StartInfo reached memory factory.'
    }
    $process = [G1MemoryProcess]::new()
    $process.Number = $s.processes.Count + 1
    $process.StartInfo = $StartInfo
    $s.processes.Add($process)
    return $process
}
function Invoke-G1MemoryCapture($Process, [string]$LogDirectory, [int]$TimeoutMilliseconds) {
    $s = Get-G1State
    Assert-G1Owned $LogDirectory
    $expected = @('python-pip', 'vswhere', 'msvc-env', 'conan-install', 'cmake-configure')
    if ($s.input.phase -ceq 'build') { $expected = @('msvc-env', 'cmake-build') }
    $role = [IO.Path]::GetFileName($LogDirectory)
    if ($Process -isnot [G1MemoryProcess] -or $Process.Number -ne $s.roles.Count + 1 -or
        $s.roles.Count -ge $expected.Count -or $role -cne $expected[$s.roles.Count] -or
        $LogDirectory -ine ($s.input.bundle + '\logs\windows-helper-' + $s.input.phase + '\' + $role) -or
        $TimeoutMilliseconds -lt 1 -or $TimeoutMilliseconds -gt 3600000) { throw 'HARNESS: unplanned memory capture.' }
    $tools = Copy-G1Object $s.input.paths
    $program = @{ 'python-pip' = $tools.python; 'vswhere' = $tools.vswhere; 'msvc-env' = $tools.cmd;
        'conan-install' = $tools.conan; 'cmake-configure' = $tools.cmake; 'cmake-build' = $tools.cmake }
    if ($Process.StartInfo.FileName -ine $program[$role]) { throw 'HARNESS: role program differs.' }
    # Child environment comes from real context/selection/StartInfo. Compare
    # against explicit synthetic dictionaries BEFORE recording any value.
    $synthetic = Copy-G1Object $s.input.parent
    foreach ($p in $s.input.msvc.PSObject.Properties) { $synthetic[$p.Name] = $p.Value }
    $synthetic['CONAN_HOME'] = $s.root + '\out\conan2'
    $synthetic['CMAKE_BUILD_PARALLEL_LEVEL'] = '1'
    foreach ($key in $Process.StartInfo.EnvironmentVariables.Keys) {
        $value = [string]$Process.StartInfo.EnvironmentVariables[$key]
        if (-not $synthetic.ContainsKey($key)) { throw 'HARNESS: nonsynthetic environment key.' }
        if ($key -ieq 'PATH') {
            foreach ($part in $value.Split(';')) { Assert-G1Owned $part }
        } elseif ($value -ine [string]$synthetic[$key]) { throw 'HARNESS: nonsynthetic environment value.' }
    }
    $s.roles.Add($role)
    $code = 0
    $stdout = [byte[]]@(79, 85, 84, 0, 255)
    $stderr = [byte[]]@(69, 82, 82, 0, 254)
    $tree = $s.root + '\out\build\windows-msvc-debug-verification'
    switch ($role) {
        'vswhere' { $stdout = [Text.Encoding]::UTF8.GetBytes([string]$s.input.msvc.VSINSTALLDIR + "`r`n") }
        'msvc-env' {
            $transport = $s.input.bundle + '\logs\windows-helper-' + $s.input.phase + '\_transport'
            $exportFile = $transport + '\export-msvc.ps1'; $batchFile = $transport + '\load-msvc.cmd'
            foreach ($path in @($exportFile, $batchFile)) {
                Assert-G1Owned $path
                if (-not [IO.File]::Exists($path)) { throw 'HARNESS: real source transport absent.' }
            }
            if ((Get-G1TextHash (Read-G1Text $exportFile)) -cne $s.input.exporter_sha256) { throw 'HARNESS: source exporter transport changed.' }
            # This is the REAL pinned function body on dictionary data, not a
            # second capture, OS environment read or expected-witness producer.
            Write-VerificationMsvcExport (Copy-G1Object $s.input.msvc) ($transport + '\msvc-env.json')
            $stream = [IO.File]::Open(($transport + '\msvc-env.json'), [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::None)
            $stream.Dispose()
            $s.transports.Add(@{ exporter_sha256 = (Get-G1Hash ([IO.File]::ReadAllBytes($exportFile)))
                batch_sha256 = (Get-G1Hash ([IO.File]::ReadAllBytes($batchFile))); executed = $false
                export_writer_closed = $true
                export_json_base64 = [Convert]::ToBase64String([IO.File]::ReadAllBytes($transport + '\msvc-env.json')) })
        }
        'conan-install' {
            Write-G1Bytes ($tree + '\conan_toolchain.cmake') ([Text.Encoding]::UTF8.GetBytes('# SYNTHETIC Conan output; not compiled'))
            $code = [int]$s.input.conan_code
            if ($code -ne 0) {
                # Test-owned collection anomaly: first cause must remain child17.
                Write-G1Bytes ($LogDirectory + '\unexpected-owned.bin') ([byte[]]@(17, 0, 255))
            }
        }
        'cmake-configure' {
            $lines = @(('CMAKE_HOME_DIRECTORY:INTERNAL=' + $s.root),
                'CMAKE_GENERATOR:INTERNAL=Ninja', 'CMAKE_BUILD_TYPE:STRING=Debug',
                ('CMAKE_TOOLCHAIN_FILE:FILEPATH=' + $tree + '\conan_toolchain.cmake'),
                ('CMAKE_COMMAND:INTERNAL=' + $tools.cmake), ('CMAKE_CTEST_COMMAND:INTERNAL=' + $tools.ctest),
                ('CMAKE_MAKE_PROGRAM:FILEPATH=' + $tools.ninja), ('CMAKE_CXX_COMPILER:FILEPATH=' + $tools.cl),
                'ARTIST_BUILD_VERIFICATION:BOOL=ON', 'BUILD_TESTING:BOOL=OFF')
            Write-G1Bytes ($tree + '\CMakeCache.txt') ([Text.Encoding]::UTF8.GetBytes(($lines -join "`n")))
            Write-G1Bytes ($tree + '\build.ninja') ([Text.Encoding]::UTF8.GetBytes('# SYNTHETIC Ninja output; never invoked'))
        }
        'cmake-build' { Write-G1Bytes ($tree + '\ArtistUniformCpuWorkload.exe') ([Text.Encoding]::UTF8.GetBytes('INERT selected CPU target identity; never execute')) }
    }
    Write-G1Bytes ($LogDirectory + '\stdout.bin') $stdout
    Write-G1Bytes ($LogDirectory + '\stderr.bin') $stderr
    $Process.HasExited = $true; $Process.ExitCode = $code
    return @{ returncode = [int]$code; timed_out = $false; interrupted = $false; error = $null }
}

$report = @{ harness_error = $null; source_error = $null; code = 0; audit = $null
    roles = @(); processes = @(); writers = @(); transports = @(); host_reads = 0; clock_reads = 0; metadata_reads = @() }
$ownedState = $false
try {
    # The driver is installed three levels below the EXACT RuntimeFixture root.
    $installedRoot = [IO.Path]::GetFullPath($PSScriptRoot + '/../../..').TrimEnd('\')
    $g1Input = Get-Content -LiteralPath $InputFile -Raw | ConvertFrom-Json
    if ([IO.Path]::GetFullPath([string]$g1Input.root) -ine $installedRoot -or
        @('audit', 'prepare', 'build') -cnotcontains [string]$g1Input.phase) { throw 'HARNESS: exact installed root/phase required.' }
    $control = $installedRoot + '\out\seam-control\' + $g1Input.phase
    if ([IO.Path]::GetFullPath($InputFile) -ine ($control + '\input.json') -or
        [IO.Path]::GetFullPath($OutputFile) -ine ($control + '\result.json') -or
        -not [IO.File]::Exists($installedRoot + '\out\seam-control\installation.json')) {
        throw 'HARNESS: exact owned control paths required.'
    }
    if ($null -ne (Get-Variable -Name ARTIST_G1_MEMORY_STATE_V1 -Scope Global -ErrorAction SilentlyContinue)) { throw 'HARNESS: state collision.' }
    $state = @{ root = $installedRoot; input = $g1Input; tick = 0; host_reads = 0; audit = $null
        processes = (New-Object 'Collections.Generic.List[object]'); roles = (New-Object 'Collections.Generic.List[string]')
        writers = (New-Object 'Collections.Generic.List[object]'); transports = (New-Object 'Collections.Generic.List[object]')
        metadata_reads = (New-Object 'Collections.Generic.List[string]') }
    Set-Variable -Name ARTIST_G1_MEMORY_STATE_V1 -Scope Global -Value $state
    $ownedState = $true
    foreach ($p in $g1Input.program_identities.PSObject.Properties) {
        $path = $installedRoot + '/' + $p.Name
        Assert-G1Owned $path
        if ((Get-G1Hash ([IO.File]::ReadAllBytes($path))) -cne [string]$p.Value) { throw 'HARNESS: installed program changed.' }
    }
    $exporterBody = Assert-G1Program
    if ($g1Input.phase -cne 'audit') {
        foreach ($p in $g1Input.inert_identities.PSObject.Properties) {
            $path = $installedRoot + '/' + $p.Name
            Assert-G1Owned $path
            if ((Get-G1Hash ([IO.File]::ReadAllBytes($path))) -cne [string]$p.Value) { throw 'HARNESS: installed inert identity changed.' }
        }
        $bundle = [IO.Path]::GetFullPath([string]$g1Input.bundle)
        Assert-G1Owned $bundle
        if ([IO.Path]::GetDirectoryName($bundle) -ine ($installedRoot + '\out\verification')) { throw 'HARNESS: wrong bundle parent.' }
        foreach ($path in @(($bundle + '\.phase.lock'), ($installedRoot + '\out\build\windows-msvc-debug-verification\.verification-writer.lock'))) {
            if (-not [IO.File]::Exists($path)) { throw 'HARNESS: lease absent at driver admission.' }
        }
        $phaseDirectory = $bundle + '\logs\windows-helper-' + $g1Input.phase
        if (-not [IO.Directory]::Exists($phaseDirectory) -or [IO.Directory]::GetFileSystemEntries($phaseDirectory).Length -ne 0) {
            throw 'HARNESS: core must own an empty phase directory.'
        }
        $payload = $bundle + '\raw\windows-helper-input.json'
        $payloadStream = [IO.File]::Open($payload, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::None)
        $payloadStream.Dispose()
        if ((Get-G1Hash ([IO.File]::ReadAllBytes($payload))) -cne $g1Input.payload_sha256) { throw 'HARNESS: payload hash mismatch.' }
        $flags = @{}
        $argv = @($g1Input.argv)
        $pairs = @('Configuration', 'Jobs', 'VerificationPhase', 'VerificationInputFile', 'VerificationBundleRoot',
            'VerificationLogDirectory', 'VerificationTimeoutSeconds')
        $expected = @('Debug', '1', [string]$g1Input.phase, $payload, $bundle, $phaseDirectory, '3600')
        $count = 14
        if ($g1Input.phase -ceq 'prepare') { $count = 15 }
        if ($argv.Count -ne $count) { throw 'HARNESS: helper argv count.' }
        for ($i = 0; $i -lt $pairs.Count; $i += 1) {
            if ([string]$argv[2 * $i] -cne ('-' + $pairs[$i]) -or [string]$argv[2 * $i + 1] -ine $expected[$i]) {
                throw 'HARNESS: helper argv exact binding.'
            }
            $flags[$pairs[$i]] = $argv[2 * $i + 1]
        }
        if ($g1Input.phase -ceq 'prepare') {
            if ([string]$argv[14] -cne '-AllowDependencyResolution') { throw 'HARNESS: prepare permission missing.' }
            $flags['AllowDependencyResolution'] = $true
        }
        # Only these two reviewed evaluations exist. Neither evaluates full
        # original/native helper or the exporter's OS-reading entry point.
        . ([scriptblock]::Create($exporterBody))
        $global:LASTEXITCODE = 0
        try { & ($installedRoot + '\scripts\build-windows.ps1') @flags; $report.code = [int]$global:LASTEXITCODE }
        catch {
            if ($_.Exception.Message -match 'HARNESS:') { throw }
            $report.source_error = $_.Exception.Message
            $report.code = 92
        }
        foreach ($process in $state.processes) {
            if ($process.DisposeCalls -ne 1 -or
                @($process.PSObject.Methods.Name | Where-Object { @('Start', 'Kill', 'WaitForExit') -contains $_ }).Count -ne 0) {
                throw 'HARNESS: memory factory lifecycle/closure invariant.'
            }
            $report.processes += @{ number = $process.Number; dispose_calls = $process.DisposeCalls; code = $process.ExitCode
                program = $process.StartInfo.FileName; command_line = $process.StartInfo.Arguments; cwd = $process.StartInfo.WorkingDirectory }
        }
        $report.roles = @($state.roles.ToArray()); $report.writers = @($state.writers.ToArray())
        $report.transports = @($state.transports.ToArray()); $report.host_reads = $state.host_reads
        $report.clock_reads = $state.tick; $report.metadata_reads = @($state.metadata_reads.ToArray())
    }
    $report.audit = $state.audit
} catch {
    $report.harness_error = $_.Exception.Message
    $report.code = 91
} finally {
    if ($ownedState) { Remove-Variable -Name ARTIST_G1_MEMORY_STATE_V1 -Scope Global }
}
# No directory creation, bundle adoption, fabricated receipt or witness here.
if ($ownedState) {
    $raw = [Text.Encoding]::UTF8.GetBytes((ConvertTo-Json -InputObject $report -Depth 12 -Compress))
    $file = [IO.File]::Open($OutputFile, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::None)
    try { $file.Write($raw, 0, $raw.Length) } finally { $file.Dispose() }
}
exit ([int]$report.code)
