param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug',
    [ValidateRange(1, 64)][int]$Jobs = 1,
    [string]$VisualStudioPath,
    [string]$Python = 'python',
    [ValidateSet('all', 'unit', 'opengl')][string]$TestLabel = 'all',
    [switch]$AddressSanitizer,
    [switch]$ConfigureOnly,
    [switch]$AsanPreflightOnly,
    [ValidateSet('prepare', 'build', 'run')][string]$VerificationPhase,
    [switch]$AllowDependencyResolution,
    [string]$VerificationInputFile,
    [string]$VerificationBundleRoot,
    [string]$VerificationLogDirectory,
    [ValidateRange(1, 3600)][int]$VerificationTimeoutSeconds = 3600
)

$ErrorActionPreference = 'Stop'
# BEGIN ARTIST_VERIFICATION_WRAPPERS
# Logical wrappers; OS primitives are not wired by this intermediate slice.
function Resolve-VerificationInputs([hashtable]$Values, [string]$Root,
    [string]$Phase, [string]$Configuration) {
    if ($null -eq $Values -or $null -ne $Values['Keys'] -or $null -ne $Values['Count'] -or
        $Values.Count -ne 4) {
        throw 'InvalidVerificationInput: exactly four private fields are required.'
    }
    $names = @('build_directory', 'targets', 'executable', 'arguments')
    foreach ($name in $Values.Keys) {
        if ($names -cnotcontains $name) { throw 'InvalidVerificationInput: unknown private field.' }
    }
    if (@('prepare', 'build', 'run') -cnotcontains $Phase -or
        @('Debug', 'Release') -cnotcontains $Configuration -or $Root.Length -eq 0) {
        throw 'InvalidVerificationInput: unsupported phase, configuration or source root.'
    }
    if ($Values['build_directory'] -isnot [string] -or $Values['build_directory'].Length -eq 0) {
        throw 'InvalidVerificationInput: build directory must be a nonempty string.'
    }
    $expected = 'debug'
    if ($Configuration -ceq 'Release') { $expected = 'release' }
    try {
        $expected = [System.IO.Path]::GetFullPath($Root + '/out/build/windows-msvc-' + $expected + '-verification')
        $directory = [System.IO.Path]::GetFullPath($Values['build_directory'])
    } catch { throw 'InvalidVerificationInput: invalid build path.' }
    if ($directory -ine $expected) {
        throw 'InvalidVerificationInput: build tree is outside the selected toolchain/configuration.'
    }
    if ($Values['targets'] -isnot [array] -or $Values['targets'].Count -eq 0) {
        throw 'InvalidVerificationInput: a nonempty target array is required.'
    }
    $names = @('ArtistUniformCpuWorkload', 'ArtistUniformOpenGLWorkload', 'ArtistLib_tests', 'ArtistLib_component_tests')
    $targets = @()
    foreach ($target in $Values['targets']) {
        if ($target -isnot [string] -or $names -cnotcontains $target -or $targets -ccontains $target) {
            throw 'InvalidVerificationInput: unknown or duplicate target.'
        }
        $targets += $target
    }
    if ($Values['executable'] -isnot [string] -or $Values['arguments'] -isnot [array] -or
        $Values['arguments'].Count -gt 128) {
        throw 'InvalidVerificationInput: invalid executable or argument array.'
    }
    $arguments = @()
    $code = 0
    foreach ($argument in $Values['arguments']) {
        if ($argument -isnot [string]) { throw 'InvalidVerificationInput: arguments must be strings.' }
        # Public string limits count Unicode scalars, NOT UTF-16 code units.
        # A valid astral pair advances the UTF-16 index twice but counts once.
        $total = 0
        for ($index = 0; $index -lt $argument.Length; $index += 1) {
            $character = [int][char]($argument[$index])
            if ($character -eq 0) { throw 'InvalidVerificationInput: NUL argument.' }
            if ($character -ge 0xD800 -and $character -le 0xDBFF) {
                $index += 1
                if ($index -ge $argument.Length) { throw 'InvalidVerificationInput: unpaired high surrogate.' }
                $character = [int][char]($argument[$index])
                if ($character -lt 0xDC00 -or $character -gt 0xDFFF) {
                    throw 'InvalidVerificationInput: invalid surrogate pair.'
                }
            }
            elseif ($character -ge 0xDC00 -and $character -le 0xDFFF) {
                throw 'InvalidVerificationInput: unpaired low surrogate.'
            }
            $total += 1
            if ($total -gt 4096 -or ($code + $total) -gt 16384) {
                throw 'InvalidVerificationInput: scalar argument limits exceeded.'
            }
        }
        $code += $total
        $arguments += $argument
    }
    $executable = ''
    if ($Phase -ceq 'run') {
        if ($Values['executable'].Length -eq 0) { throw 'InvalidVerificationInput: run needs an executable.' }
        try { $executable = [System.IO.Path]::GetFullPath($Values['executable']) }
        catch { throw 'InvalidVerificationInput: invalid executable path.' }
        $expected = ''
        foreach ($target in $targets) {
            if (@('ArtistUniformCpuWorkload', 'ArtistUniformOpenGLWorkload') -ccontains $target) {
                $current = [System.IO.Path]::GetFullPath($directory + '/' + $target + '.exe')
                if ($executable -ieq $current) { $expected = $current }
            }
        }
        if ($expected -ceq '') { throw 'InvalidVerificationInput: executable is not a selected workload in the build tree.' }
    }
    elseif ($Values['executable'] -cne '' -or $arguments.Count -ne 0) {
        throw 'InvalidVerificationInput: prepare/build cannot carry a run command.'
    }
    return @{ build_directory = $directory; targets = $targets; executable = $executable; arguments = $arguments }
}

function ConvertTo-VerificationWindowsArgument([string]$Value) {
    $result = '"'
    $slashes = 0
    for ($index = 0; $index -lt $Value.Length; $index += 1) {
        $character = $Value[$index]
        if ([int][char]$character -eq 0) { throw 'InvalidVerificationInput: NUL cannot be encoded in native argv.' }
        if ($character -eq '\') { $slashes += 1; continue }
        if ($character -eq '"') { $result += ('\' * (2 * $slashes + 1)) + '"' }
        else { $result += ('\' * $slashes) + $character }
        $slashes = 0
    }
    $result += ('\' * (2 * $slashes)) + '"'
    return $result
}

function Get-VerificationProcessEnvironment {
    $current = Read-VerificationEnvironmentLeaf
    if ($current -isnot [hashtable]) { throw 'InvalidVerificationEnvironment: expected a map.' }
    # Hashtable addition copies without the shadowable .Keys/.Count accessors.
    return (@{} + $current)
}

function Set-VerificationProcessEnvironment([hashtable]$Values) {
    $current = Read-VerificationEnvironmentLeaf
    if ($current -isnot [hashtable] -or $null -eq $Values) {
        throw 'InvalidVerificationEnvironment: expected maps.'
    }
    # Raw keys are read-only here; environment entries named Keys/Count remain data.
    foreach ($name in @($current.PSBase.Keys)) {
        if ($Values.PSBase.Keys -notcontains $name) { Write-VerificationEnvironmentLeaf $name $null }
    }
    foreach ($name in @($Values.PSBase.Keys)) { Write-VerificationEnvironmentLeaf $name $Values[$name] }
}

function Get-VerificationMsvcEnvironment {
    $metadata = Read-VerificationMsvcEnvironmentLeaf
    if ($metadata -isnot [hashtable] -or $metadata['VCToolsVersion'] -isnot [string] -or
        $metadata['VCToolsVersion'] -notmatch '^14\.44\.[0-9]+\\?\z' -or
        $metadata['VSCMD_ARG_TGT_ARCH'] -cne 'x64' -or
        $metadata['WindowsSDKVersion'] -isnot [string] -or
        $metadata['WindowsSDKVersion'] -notmatch '^10\.0\.[0-9]+\.[0-9]+\\?\z') {
        throw 'InvalidMsvcEnvironment: expected toolset 14.44, x64 and Windows SDK metadata.'
    }
    # Metadata is not a compile/link proof of C++23 format or stacktrace support.
    return (@{} + $metadata)
}

function Invoke-VerificationChild([string]$Program, [string[]]$Arguments) {
    if ($Program.Length -eq 0) { throw 'InvalidVerificationInput: missing child program.' }
    $encoded = @()
    foreach ($argument in $Arguments) {
        $encoded += ConvertTo-VerificationWindowsArgument $argument
    }
    $commandLine = $encoded -join ' '
    # The future native leaf must bound the COMPLETE CreateProcessW command line
    # after quoting (argv0 + arguments + NUL), in UTF-16 units, not UTF-8 bytes.
    $code = Invoke-VerificationProcessLeaf $Program $commandLine
    if ($code -isnot [int]) { throw 'InvalidVerificationProcessResult: expected a raw integer exit code.' }
    return $code
}
# END ARTIST_VERIFICATION_WRAPPERS

# BEGIN ARTIST_VERIFICATION_NATIVE
# Native primitives only. The full helper remains closed below; loading this
# region is a separately reviewed source-pin admission, not a sandbox.
function Read-VerificationPayloadFile([string]$InputFile, [string]$BundleRoot) {
    $file = $null
    try {
        if ([string]::IsNullOrWhiteSpace($InputFile) -or [string]::IsNullOrWhiteSpace($BundleRoot) -or
            -not [IO.Path]::IsPathRooted($InputFile) -or -not [IO.Path]::IsPathRooted($BundleRoot)) {
            throw 'absolute bundle and input paths required'
        }
        $path = [IO.Path]::GetFullPath($InputFile)
        if ($path.Substring([IO.Path]::GetPathRoot($path).Length).IndexOf(':') -ge 0) { throw 'alternate data stream path' }
        $root = [IO.Path]::GetFullPath($BundleRoot).TrimEnd([char[]]@('\', '/')) + '\'
        if (-not $path.StartsWith($root, [StringComparison]::OrdinalIgnoreCase) -or
            -not [IO.Directory]::Exists($root)) { throw 'input must be inside an existing bundle' }
        # Inspect the file AND every ancestor, including ancestors of the bundle.
        # This is a local-path preflight, not a race-proof hostile-filesystem jail.
        $component = $path
        while ($null -ne $component) {
            $attributes = [IO.File]::GetAttributes($component)
            if (($attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'reparse component' }
            $parent = [IO.Directory]::GetParent($component)
            if ($null -eq $parent) { $component = $null } else { $component = $parent.FullName }
        }
        $file = [IO.File]::Open($path, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::Read)
        $length = $file.Length
        if ($length -lt 2 -or $length -gt 1048576) { throw 'payload size exceeds bounds' }
        $bytes = New-Object byte[] ([int]$length)
        $offset = 0
        while ($offset -lt $bytes.Length) {
            $count = $file.Read($bytes, $offset, $bytes.Length - $offset)
            if ($count -le 0) { throw 'short payload read' }
            $offset += $count
        }
        if ($file.ReadByte() -ne -1) { throw 'payload grew beyond its declared length' }
        $utf8 = New-Object Text.UTF8Encoding($false, $true)
        $text = $utf8.GetString($bytes)
        # A small strict reader for the existing PRIVATE four-field transport.
        # No ConvertFrom-Json duplicate-key collapse, dynamic evaluation, core
        # Request/receipt schema or dependency on a .NET Core JSON assembly.
        $cursor = @{ text = $text; index = 0 }
        function Skip-PayloadWhitespace {
            while ($cursor.index -lt $cursor.text.Length -and
                [int][char]$cursor.text[$cursor.index] -in @(0x20, 0x09, 0x0A, 0x0D)) { $cursor.index += 1 }
        }
        function Require-PayloadCharacter([char]$Expected) {
            Skip-PayloadWhitespace
            if ($cursor.index -ge $cursor.text.Length -or $cursor.text[$cursor.index] -cne $Expected) {
                throw 'unexpected JSON token'
            }
            $cursor.index += 1
        }
        function Read-PayloadString {
            Require-PayloadCharacter '"'
            $builder = New-Object Text.StringBuilder
            $closed = $false
            while ($cursor.index -lt $cursor.text.Length) {
                $character = $cursor.text[$cursor.index]; $cursor.index += 1
                if ($character -ceq '"') { $closed = $true; break }
                if ([int][char]$character -lt 0x20) { throw 'unescaped JSON control character' }
                if ($character -ceq '\') {
                    if ($cursor.index -ge $cursor.text.Length) { throw 'incomplete JSON escape' }
                    $escape = $cursor.text[$cursor.index]; $cursor.index += 1
                    switch -CaseSensitive ($escape) {
                        '"' { [void]$builder.Append('"') }
                        '\' { [void]$builder.Append('\') }
                        '/' { [void]$builder.Append('/') }
                        'b' { [void]$builder.Append([char]8) }
                        'f' { [void]$builder.Append([char]12) }
                        'n' { [void]$builder.Append([char]10) }
                        'r' { [void]$builder.Append([char]13) }
                        't' { [void]$builder.Append([char]9) }
                        'u' {
                            if ($cursor.index + 4 -gt $cursor.text.Length) { throw 'incomplete Unicode escape' }
                            $hex = $cursor.text.Substring($cursor.index, 4)
                            if ($hex -cnotmatch '\A[0-9a-fA-F]{4}\z') { throw 'invalid Unicode escape' }
                            [void]$builder.Append([char][Convert]::ToInt32($hex, 16)); $cursor.index += 4
                        }
                        default { throw 'invalid JSON escape' }
                    }
                } else { [void]$builder.Append($character) }
            }
            if (-not $closed) { throw 'unterminated JSON string' }
            $value = $builder.ToString()
            # UTF-8 decoding validates literal scalars; validate escaped pairs too.
            for ($index = 0; $index -lt $value.Length; $index += 1) {
                if ([char]::IsHighSurrogate($value[$index])) {
                    $index += 1
                    if ($index -ge $value.Length -or -not [char]::IsLowSurrogate($value[$index])) {
                        throw 'unpaired JSON surrogate'
                    }
                } elseif ([char]::IsLowSurrogate($value[$index])) { throw 'unpaired JSON surrogate' }
            }
            return $value
        }
        function Read-PayloadStringArray {
            Require-PayloadCharacter '['
            Skip-PayloadWhitespace
            $items = New-Object 'Collections.Generic.List[string]'
            if ($cursor.index -lt $cursor.text.Length -and $cursor.text[$cursor.index] -ceq ']') {
                $cursor.index += 1; return ,($items.ToArray())
            }
            while ($true) {
                $items.Add((Read-PayloadString))
                Skip-PayloadWhitespace
                if ($cursor.index -ge $cursor.text.Length) { throw 'unterminated JSON array' }
                if ($cursor.text[$cursor.index] -ceq ']') { $cursor.index += 1; break }
                Require-PayloadCharacter ','
            }
            return ,($items.ToArray())
        }
        Require-PayloadCharacter '{'
        $values = @{}
        while ($true) {
            $name = Read-PayloadString
            if (@('build_directory', 'targets', 'executable', 'arguments') -cnotcontains $name -or
                $values.ContainsKey($name)) { throw 'unknown or duplicate private field' }
            Require-PayloadCharacter ':'
            if ($name -ceq 'targets' -or $name -ceq 'arguments') { $values[$name] = Read-PayloadStringArray }
            else { $values[$name] = Read-PayloadString }
            Skip-PayloadWhitespace
            if ($cursor.index -ge $cursor.text.Length) { throw 'unterminated JSON object' }
            if ($cursor.text[$cursor.index] -ceq '}') { $cursor.index += 1; break }
            Require-PayloadCharacter ','
        }
        Skip-PayloadWhitespace
        if ($cursor.index -ne $cursor.text.Length -or $values.Count -ne 4) { throw 'missing fields or trailing JSON data' }
        return $values
    } catch { throw 'InvalidVerificationPayload: bounded UTF-8 private payload or bundle path is invalid.' }
    finally { if ($null -ne $file) { $file.Dispose() } }
}

function Resolve-VerificationPhaseFlags([hashtable]$Flags) {
    if ($null -eq $Flags) { throw 'InvalidVerificationFlags: a flags map is required.' }
    $names = @('Configuration', 'Jobs', 'VisualStudioPath', 'Python', 'TestLabel',
        'AddressSanitizer', 'ConfigureOnly', 'AsanPreflightOnly', 'VerificationPhase',
        'AllowDependencyResolution', 'VerificationInputFile', 'VerificationBundleRoot',
        'VerificationLogDirectory', 'VerificationTimeoutSeconds')
    foreach ($name in $Flags.PSBase.Keys) {
        if ($name -isnot [string] -or $names -cnotcontains $name) { throw 'InvalidVerificationFlags: unknown flag.' }
    }
    if ($Flags.ContainsKey('Configuration') -and @('Debug', 'Release') -cnotcontains $Flags['Configuration']) {
        throw 'InvalidVerificationFlags: unsupported configuration.'
    }
    if ($Flags.ContainsKey('Jobs') -and
        (($Flags['Jobs'] -isnot [int] -and $Flags['Jobs'] -isnot [long]) -or
         $Flags['Jobs'] -lt 1 -or $Flags['Jobs'] -gt 64)) { throw 'InvalidVerificationFlags: jobs must be 1..64.' }
    foreach ($name in @('VisualStudioPath', 'Python', 'TestLabel')) {
        if ($Flags.ContainsKey($name) -and ($Flags[$name] -isnot [string] -or
            [string]::IsNullOrWhiteSpace($Flags[$name]) -or $Flags[$name].IndexOf([char]0) -ge 0)) {
            throw 'InvalidVerificationFlags: invalid legacy string flag.'
        }
    }
    if ($Flags.ContainsKey('TestLabel') -and @('all', 'unit', 'opengl') -cnotcontains $Flags['TestLabel']) {
        throw 'InvalidVerificationFlags: unsupported test label.'
    }
    foreach ($name in @('AddressSanitizer', 'ConfigureOnly', 'AsanPreflightOnly', 'AllowDependencyResolution')) {
        if ($Flags.ContainsKey($name) -and $Flags[$name] -isnot [bool] -and
            $Flags[$name] -isnot [Management.Automation.SwitchParameter]) {
            throw 'InvalidVerificationFlags: switches must be booleans.'
        }
    }
    if (-not $Flags.ContainsKey('VerificationPhase')) {
        foreach ($name in @('AllowDependencyResolution', 'VerificationInputFile', 'VerificationBundleRoot',
            'VerificationLogDirectory', 'VerificationTimeoutSeconds')) {
            if ($Flags.ContainsKey($name)) { throw 'InvalidVerificationFlags: verification flags need an explicit phase.' }
        }
        if (($Flags['AddressSanitizer'] -and $Flags.ContainsKey('Configuration') -and $Flags['Configuration'] -cne 'Debug') -or
            ($Flags['AsanPreflightOnly'] -and (-not $Flags['AddressSanitizer'] -or $Flags['ConfigureOnly']))) {
            throw 'InvalidVerificationFlags: incompatible legacy ASan flags.'
        }
        return 'legacy'
    }
    $phase = $Flags['VerificationPhase']
    if ($phase -isnot [string] -or @('prepare', 'build', 'run') -cnotcontains $phase -or
        $Flags['ConfigureOnly'] -or $Flags['AddressSanitizer'] -or $Flags['AsanPreflightOnly'] -or
        $Flags.ContainsKey('TestLabel')) { throw 'InvalidVerificationFlags: verification and legacy modes are exclusive.' }
    foreach ($name in @('VerificationInputFile', 'VerificationBundleRoot', 'VerificationLogDirectory')) {
        if ($Flags[$name] -isnot [string] -or [string]::IsNullOrWhiteSpace($Flags[$name]) -or
            $Flags[$name].IndexOf([char]0) -ge 0) { throw 'InvalidVerificationFlags: missing private transport path.' }
    }
    if (($Flags['VerificationTimeoutSeconds'] -isnot [int] -and $Flags['VerificationTimeoutSeconds'] -isnot [long]) -or
        $Flags['VerificationTimeoutSeconds'] -lt 1 -or $Flags['VerificationTimeoutSeconds'] -gt 3600) {
        throw 'InvalidVerificationFlags: timeout must be 1..3600 seconds.'
    }
    if (($phase -ceq 'prepare' -and -not $Flags['AllowDependencyResolution']) -or
        ($phase -cne 'prepare' -and $Flags['AllowDependencyResolution'])) {
        throw 'InvalidVerificationFlags: dependency resolution is prepare-only and explicitly opted in.'
    }
    return $phase
}

function New-VerificationNativeStartInfo([string]$Program, [string]$CommandLine,
    [string]$WorkingDirectory, [hashtable]$Environment) {
    if ([string]::IsNullOrWhiteSpace($Program) -or $Program.IndexOfAny([char[]]@([char]0, '"')) -ge 0 -or
        -not [IO.Path]::IsPathRooted($Program) -or -not [IO.File]::Exists($Program) -or
        [IO.Path]::GetExtension($Program) -ine '.exe' -or
        [string]::IsNullOrWhiteSpace($WorkingDirectory) -or -not [IO.Path]::IsPathRooted($WorkingDirectory) -or
        -not [IO.Directory]::Exists($WorkingDirectory) -or $null -eq $Environment) {
        throw 'InvalidVerificationCommandLine: existing native executable, cwd and explicit environment required.'
    }
    # FileName becomes quoted argv0 (Windows filenames cannot contain quotes).
    # CommandLine is ALREADY quoted argv, never quote it again or count UTF-8.
    $units = [long]$Program.Length + 2 + 1
    if ($CommandLine.Length -gt 0) { $units += 1 + [long]$CommandLine.Length }
    if ($units -gt 32767 -or $CommandLine.IndexOf([char]0) -ge 0) {
        throw 'InvalidVerificationCommandLine: complete UTF-16 command including argv0 and NUL exceeds bounds.'
    }
    for ($index = 0; $index -lt $CommandLine.Length; $index += 1) {
        if ([char]::IsHighSurrogate($CommandLine[$index])) {
            $index += 1
            if ($index -ge $CommandLine.Length -or -not [char]::IsLowSurrogate($CommandLine[$index])) {
                throw 'InvalidVerificationCommandLine: unpaired command surrogate.'
            }
        } elseif ([char]::IsLowSurrogate($CommandLine[$index])) { throw 'InvalidVerificationCommandLine: unpaired command surrogate.' }
    }
    $info = New-Object Diagnostics.ProcessStartInfo
    $info.FileName = $Program; $info.Arguments = $CommandLine; $info.WorkingDirectory = $WorkingDirectory
    $info.UseShellExecute = $false; $info.CreateNoWindow = $true
    $info.RedirectStandardOutput = $true; $info.RedirectStandardError = $true
    $info.EnvironmentVariables.Clear()
    $units = 1
    foreach ($name in $Environment.PSBase.Keys) {
        if ($name -isnot [string] -or $name.Length -eq 0 -or $name.IndexOfAny([char[]]@([char]0, '=')) -ge 0 -or
            $Environment[$name] -isnot [string] -or $Environment[$name].IndexOf([char]0) -ge 0) {
            throw 'InvalidVerificationEnvironment: expected explicit string entries, including empty values.'
        }
        $units += [long]$name.Length + [long]$Environment[$name].Length + 2
        if ($units -gt 32767) { throw 'InvalidVerificationEnvironment: explicit environment block exceeds bounds.' }
        # Add, not a parent process-environment mutation. Empty stays present;
        # absent names (including all six public context names) stay absent.
        $info.EnvironmentVariables.Add($name, $Environment[$name])
    }
    return $info
}

function Invoke-VerificationNativeProcess([System.Diagnostics.Process]$Process,
    [string]$LogDirectory, [int]$TimeoutMilliseconds) {
    if ($null -eq $Process -or $TimeoutMilliseconds -lt 1 -or $TimeoutMilliseconds -gt 3600000 -or
        $Process.StartInfo.UseShellExecute -or -not $Process.StartInfo.CreateNoWindow -or
        -not $Process.StartInfo.RedirectStandardOutput -or -not $Process.StartInfo.RedirectStandardError) {
        throw 'InvalidVerificationProcess: unstarted owned process, redirects and bounded timeout required.'
    }
    # The caller owns this exact object/StartInfo. Never create another Process,
    # discover a PID, run a shell, or dispose the caller's Process here.
    $associated = $false
    try { [void]$Process.get_Id(); $associated = $true }
    catch { if ($_.Exception.GetBaseException() -isnot [InvalidOperationException]) { throw 'InvalidVerificationProcess: cannot inspect owned process.' } }
    if ($associated) { throw 'InvalidVerificationProcess: caller must provide an unstarted process.' }
    $stdoutFile = $null; $stderrFile = $null; $streams = @()
    $started = $false; $timedOut = $false; $returncode = $null
    $failures = New-Object 'Collections.Generic.List[string]'
    function Receive-OwnedNativeBytes($Streams) {
        foreach ($stream in $Streams) {
            if ($stream.done -or $stream.stopped -or $null -eq $stream.pending -or -not $stream.pending.IsCompleted) { continue }
            try {
                $task = $stream.pending; $stream.pending = $null
                $count = $task.GetAwaiter().GetResult() # completed only; cannot wait here
                if ($count -eq 0) { $stream.done = $true; continue }
                $stream.file.Write($stream.buffer, 0, $count)
                $stream.pending = $stream.source.ReadAsync($stream.buffer, 0, $stream.buffer.Length)
            } catch {
                $stream.stopped = $true
                throw 'InvalidVerificationProcess: raw stream read/write failed; partial logs retained.'
            }
        }
    }
    try {
        try {
            if ([string]::IsNullOrWhiteSpace($LogDirectory) -or -not [IO.Path]::IsPathRooted($LogDirectory)) { throw 'absolute log directory required' }
            $logs = [IO.Path]::GetFullPath($LogDirectory)
            if (-not [IO.Directory]::Exists($logs) -or
                ([IO.File]::GetAttributes($logs) -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'existing non-reparse log directory required' }
            $stdoutPath = [IO.Path]::Combine($logs, 'stdout.bin')
            $stderrPath = [IO.Path]::Combine($logs, 'stderr.bin')
            if ([IO.File]::Exists($stdoutPath) -or [IO.Directory]::Exists($stdoutPath) -or
                [IO.File]::Exists($stderrPath) -or [IO.Directory]::Exists($stderrPath)) { throw 'existing logs must not be overwritten' }
            # CreateNew enforces no-clobber even if a path appears after preflight.
            $stdoutFile = [IO.File]::Open($stdoutPath, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
            $stderrFile = [IO.File]::Open($stderrPath, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
        } catch { throw 'InvalidVerificationLog: raw log preflight/open failed before starting any child.' }
        $clock = [Diagnostics.Stopwatch]::StartNew()
        $started = $Process.Start()
        if (-not $started) { throw 'InvalidVerificationProcess: owned process was not started.' }
        $streams = @(
            @{ reader = $Process.StandardOutput; file = $stdoutFile; done = $false; stopped = $false; pending = $null },
            @{ reader = $Process.StandardError; file = $stderrFile; done = $false; stopped = $false; pending = $null }
        )
        foreach ($stream in $streams) {
            $stream.source = $stream.reader.BaseStream
            $stream.buffer = New-Object byte[] 65536
            $stream.pending = $stream.source.ReadAsync($stream.buffer, 0, $stream.buffer.Length)
        }
        while (-not $Process.get_HasExited()) {
            Receive-OwnedNativeBytes $streams
            if ($clock.ElapsedMilliseconds -ge $TimeoutMilliseconds) { $timedOut = $true; break }
            [Threading.Thread]::Sleep(10) # polling is bounded by the execution deadline
        }
        if (-not $timedOut) { $returncode = $Process.ExitCode }
    } catch {
        if ($_.Exception.Message.StartsWith('InvalidVerification')) { $failures.Add($_.Exception.Message) }
        else { $failures.Add('InvalidVerificationProcess: native start/capture failed; partial raw logs retained.') }
    }
    finally {
        if ($started) {
            try {
                if (-not $Process.get_HasExited()) {
                    $Process.Kill() # only this owned child, once; no descendants/PID lookup
                    if (-not $Process.WaitForExit(2000)) { throw 'owned child did not stop' }
                }
            } catch { $failures.Add('InvalidVerificationProcess: owned-child stop was not confirmed within 2000 ms; partial logs retained.') }
            try {
                $drainClock = [Diagnostics.Stopwatch]::StartNew()
                while (@($streams | Where-Object { -not $_.done -and -not $_.stopped }).Count -gt 0) {
                    Receive-OwnedNativeBytes $streams
                    if ($drainClock.ElapsedMilliseconds -ge 2000) { throw 'raw stream drain deadline expired' }
                    [Threading.Thread]::Sleep(10)
                }
            } catch { $failures.Add('InvalidVerificationProcess: raw drain failed or exceeded 2000 ms; partial logs retained.') }
        }
        # Close pipe readers before observing any unfinished ReadAsync task.
        # There are at most two 64-KiB reads, no unowned worker/script/job. A
        # child stop closes writer handles in the finite, descendant-free fake.
        foreach ($stream in $streams) {
            try { $stream.reader.Dispose() }
            catch { $failures.Add('InvalidVerificationProcess: raw reader disposal failed; partial logs retained.') }
            if ($null -ne $stream.pending) {
                try {
                    if (-not $stream.pending.Wait(1000)) { throw 'raw reader task still pending' }
                } catch { $failures.Add('InvalidVerificationProcess: raw reader task failed or was not reclaimed within 1000 ms.') }
            }
        }
        foreach ($file in @($stdoutFile, $stderrFile)) {
            if ($null -ne $file) {
                try { $file.Dispose() }
                catch { $failures.Add('InvalidVerificationLog: closing partial raw logs failed.') }
            }
        }
    }
    if ($failures.Count -gt 0) { throw ($failures -join ' | ') }
    return @{ returncode = $returncode; timed_out = $timedOut; stdout_path = $stdoutPath; stderr_path = $stderrPath }
}
# END ARTIST_VERIFICATION_NATIVE

# BEGIN ARTIST_VERIFICATION_BINDING
# Private to one admitted helper invocation. No process environment mutation.
function Get-VerificationContext {
    $binding = Get-Variable -Name ArtistVerificationContext -Scope Script -ErrorAction SilentlyContinue
    if ($null -eq $binding -or $binding.Value -isnot [hashtable]) {
        throw 'InvalidVerificationContext: no active invocation.'
    }
    return $binding.Value
}

function Copy-VerificationMap([hashtable]$Values) { return (@{} + $Values) }

function Get-VerificationAbsolutePath([string]$Value) {
    if ([string]::IsNullOrWhiteSpace($Value) -or $Value.IndexOf([char]0) -ge 0 -or
        $Value -notmatch '^(?:[A-Za-z]:[\\/]|\\\\[^\\/]+\\[^\\/]+(?:\\|$))') {
        throw 'InvalidVerificationInput: fully qualified Windows path required.'
    }
    try { return [IO.Path]::GetFullPath($Value) }
    catch { throw 'InvalidVerificationInput: malformed absolute path.' }
}

function Assert-VerificationRegularPath([string]$Path, [bool]$Directory) {
    $pathValue = Get-VerificationAbsolutePath $Path
    if (($Directory -and -not [IO.Directory]::Exists($pathValue)) -or
        (-not $Directory -and -not [IO.File]::Exists($pathValue))) {
        throw 'InvalidVerificationInput: required regular path is absent.'
    }
    $component = $pathValue
    while ($null -ne $component) {
        $attributes = [IO.File]::GetAttributes($component)
        if (($attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
            throw 'InvalidVerificationInput: reparse path is not owned.'
        }
        $parent = [IO.Directory]::GetParent($component)
        if ($null -eq $parent) { $component = $null } else { $component = $parent.FullName }
    }
}

function New-VerificationOwnedDirectory([string]$Path) {
    $value = Get-VerificationAbsolutePath $Path
    if ([IO.File]::Exists($value) -or [IO.Directory]::Exists($value)) {
        throw 'InvalidVerificationLog: existing directory cannot be adopted.'
    }
    Assert-VerificationRegularPath ([IO.Path]::GetDirectoryName($value)) $true
    # The core holds the bundle lease. This is not a hostile-filesystem jail.
    [void][IO.Directory]::CreateDirectory($value)
    Assert-VerificationRegularPath $value $true
}

function Write-VerificationPrivateJson([string]$Path, [hashtable]$Values) {
    Assert-VerificationRegularPath ([IO.Path]::GetDirectoryName($Path)) $true
    $text = ConvertTo-Json -InputObject $Values -Depth 4 -Compress
    $bytes = [Text.Encoding]::UTF8.GetBytes($text)
    $file = [IO.File]::Open($Path, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
    $failure = $null
    try { $file.Write($bytes, 0, $bytes.Length) } catch { $failure = $_.Exception.Message }
    finally {
        try { $file.Dispose() } catch {
            if ($null -eq $failure) { $failure = $_.Exception.Message } else { $failure += ' | close: ' + $_.Exception.Message }
        }
    }
    if ($null -ne $failure) { throw $failure }
}

function Read-VerificationFlatStrings([string]$Path, [string[]]$Names, [bool]$RequireAll) {
    Assert-VerificationRegularPath $Path $false
    $file = [IO.File]::Open($Path, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::Read)
    $failure = $null
    try {
        if ($file.Length -lt 2 -or $file.Length -gt 65536) { throw 'InvalidVerificationInput: flat transport exceeds bounds.' }
        $bytes = New-Object byte[] ([int]$file.Length)
        $offset = 0
        while ($offset -lt $bytes.Length) {
            $count = $file.Read($bytes, $offset, $bytes.Length - $offset)
            if ($count -le 0) { throw 'InvalidVerificationInput: short private transport read.' }
            $offset += $count
        }
        if ($file.ReadByte() -ne -1) { throw 'InvalidVerificationInput: private transport grew.' }
    } catch { $failure = $_.Exception.Message }
    finally {
        try { $file.Dispose() } catch {
            if ($null -eq $failure) { $failure = $_.Exception.Message } else { $failure += ' | close: ' + $_.Exception.Message }
        }
    }
    if ($null -ne $failure) { throw $failure }
    $utf8 = New-Object Text.UTF8Encoding($false, $true)
    $text = $utf8.GetString($bytes)
    # Only a flat object of quoted strings, not a general JSON reader. Decode
    # each pair as an object so empty strings do not traverse a scalar return.
    $stringToken = '"(?:[^"\\\x00-\x1f]|\\(?:["\\/bfnrt]|u[0-9a-fA-F]{4}))*"'
    $pattern = '\G\s*(?<key>' + $stringToken + ')\s*:\s*(?<value>' + $stringToken + ')\s*'
    $pairReader = New-Object Text.RegularExpressions.Regex($pattern)
    $open = [Text.RegularExpressions.Regex]::Match($text, '^\s*\{\s*')
    if (-not $open.Success) { throw 'InvalidVerificationInput: flat JSON object required.' }
    $position = $open.Length; $values = @{}
    if ($position -lt $text.Length -and $text[$position] -cne '}') {
        while ($true) {
            $pair = $pairReader.Match($text, $position)
            if (-not $pair.Success -or $pair.Index -ne $position) { throw 'InvalidVerificationInput: flat string pair required.' }
            $decoded = ConvertFrom-Json -InputObject ('{"key":' + $pair.Groups['key'].Value + ',"value":' + $pair.Groups['value'].Value + '}')
            if ($Names -cnotcontains $decoded.key -or $values.ContainsKey($decoded.key) -or
                $decoded.value -isnot [string] -or $decoded.value.IndexOf([char]0) -ge 0) {
                throw 'InvalidVerificationInput: unknown, duplicate or invalid private string.'
            }
            $values[$decoded.key] = $decoded.value
            $position = $pair.Index + $pair.Length
            if ($position -lt $text.Length -and $text[$position] -ceq ',') { $position += 1; continue }
            break
        }
    }
    if ($position -ge $text.Length -or $text.Substring($position) -notmatch '^}\s*$' -or
        ($RequireAll -and $values.Count -ne $Names.Count)) { throw 'InvalidVerificationInput: incomplete flat private object.' }
    if ($RequireAll) {
        foreach ($name in $Names) { if ([string]::IsNullOrWhiteSpace($values[$name])) { throw 'InvalidVerificationInput: nonempty private strings required.' } }
    }
    return $values
}

function Get-VerificationEnvironmentNames {
    return @('PATH', 'SystemRoot', 'WINDIR', 'SystemDrive', 'COMSPEC', 'PATHEXT', 'TEMP', 'TMP',
        'USERPROFILE', 'APPDATA', 'LOCALAPPDATA', 'HOMEDRIVE', 'HOMEPATH', 'ProgramFiles', 'ProgramFiles(x86)',
        'INCLUDE', 'LIB', 'LIBPATH', 'VSINSTALLDIR', 'VCINSTALLDIR', 'VCToolsInstallDir', 'VCToolsVersion',
        'VSCMD_ARG_TGT_ARCH', 'VSCMD_ARG_HOST_ARCH', 'VSCMD_VER', 'DevEnvDir', 'WindowsSdkDir',
        'WindowsSDKVersion', 'WindowsSDKLibVersion', 'UniversalCRTSdkDir', 'UCRTVersion',
        'CONAN_HOME', 'CMAKE_BUILD_PARALLEL_LEVEL', 'OS', 'PROCESSOR_ARCHITECTURE', 'PROCESSOR_ARCHITEW6432')
}

function Select-VerificationEnvironment([hashtable]$Values) {
    $selected = @{}
    foreach ($name in (Get-VerificationEnvironmentNames)) {
        if ($Values.ContainsKey($name)) {
            if ($Values[$name] -isnot [string] -or $Values[$name].IndexOf([char]0) -ge 0) {
                throw 'InvalidVerificationEnvironment: string entries required.'
            }
            $selected[$name] = $Values[$name]
        }
    }
    return $selected
}

function Read-VerificationHostObservationLeaf {
    [Collections.IDictionary]$block = [Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)
    $values = @{}
    foreach ($name in $block.PSBase.Keys) { $values[$name] = $block[$name] }
    $self = [Diagnostics.Process]::GetCurrentProcess()
    try { $executable = $self.MainModule.FileName } finally { $self.Dispose() }
    return @{ environment = $values; process_executable = $executable }
}

function Read-VerificationClockLeaf {
    return [long][Math]::Floor(1000.0 * [Diagnostics.Stopwatch]::GetTimestamp() / [Diagnostics.Stopwatch]::Frequency)
}

function Enter-VerificationContext([hashtable]$Admission) {
    if ($null -ne (Get-Variable -Name ArtistVerificationContext -Scope Script -ErrorAction SilentlyContinue)) {
        throw 'InvalidVerificationContext: invocation is already active; reentrance refused.'
    }
    if ($null -eq $Admission -or @('prepare', 'build') -cnotcontains $Admission.phase -or
        @('Debug', 'Release') -cnotcontains $Admission.configuration -or
        $Admission.timeout_milliseconds -lt 1 -or $Admission.timeout_milliseconds -gt 3600000 -or
        $Admission.jobs -lt 1 -or $Admission.jobs -gt 64) { throw 'InvalidVerificationContext: admitted values required.' }
    $rootValue = Get-VerificationAbsolutePath $Admission.root
    $bundleValue = Get-VerificationAbsolutePath $Admission.bundle_root
    $logValue = Get-VerificationAbsolutePath $Admission.log_directory
    if ($logValue -ine [IO.Path]::Combine($bundleValue, 'logs', ('windows-helper-' + $Admission.phase))) {
        throw 'InvalidVerificationInput: phase logs must belong to the admitted bundle.'
    }
    Assert-VerificationRegularPath $logValue $true
    $hostObservation = Read-VerificationHostObservationLeaf
    if ($hostObservation -isnot [hashtable] -or $hostObservation.environment -isnot [hashtable]) {
        throw 'InvalidVerificationEnvironment: explicit host observation required.'
    }
    $context = @{
        Root = $rootValue; Bundle = $bundleValue; LogDirectory = $logValue
        BuildDirectory = (Get-VerificationAbsolutePath $Admission.build_directory)
        Phase = $Admission.phase; Configuration = $Admission.configuration; Jobs = [int]$Admission.jobs
        WorkingEnvironment = (Copy-VerificationMap $hostObservation.environment)
        HostExecutable = (Get-VerificationAbsolutePath $hostObservation.process_executable)
        RequestedPython = [string]$Admission.python; RequestedVisualStudio = [string]$Admission.visual_studio_installation
        Deadline = ([long](Read-VerificationClockLeaf) + [long]$Admission.timeout_milliseconds)
        Tools = @{}; UsedRoles = @{}; CurrentRole = ''; Traces = (New-Object 'Collections.Generic.List[object]')
        SelectedVisualStudio = ''; Msvc = $null; Prepared = $null; BootstrapPython = ''; Vswhere = ''; Cmd = ''
    }
    if ($Admission.ContainsKey('prepared_witness')) { $context.Prepared = $Admission.prepared_witness }
    Set-Variable -Name ArtistVerificationContext -Scope Script -Value $context
}

function Exit-VerificationContext {
    [void](Get-VerificationContext)
    Remove-Variable -Name ArtistVerificationContext -Scope Script -ErrorAction Stop
}

function Read-VerificationEnvironmentLeaf {
    $context = Get-VerificationContext
    return (Copy-VerificationMap $context.WorkingEnvironment)
}

function Write-VerificationEnvironmentLeaf([string]$Name, $Value) {
    $context = Get-VerificationContext
    if ([string]::IsNullOrEmpty($Name) -or $Name.IndexOfAny([char[]]@([char]0, '=')) -ge 0 -or
        ($null -ne $Value -and ($Value -isnot [string] -or $Value.IndexOf([char]0) -ge 0))) {
        throw 'InvalidVerificationEnvironment: invalid private environment entry.'
    }
    if ($null -eq $Value) { $context.WorkingEnvironment.Remove($Name) }
    else { $context.WorkingEnvironment[$Name] = $Value }
}

function Set-VerificationCommandRole([string]$Role) {
    $context = Get-VerificationContext
    $allowed = @('msvc-env', 'cmake-build')
    if ($context.Phase -ceq 'prepare') { $allowed = @('python-venv', 'python-ensurepip', 'python-pip', 'vswhere', 'msvc-env', 'conan-install', 'cmake-configure') }
    if ($allowed -cnotcontains $Role -or $context.UsedRoles.ContainsKey($Role)) {
        throw 'InvalidVerificationProcess: unknown or repeated phase role.'
    }
    $context.CurrentRole = $Role
}

function New-VerificationProcessObjectLeaf([Diagnostics.ProcessStartInfo]$StartInfo) {
    $process = New-Object Diagnostics.Process
    $process.StartInfo = $StartInfo
    return $process
}

function Resolve-VerificationExecutable([string]$Value, [hashtable]$Environment) {
    if ([string]::IsNullOrWhiteSpace($Value) -or $Value.IndexOfAny([char[]]@([char]0, '"')) -ge 0) {
        throw 'InvalidVerificationTooling: executable name is invalid.'
    }
    if ([IO.Path]::IsPathRooted($Value)) {
        $path = Get-VerificationAbsolutePath $Value
        if (-not [IO.File]::Exists($path) -or [IO.Path]::GetExtension($path) -ine '.exe') { throw 'InvalidVerificationTooling: existing executable required.' }
        return $path
    }
    if ($Value.IndexOfAny([char[]]@('\', '/', ':')) -ge 0 -or -not $Environment.ContainsKey('PATH')) {
        throw 'InvalidVerificationTooling: no cwd-relative executable resolution.'
    }
    $name = $Value
    if ([IO.Path]::GetExtension($name) -ceq '') { $name += '.exe' }
    foreach ($directory in $Environment['PATH'].Split(';')) {
        if ([string]::IsNullOrWhiteSpace($directory) -or -not [IO.Path]::IsPathRooted($directory.Trim('"'))) { continue }
        $candidate = [IO.Path]::Combine($directory.Trim('"'), $name)
        if ([IO.File]::Exists($candidate)) { return (Get-VerificationAbsolutePath $candidate) }
    }
    throw 'InvalidVerificationTooling: preinstalled executable is absent; no fallback.'
}

function Read-VerificationToolMetadataLeaf([string]$Program) {
    $path = Get-VerificationAbsolutePath $Program
    if (-not [IO.File]::Exists($path)) { throw 'InvalidVerificationTooling: metadata executable is absent.' }
    $name = [IO.Path]::GetFileNameWithoutExtension($path)
    if (@('conan', 'ninja') -contains $name) {
        $venv = [IO.Directory]::GetParent([IO.Path]::GetDirectoryName($path)).FullName
        $site = [IO.Path]::Combine($venv, 'Lib', 'site-packages')
        $versions = @()
        if ([IO.Directory]::Exists($site)) {
            foreach ($directory in [IO.Directory]::GetDirectories($site, ($name + '-*.dist-info'))) {
                $metadata = [IO.Path]::Combine($directory, 'METADATA')
                if ([IO.File]::Exists($metadata) -and (New-Object IO.FileInfo($metadata)).Length -le 65536) {
                    foreach ($line in [IO.File]::ReadAllLines($metadata)) {
                        if ($line.StartsWith('Version: ')) { $versions += $line.Substring(9).Trim() }
                    }
                }
            }
        }
        if ($versions.Count -ne 1) { throw 'InvalidVerificationTooling: ambiguous installed package metadata.' }
        return @{ version = $versions[0] }
    }
    $metadata = [Diagnostics.FileVersionInfo]::GetVersionInfo($path)
    $match = [Text.RegularExpressions.Regex]::Match([string]$metadata.ProductVersion, '\d+\.\d+(?:\.\d+){0,2}')
    if (-not $match.Success) { throw 'InvalidVerificationTooling: executable version metadata is absent.' }
    return @{ version = $match.Value }
}

function Get-VerificationTooling([string]$Root) {
    $context = Get-VerificationContext
    if ((Get-VerificationAbsolutePath $Root) -ine $context.Root) { throw 'InvalidVerificationTooling: source root differs.' }
    $scripts = [IO.Path]::Combine([string[]]@($context.Root, 'out', 'tooling', 'build', 'Scripts'))
    $tools = @{ python = [IO.Path]::Combine($scripts, 'python.exe'); conan = [IO.Path]::Combine($scripts, 'conan.exe')
        ninja = [IO.Path]::Combine($scripts, 'ninja.exe'); cmake = '' }
    if ($context.Phase -ceq 'build') {
        if ($context.Prepared -isnot [hashtable]) { throw 'InvalidVerificationPreparedTree: admitted witness required.' }
        foreach ($name in @('python', 'conan', 'ninja')) {
            if ((Get-VerificationAbsolutePath $context.Prepared[$name]) -ine $tools[$name]) { throw 'InvalidVerificationTooling: prepared tooling path differs.' }
        }
        $tools.cmake = Get-VerificationAbsolutePath $context.Prepared.cmake
    } else { $tools.cmake = Resolve-VerificationExecutable 'cmake.exe' $context.WorkingEnvironment }
    foreach ($name in @('python', 'conan', 'ninja', 'cmake')) {
        if (-not [IO.File]::Exists($tools[$name])) { throw 'InvalidVerificationTooling: existing tooling required; no build repair.' }
        $metadata = Read-VerificationToolMetadataLeaf $tools[$name]
        $version = $null
        if ($metadata -isnot [hashtable] -or -not [version]::TryParse([string]$metadata.version, [ref]$version) -or
            ($name -ceq 'python' -and $version -lt [version]'3.10') -or
            ($name -ceq 'cmake' -and $version -lt [version]'3.25') -or
            ($name -ceq 'conan' -and $metadata.version -cne '2.12.2') -or
            ($name -ceq 'ninja' -and $metadata.version -cne '1.13.0')) {
            throw 'InvalidVerificationTooling: declared installed tooling version is unsupported.'
        }
    }
    $context.Tools = $tools
    return (Copy-VerificationMap $tools)
}

function Initialize-VerificationTooling([string]$Root, [string]$Python) {
    $context = Get-VerificationContext
    if ($context.Phase -cne 'prepare' -or (Get-VerificationAbsolutePath $Root) -ine $context.Root) {
        throw 'InvalidVerificationTooling: bootstrap is prepare-only.'
    }
    $context.BootstrapPython = Resolve-VerificationExecutable $Python $context.WorkingEnvironment
    # CMake is a preinstalled application, never a pip requirement.
    [void](Resolve-VerificationExecutable 'cmake.exe' $context.WorkingEnvironment)
    $venv = [IO.Path]::Combine($context.Root, 'out', 'tooling', 'build')
    $pythonPath = [IO.Path]::Combine($venv, 'Scripts', 'python.exe')
    $context.Tools.python = $pythonPath
    if (-not [IO.File]::Exists($pythonPath)) {
        Set-VerificationCommandRole 'python-venv'
        $code = Invoke-VerificationChild $context.BootstrapPython @('-m', 'venv', $venv)
        if ($code -ne 0) { throw ('InvalidVerificationTooling: venv failed with raw code ' + $code) }
    }
    if (-not [IO.File]::Exists([IO.Path]::Combine($venv, 'Scripts', 'pip.exe'))) {
        Set-VerificationCommandRole 'python-ensurepip'
        $code = Invoke-VerificationChild $pythonPath @('-m', 'ensurepip', '--upgrade')
        if ($code -ne 0) { throw ('InvalidVerificationTooling: ensurepip failed with raw code ' + $code) }
    }
    Set-VerificationCommandRole 'python-pip'
    $code = Invoke-VerificationChild $pythonPath @('-m', 'pip', 'install', '--disable-pip-version-check', '-r', 'scripts/build-requirements.txt')
    if ($code -ne 0) { throw ('InvalidVerificationTooling: pip failed with raw code ' + $code) }
    return (Get-VerificationTooling $Root)
}

function Invoke-VerificationProcessLeaf([string]$Program, [string]$CommandLine) {
    $context = Get-VerificationContext
    $remaining = [long]$context.Deadline - [long](Read-VerificationClockLeaf)
    if ($remaining -le 0) { throw 'InvalidVerificationBudget: invocation deadline exhausted before launch.' }
    $role = [string]$context.CurrentRole
    if ($role.Length -eq 0 -or $context.UsedRoles.ContainsKey($role)) { throw 'InvalidVerificationProcess: unbound or repeated command role.' }
    $expected = ''
    switch ($role) {
        'python-venv' { $expected = $context.BootstrapPython }
        'python-ensurepip' { $expected = $context.Tools.python }
        'python-pip' { $expected = $context.Tools.python }
        'vswhere' { $expected = $context.Vswhere }
        'msvc-env' { $expected = $context.Cmd }
        'conan-install' { $expected = $context.Tools.conan }
        'cmake-configure' { $expected = $context.Tools.cmake }
        'cmake-build' { $expected = $context.Tools.cmake }
        default { throw 'InvalidVerificationProcess: no program for role.' }
    }
    $programPath = Get-VerificationAbsolutePath $Program
    if ($programPath -ine (Get-VerificationAbsolutePath $expected)) { throw 'InvalidVerificationProcess: program differs from selected tool.' }
    $logDirectory = [IO.Path]::Combine($context.LogDirectory, $role)
    New-VerificationOwnedDirectory $logDirectory
    $context.UsedRoles[$role] = $true
    $environment = Select-VerificationEnvironment $context.WorkingEnvironment
    $environment['CONAN_HOME'] = [IO.Path]::Combine($context.Root, 'out', 'conan2')
    $environment['CMAKE_BUILD_PARALLEL_LEVEL'] = [string]$context.Jobs
    $scripts = [IO.Path]::Combine([string[]]@($context.Root, 'out', 'tooling', 'build', 'Scripts'))
    if ($environment.ContainsKey('PATH')) { $environment['PATH'] = $scripts + ';' + $environment['PATH'] }
    else { $environment['PATH'] = $scripts }
    if ($context.Msvc -is [hashtable]) { $environment['PATH'] = [IO.Path]::GetDirectoryName($context.Tools.cl) + ';' + $environment['PATH'] }
    $process = $null; $info = $null; $returncode = $null; $timedOut = $false; $interrupted = $false
    $errors = New-Object 'Collections.Generic.List[string]'
    try {
        $info = New-VerificationNativeStartInfo $programPath $CommandLine $context.Root $environment
        $process = New-VerificationProcessObjectLeaf $info
        if ($null -eq $process) { throw 'InvalidVerificationProcess: factory returned no owned process.' }
        $outcome = Invoke-VerificationNativeProcess $process $logDirectory ([int][Math]::Min(3600000, $remaining))
        if ($outcome -isnot [hashtable] -or $outcome.timed_out -isnot [bool] -or
            ($null -ne $outcome.returncode -and $outcome.returncode -isnot [int])) {
            throw 'InvalidVerificationProcess: invalid native outcome.'
        }
        $returncode = $outcome.returncode; $timedOut = $outcome.timed_out
        if ($timedOut -or $null -eq $returncode) { $errors.Add('InvalidVerificationProcess: timeout or missing native code.') }
    } catch {
        $errors.Add($_.Exception.Message)
        if ($_.Exception.GetBaseException() -is [OperationCanceledException] -or
            $_.Exception.GetBaseException() -is [Management.Automation.PipelineStoppedException]) { $interrupted = $true }
        if ($null -eq $returncode -and $null -ne $process) {
            try { if ($process.HasExited) { $returncode = [int]$process.ExitCode } } catch { }
        }
    } finally {
        # Exactly this object, exactly once. The pinned primitive owns streams.
        # The inherited-writer/Dispose gap remains a native qualification limit.
        if ($null -ne $process) {
            try { $process.Dispose() } catch { $errors.Add('InvalidVerificationProcess: owned Dispose failed: ' + $_.Exception.Message) }
        }
        $errorText = $null
        if ($errors.Count -gt 0) { $errorText = $errors -join ' | ' }
        $trace = @{ format = 'artist-helper-command-v1'; role = $role; program = $programPath
            command_line = $CommandLine; cwd = $context.Root; returncode = $returncode
            timed_out = $timedOut; interrupted = $interrupted; error = $errorText }
        if ($null -ne $info) { $trace.command_line = $info.Arguments }
        try {
            Write-VerificationPrivateJson ([IO.Path]::Combine($logDirectory, 'command.json')) $trace
            $context.Traces.Add($trace)
        } catch { $errors.Add('InvalidVerificationLog: private trace publication failed: ' + $_.Exception.Message) }
    }
    if ($errors.Count -gt 0) { throw ($errors -join ' | ') }
    return [int]$returncode
}

function Get-VerificationExporterSource {
    return @'
param([Parameter(Mandatory = $true)][string]$OutputFile)
function Write-VerificationMsvcExport([Collections.IDictionary]$Snapshot, [string]$OutputFile) {
    $names = @('PATH', 'SystemRoot', 'WINDIR', 'SystemDrive', 'COMSPEC', 'PATHEXT', 'TEMP', 'TMP',
        'USERPROFILE', 'APPDATA', 'LOCALAPPDATA', 'HOMEDRIVE', 'HOMEPATH', 'ProgramFiles', 'ProgramFiles(x86)',
        'INCLUDE', 'LIB', 'LIBPATH', 'VSINSTALLDIR', 'VCINSTALLDIR', 'VCToolsInstallDir', 'VCToolsVersion',
        'VSCMD_ARG_TGT_ARCH', 'VSCMD_ARG_HOST_ARCH', 'VSCMD_VER', 'DevEnvDir', 'WindowsSdkDir',
        'WindowsSDKVersion', 'WindowsSDKLibVersion', 'UniversalCRTSdkDir', 'UCRTVersion',
        'CONAN_HOME', 'CMAKE_BUILD_PARALLEL_LEVEL', 'OS', 'PROCESSOR_ARCHITECTURE', 'PROCESSOR_ARCHITEW6432')
    if (-not [IO.Path]::IsPathRooted($OutputFile) -or $null -eq $Snapshot) { throw 'InvalidVerificationEnvironment: absolute export target and snapshot required.' }
    $values = @{}
    foreach ($name in $names) {
        if ($Snapshot.Contains($name)) {
            $value = $Snapshot[$name]
            if ($value -isnot [string] -or $value.IndexOf([char]0) -ge 0) { throw 'InvalidVerificationEnvironment: export values must be strings.' }
            $values[$name] = $value
        }
    }
    $text = ConvertTo-Json -InputObject $values -Depth 3 -Compress
    $bytes = [Text.Encoding]::UTF8.GetBytes($text)
    $file = [IO.File]::Open($OutputFile, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
    $failure = $null
    try { $file.Write($bytes, 0, $bytes.Length) } catch { $failure = $_.Exception.Message }
    finally {
        try { $file.Dispose() } catch {
            if ($null -eq $failure) { $failure = $_.Exception.Message } else { $failure += ' | close: ' + $_.Exception.Message }
        }
    }
    if ($null -ne $failure) { throw $failure }
}
Write-VerificationMsvcExport -Snapshot ([Environment]::GetEnvironmentVariables([EnvironmentVariableTarget]::Process)) -OutputFile $OutputFile
'@
}

function Get-VerificationToolsetDirectory([string]$Installation) {
    $installationPath = Get-VerificationAbsolutePath $Installation
    if ($installationPath.IndexOfAny([char[]]@('"', '%', '!', '&', '|', '<', '>', '^', [char]13, [char]10)) -ge 0) {
        throw 'InvalidMsvcEnvironment: installation path cannot be transported as a fixed cmd path.'
    }
    $directory = [IO.Path]::Combine($installationPath, 'VC', 'Tools', 'MSVC')
    $batch = [IO.Path]::Combine([string[]]@($installationPath, 'VC', 'Auxiliary', 'Build', 'vcvarsall.bat'))
    if (-not [IO.Directory]::Exists($directory) -or -not [IO.File]::Exists($batch)) { throw 'InvalidMsvcEnvironment: selected Visual Studio is incomplete.' }
    $selected = $null; $selectedVersion = [version]'0.0'
    foreach ($candidate in [IO.Directory]::GetDirectories($directory, '14.44.*')) {
        $version = $null
        if ([version]::TryParse([IO.Path]::GetFileName($candidate), [ref]$version) -and $version -gt $selectedVersion -and
            [IO.File]::Exists([IO.Path]::Combine([string[]]@($candidate, 'bin', 'Hostx64', 'x64', 'cl.exe')))) {
            $selected = $candidate; $selectedVersion = $version
        }
    }
    if ($null -eq $selected) { throw 'InvalidMsvcEnvironment: MSVC 14.44 Hostx64/x64 is required.' }
    return $selected
}

function Read-VerificationMsvcEnvironmentLeaf {
    $context = Get-VerificationContext
    $installation = $context.RequestedVisualStudio
    if ($context.Phase -ceq 'build') {
        if ($context.Prepared -isnot [hashtable]) { throw 'InvalidVerificationPreparedTree: no prepared tool choices.' }
        if ($installation.Length -gt 0) {
            $buildDirectories = @{
                requested = (Get-VerificationAbsolutePath $installation)
                prepared = (Get-VerificationAbsolutePath $context.Prepared.visual_studio_installation)
            }
            # Apply the same reviewed directory identity to both sides BEFORE
            # the build guard, including preservation of drive/UNC roots.
            foreach ($choice in @('requested', 'prepared')) {
                $directory = $buildDirectories[$choice]
                $directoryRoot = [IO.Path]::GetPathRoot($directory)
                $directory = $directory.TrimEnd([char[]]@('\', '/'))
                if ($directory.Length -lt $directoryRoot.Length) { $directory = $directoryRoot }
                $buildDirectories[$choice] = $directory
            }
            if ($buildDirectories['requested'] -ine $buildDirectories['prepared']) {
                throw 'InvalidMsvcEnvironment: explicit Visual Studio differs from preparation.'
            }
        }
        $installation = $context.Prepared.visual_studio_installation
    }
    if ([string]::IsNullOrWhiteSpace($installation)) {
        if (-not $context.WorkingEnvironment.ContainsKey('ProgramFiles(x86)')) { throw 'InvalidMsvcEnvironment: Visual Studio Installer location is absent.' }
        $context.Vswhere = [IO.Path]::Combine($context.WorkingEnvironment['ProgramFiles(x86)'], 'Microsoft Visual Studio', 'Installer', 'vswhere.exe')
        Set-VerificationCommandRole 'vswhere'
        $code = Invoke-VerificationChild $context.Vswhere @('-utf8', '-sort', '-products', '*', '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', '-property', 'installationPath')
        if ($code -ne 0) { throw ('InvalidMsvcEnvironment: vswhere failed with raw code ' + $code) }
        $outputPath = [IO.Path]::Combine($context.LogDirectory, 'vswhere', 'stdout.bin')
        if ((New-Object IO.FileInfo($outputPath)).Length -gt 1048576) { throw 'InvalidMsvcEnvironment: discovery output exceeds parsing bounds; raw log retained.' }
        $output = [IO.File]::ReadAllLines($outputPath, [Text.Encoding]::UTF8)
        foreach ($line in $output) {
            if ([string]::IsNullOrWhiteSpace($line)) { continue }
            try { [void](Get-VerificationToolsetDirectory $line.Trim()); $installation = $line.Trim(); break }
            catch { }
        }
        if ([string]::IsNullOrWhiteSpace($installation)) { throw 'InvalidMsvcEnvironment: no discovered 14.44/x64 installation.' }
    }
    $installation = Get-VerificationAbsolutePath $installation
    # Directory identity ignores final separators, but must preserve drive/UNC roots.
    $installationRoot = [IO.Path]::GetPathRoot($installation)
    $installation = $installation.TrimEnd([char[]]@('\', '/'))
    if ($installation.Length -lt $installationRoot.Length) { $installation = $installationRoot }
    $toolset = Get-VerificationToolsetDirectory $installation
    $context.SelectedVisualStudio = $installation
    $context.Tools.cl = [IO.Path]::Combine([string[]]@($toolset, 'bin', 'Hostx64', 'x64', 'cl.exe'))
    $context.Cmd = Resolve-VerificationExecutable ([IO.Path]::Combine($context.WorkingEnvironment['SystemRoot'], 'System32', 'cmd.exe')) $context.WorkingEnvironment
    $transport = [IO.Path]::Combine($context.LogDirectory, '_transport')
    New-VerificationOwnedDirectory $transport
    $exportPath = [IO.Path]::Combine($transport, 'export-msvc.ps1')
    $batchPath = [IO.Path]::Combine($transport, 'load-msvc.cmd')
    $environmentPath = [IO.Path]::Combine($transport, 'msvc-env.json')
    $owned = New-Object 'Collections.Generic.List[string]'
    $firstError = $null; $result = $null
    try {
        $exportBytes = [Text.Encoding]::UTF8.GetBytes((Get-VerificationExporterSource))
        $file = [IO.File]::Open($exportPath, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
        $owned.Add($exportPath)
        $failure = $null
        try { $file.Write($exportBytes, 0, $exportBytes.Length) } catch { $failure = $_.Exception.Message }
        finally {
            try { $file.Dispose() } catch {
                if ($null -eq $failure) { $failure = $_.Exception.Message } else { $failure += ' | close: ' + $_.Exception.Message }
            }
        }
        if ($null -ne $failure) { throw $failure }
        foreach ($path in @($batchPath, $environmentPath, $exportPath, $context.HostExecutable)) {
            if ($path.IndexOfAny([char[]]@('"', '%', '!', '&', '|', '<', '>', '^', [char]13, [char]10)) -ge 0) { throw 'InvalidMsvcEnvironment: fixed cmd transport path is unsafe.' }
        }
        $vcvars = [IO.Path]::Combine([string[]]@($installation, 'VC', 'Auxiliary', 'Build', 'vcvarsall.bat'))
        $lines = @('@echo off', ('call "' + $vcvars + '" x64 -vcvars_ver=14.44 >nul'),
            'if errorlevel 1 exit /b %errorlevel%',
            ('"' + $context.HostExecutable + '" -NoProfile -NonInteractive -ExecutionPolicy Bypass -File "' + $exportPath + '" -OutputFile "' + $environmentPath + '"'))
        $encoding = [Text.Encoding]::GetEncoding([Text.Encoding]::Default.CodePage, [Text.EncoderFallback]::ExceptionFallback, [Text.DecoderFallback]::ExceptionFallback)
        $batchBytes = $encoding.GetBytes(($lines -join "`r`n"))
        $file = [IO.File]::Open($batchPath, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
        $owned.Add($batchPath)
        $failure = $null
        try { $file.Write($batchBytes, 0, $batchBytes.Length) } catch { $failure = $_.Exception.Message }
        finally {
            try { $file.Dispose() } catch {
                if ($null -eq $failure) { $failure = $_.Exception.Message } else { $failure += ' | close: ' + $_.Exception.Message }
            }
        }
        if ($null -ne $failure) { throw $failure }
        Set-VerificationCommandRole 'msvc-env'
        $code = Invoke-VerificationProcessLeaf $context.Cmd ('/d /s /c ""' + $batchPath + '""')
        if ($code -ne 0) { throw ('InvalidMsvcEnvironment: vcvars/export failed with raw code ' + $code) }
        $result = Read-VerificationFlatStrings $environmentPath (Get-VerificationEnvironmentNames) $false
        $owned.Add($environmentPath)
        $observedInstallation = $null
        if ($result.ContainsKey('VSINSTALLDIR')) {
            $observedInstallation = Get-VerificationAbsolutePath $result.VSINSTALLDIR
            $observedRoot = [IO.Path]::GetPathRoot($observedInstallation)
            $observedInstallation = $observedInstallation.TrimEnd([char[]]@('\', '/'))
            if ($observedInstallation.Length -lt $observedRoot.Length) { $observedInstallation = $observedRoot }
        }
        if (-not $result.ContainsKey('VSINSTALLDIR') -or
            $observedInstallation -ine $installation -or
            $result['VCToolsVersion'] -notmatch '^14\.44\.[0-9]+\\?\z' -or $result['VSCMD_ARG_TGT_ARCH'] -cne 'x64' -or
            $result['VSCMD_ARG_HOST_ARCH'] -cne 'x64' -or
            $result['WindowsSDKVersion'] -notmatch '^10\.0\.[0-9]+\.[0-9]+\\?\z') {
            throw 'InvalidMsvcEnvironment: selected installation/toolset/SDK metadata differs.'
        }
        $result['VSINSTALLDIR'] = $installation
        $result['VCToolsVersion'] = $result['VCToolsVersion'].TrimEnd('\')
        $result['WindowsSDKVersion'] = $result['WindowsSDKVersion'].TrimEnd('\')
        $observedCl = [IO.Path]::Combine([string[]]@($installation, 'VC', 'Tools', 'MSVC', $result['VCToolsVersion'], 'bin', 'Hostx64', 'x64', 'cl.exe'))
        if (-not [IO.File]::Exists($observedCl)) { throw 'InvalidMsvcEnvironment: observed x64 compiler is absent.' }
        $context.Tools.cl = $observedCl
        if ($context.Phase -ceq 'build' -and
            ($result['VCToolsVersion'] -cne $context.Prepared.msvc_version -or
             $result['VSCMD_ARG_TGT_ARCH'] -cne $context.Prepared.msvc_architecture -or
             $result['WindowsSDKVersion'] -cne $context.Prepared.windows_sdk_version -or
             $observedCl -ine (Get-VerificationAbsolutePath $context.Prepared.cl))) {
            throw 'InvalidMsvcEnvironment: observed build choices drifted; no repair.'
        }
        $context.Msvc = $result
    } catch { $firstError = $_.Exception.Message }
    finally {
        foreach ($path in $owned) {
            try { Assert-VerificationRegularPath $path $false; [IO.File]::Delete($path) }
            catch {
                if ($null -eq $firstError) { $firstError = 'InvalidVerificationLog: owned transport cleanup failed.' }
                else { $firstError += ' | InvalidVerificationLog: owned transport cleanup failed.' }
            }
        }
    }
    if ($null -ne $firstError) { throw $firstError }
    return $result
}

function Get-VerificationWitnessNames {
    return @('format', 'configuration', 'build_directory', 'visual_studio_installation', 'msvc_version',
        'msvc_architecture', 'windows_sdk_version', 'python', 'conan', 'ninja', 'cmake', 'cl')
}

function Assert-VerificationPreparedTree([string]$BuildDirectory, [string]$Configuration) {
    # Bundle and targets are real bound/validated entry values, not ambient tool
    # observations. No context, host snapshot, clock or process exists here.
    $tree = Get-VerificationAbsolutePath $BuildDirectory
    $bundle = Get-VerificationAbsolutePath $VerificationBundleRoot
    $witness = Read-VerificationFlatStrings ([IO.Path]::Combine($bundle, 'logs', 'windows-helper-prepare', 'toolchain.json')) (Get-VerificationWitnessNames) $true
    if ($witness.format -cne 'artist-helper-toolchain-v1' -or $witness.configuration -cne $Configuration -or
        (Get-VerificationAbsolutePath $witness.build_directory) -ine $tree -or $witness.msvc_architecture -cne 'x64' -or
        $witness.msvc_version -notmatch '^14\.44\.[0-9]+\z' -or $witness.windows_sdk_version -notmatch '^10\.0\.[0-9]+\.[0-9]+\z') {
        throw 'InvalidVerificationPreparedTree: witness differs from configuration/tree.'
    }
    $sourceRoot = Split-Path $PSScriptRoot -Parent
    $scripts = [IO.Path]::Combine([string[]]@($sourceRoot, 'out', 'tooling', 'build', 'Scripts'))
    foreach ($name in @('python', 'conan', 'ninja')) {
        if ((Get-VerificationAbsolutePath $witness[$name]) -ine [IO.Path]::Combine($scripts, ($name + '.exe'))) {
            throw 'InvalidVerificationPreparedTree: tool path is not the prepared venv.'
        }
    }
    $visualStudio = Get-VerificationAbsolutePath $witness.visual_studio_installation
    $compiler = [IO.Path]::Combine([string[]]@($visualStudio, 'VC', 'Tools', 'MSVC', $witness.msvc_version, 'bin', 'Hostx64', 'x64', 'cl.exe'))
    if ((Get-VerificationAbsolutePath $witness.cl) -ine $compiler -or [IO.Path]::GetFileName($witness.cmake) -ine 'cmake.exe') {
        throw 'InvalidVerificationPreparedTree: compiler/CMake choice is incoherent.'
    }
    foreach ($name in @('python', 'conan', 'ninja', 'cmake', 'cl')) {
        if (-not [IO.File]::Exists((Get-VerificationAbsolutePath $witness[$name]))) { throw 'InvalidVerificationPreparedTree: declared tool path is absent.' }
    }
    foreach ($name in @('CMakeCache.txt', 'conan_toolchain.cmake', 'build.ninja')) { Assert-VerificationRegularPath ([IO.Path]::Combine($tree, $name)) $false }
    $cachePath = [IO.Path]::Combine($tree, 'CMakeCache.txt')
    if ((New-Object IO.FileInfo($cachePath)).Length -gt 1048576) { throw 'InvalidVerificationPreparedTree: cache exceeds bounds.' }
    $required = @{
        'CMAKE_HOME_DIRECTORY:INTERNAL' = $sourceRoot
        'CMAKE_GENERATOR:INTERNAL' = 'Ninja'; 'CMAKE_BUILD_TYPE:STRING' = $Configuration
        'CMAKE_TOOLCHAIN_FILE:FILEPATH' = [IO.Path]::Combine($tree, 'conan_toolchain.cmake')
        'CMAKE_COMMAND:INTERNAL' = $witness.cmake; 'CMAKE_MAKE_PROGRAM:FILEPATH' = $witness.ninja
        'CMAKE_CXX_COMPILER:FILEPATH' = $witness.cl; 'ARTIST_BUILD_VERIFICATION:BOOL' = 'ON'; 'BUILD_TESTING:BOOL' = 'OFF'
    }
    if ($VerificationTargets -ccontains 'ArtistLib_tests' -or $VerificationTargets -ccontains 'ArtistLib_component_tests') {
        $required['BUILD_TESTING:BOOL'] = 'ON'
        $required['CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE:STRING'] = 'PRE_TEST'
    }
    $seen = @{}
    foreach ($line in [IO.File]::ReadAllLines($cachePath)) {
        if ($line.Length -eq 0 -or $line.StartsWith('#') -or $line.StartsWith('//')) { continue }
        $separator = $line.IndexOf('=')
        if ($separator -lt 1) { continue }
        $key = $line.Substring(0, $separator)
        $bare = $key.Split(':')[0]
        foreach ($expectedKey in $required.PSBase.Keys) {
            if ($bare -ceq $expectedKey.Split(':')[0]) {
                if ($key -cne $expectedKey -or $seen.ContainsKey($key)) { throw 'InvalidVerificationPreparedTree: duplicate/mistyped required cache key.' }
                $value = $line.Substring($separator + 1)
                if ($bare -cin @('CMAKE_HOME_DIRECTORY', 'CMAKE_TOOLCHAIN_FILE', 'CMAKE_COMMAND', 'CMAKE_MAKE_PROGRAM', 'CMAKE_CXX_COMPILER')) {
                    if ((Get-VerificationAbsolutePath $value) -ine (Get-VerificationAbsolutePath $required[$key])) { throw 'InvalidVerificationPreparedTree: cache path differs.' }
                } elseif ($value -cne $required[$key]) { throw 'InvalidVerificationPreparedTree: cache option differs.' }
                $seen[$key] = $true
            }
        }
    }
    if ($seen.Count -ne $required.Count) { throw 'InvalidVerificationPreparedTree: required cache entry missing.' }
    return $witness
}

function Write-VerificationToolchainWitness {
    $context = Get-VerificationContext
    if ($context.Phase -cne 'prepare' -or $context.Msvc -isnot [hashtable]) { throw 'InvalidVerificationContext: no successful preparation metadata.' }
    foreach ($role in @('msvc-env', 'conan-install', 'cmake-configure')) {
        if (-not $context.UsedRoles.ContainsKey($role)) { throw 'InvalidVerificationLog: required preparation role is absent.' }
    }
    foreach ($trace in $context.Traces) {
        if ($trace.returncode -isnot [int] -or $trace.returncode -ne 0 -or $trace.timed_out -or $trace.interrupted -or $null -ne $trace.error) {
            throw 'InvalidVerificationLog: unsuccessful preparation cannot publish a witness.'
        }
    }
    $witness = @{ format = 'artist-helper-toolchain-v1'; configuration = $context.Configuration
        build_directory = $context.BuildDirectory; visual_studio_installation = $context.SelectedVisualStudio
        msvc_version = $context.Msvc['VCToolsVersion']; msvc_architecture = $context.Msvc['VSCMD_ARG_TGT_ARCH']
        windows_sdk_version = $context.Msvc['WindowsSDKVersion']; python = $context.Tools.python
        conan = $context.Tools.conan; ninja = $context.Tools.ninja; cmake = $context.Tools.cmake; cl = $context.Tools.cl }
    Write-VerificationPrivateJson ([IO.Path]::Combine($context.LogDirectory, 'toolchain.json')) $witness
}
# END ARTIST_VERIFICATION_BINDING

$root = Split-Path $PSScriptRoot -Parent
$tooling = Join-Path $root 'out/tooling'
$venvPython = Join-Path $tooling 'build/Scripts/python.exe'

# Admit every verification flag, even orphan flags, before observations/effects.
$verificationRequested = $PSBoundParameters.ContainsKey('VerificationPhase')
foreach ($name in @('AllowDependencyResolution', 'VerificationInputFile', 'VerificationBundleRoot', 'VerificationLogDirectory', 'VerificationTimeoutSeconds')) {
    if ($PSBoundParameters.ContainsKey($name)) { $verificationRequested = $true }
}
if ($verificationRequested) {
    # A non-advanced script leaves unbound flags/positionals in $args, not
    # PSBoundParameters. Reject them without disclosing arbitrary input values.
    if ($args.Count -ne 0) { throw 'InvalidVerificationFlags: UnknownFlag or unexpected argument.' }
    $verificationFlags = @{} + $PSBoundParameters
    $verificationFlags['Configuration'] = $Configuration
    $verificationFlags['Jobs'] = $Jobs
    $verificationFlags['VerificationTimeoutSeconds'] = $VerificationTimeoutSeconds
    $admittedPhase = Resolve-VerificationPhaseFlags $verificationFlags
    if ($admittedPhase -ceq 'run') { throw 'InvalidVerificationFlags: run is not a helper phase; use the direct adapter.' }
    $bundlePath = Get-VerificationAbsolutePath $VerificationBundleRoot
    $phaseLog = Get-VerificationAbsolutePath $VerificationLogDirectory
    if ((Get-VerificationAbsolutePath $VerificationInputFile) -ine [IO.Path]::Combine($bundlePath, 'raw', 'windows-helper-input.json')) {
        throw 'InvalidVerificationPayload: input differs from the owned bundle transport.'
    }
    if ($phaseLog -ine [IO.Path]::Combine($bundlePath, 'logs', ('windows-helper-' + $admittedPhase))) {
        throw 'InvalidVerificationInput: phase log directory is outside the selected bundle/phase.'
    }
    Assert-VerificationRegularPath $phaseLog $true
    $verificationValues = Read-VerificationPayloadFile $VerificationInputFile $VerificationBundleRoot
    $verificationValues = Resolve-VerificationInputs $verificationValues $root $admittedPhase $Configuration
    $VerificationBuildDirectory = $verificationValues.build_directory
    $VerificationTargets = $verificationValues.targets
    $preparedWitness = $null
    if ($admittedPhase -ceq 'build') { $preparedWitness = Assert-VerificationPreparedTree $VerificationBuildDirectory $Configuration }
}

# BEGIN ARTIST_VERIFICATION_DISPATCH
if ($verificationRequested) {
    $admission = @{ root = $root; phase = $admittedPhase; configuration = $Configuration
        bundle_root = $VerificationBundleRoot; build_directory = $VerificationBuildDirectory
        log_directory = $VerificationLogDirectory; jobs = $Jobs; timeout_milliseconds = ($VerificationTimeoutSeconds * 1000)
        python = $Python; visual_studio_installation = $VisualStudioPath; prepared_witness = $preparedWitness }
    Enter-VerificationContext $admission
    $originalEnvironment = $null
    $verificationEnvironmentAttempted = $false
    try {
        $originalEnvironment = Get-VerificationProcessEnvironment
        if ($admittedPhase -ceq 'prepare') { $tools = Initialize-VerificationTooling $root $Python }
        else { $tools = Get-VerificationTooling $root }
        $msvcEnvironment = Select-VerificationEnvironment (Get-VerificationMsvcEnvironment)
        $VisualStudioPath = $msvcEnvironment['VSINSTALLDIR']
        # Mark BEFORE the call: a partially applied map still needs restoration.
        $verificationEnvironmentAttempted = $true
        Set-VerificationProcessEnvironment $msvcEnvironment

        if ($admittedPhase -ceq 'prepare') {
            $profile = $root + '/conan/profiles/windows-msvc'
            $arguments = @('install', '.', '-pr:h', $profile, '-pr:b', $profile,
                '-s:h', "build_type=$Configuration", '-s:b', "build_type=$Configuration",
                '-c:h', "tools.microsoft.msbuild:installation_path=$VisualStudioPath",
                '-c:b', "tools.microsoft.msbuild:installation_path=$VisualStudioPath",
                '-c:h', 'tools.cmake.cmaketoolchain:user_presets=',
                '-c:h', "tools.build:jobs=$Jobs", '-of', $VerificationBuildDirectory,
                '--build=missing')
            Set-VerificationCommandRole 'conan-install'
            $childCode = Invoke-VerificationChild $tools.conan $arguments
            if ($childCode -ne 0) { exit $childCode }

            $preset = 'windows-msvc-debug'
            if ($Configuration -eq 'Release') { $preset = 'windows-msvc-release' }
            $arguments = @('--fresh', '--preset', $preset, '-B', $VerificationBuildDirectory,
                '--toolchain', ($VerificationBuildDirectory + '/conan_toolchain.cmake'),
                '-DARTIST_BUILD_VERIFICATION=ON')
            if ($VerificationTargets -ccontains 'ArtistLib_tests' -or
                $VerificationTargets -ccontains 'ArtistLib_component_tests') {
                $arguments += @('-DBUILD_TESTING=ON', '-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE:STRING=PRE_TEST')
            }
            else { $arguments += '-DBUILD_TESTING=OFF' }
            Set-VerificationCommandRole 'cmake-configure'
            $childCode = Invoke-VerificationChild $tools.cmake $arguments
            if ($childCode -eq 0) { Write-VerificationToolchainWitness }
        }
        else {
            $arguments = @('--build', $VerificationBuildDirectory, '--target') +
                $VerificationTargets + @('--parallel', "$Jobs")
            Set-VerificationCommandRole 'cmake-build'
            $childCode = Invoke-VerificationChild $tools.cmake $arguments
        }
        exit $childCode
    }
    finally {
        try { if ($verificationEnvironmentAttempted -and $null -ne $originalEnvironment) { Set-VerificationProcessEnvironment $originalEnvironment } }
        finally { Exit-VerificationContext }
    }
}
# END ARTIST_VERIFICATION_DISPATCH
if ($AddressSanitizer -and $Configuration -ne 'Debug') {
    throw 'AddressSanitizer currently supports Debug only; use its separate build tree.'
}
if ($AsanPreflightOnly -and (-not $AddressSanitizer -or $ConfigureOnly)) {
    throw 'AsanPreflightOnly requires AddressSanitizer and cannot be combined with ConfigureOnly.'
}
$preset = 'windows-msvc-' + $Configuration.ToLowerInvariant()
if ($AddressSanitizer) { $preset += '-asan' }
$buildDir = Join-Path $root "out/build/$preset"
$savedEnvironment = @{}
# Q1: the ASan policy of every instrumented child (GoogleTest discovery, the
# canary itself, CTest and its test processes) must not depend on the parent
# session. The module lives next to this script.
. (Join-Path $PSScriptRoot 'asan-env.ps1')
# H-RESTORE (R-I0/V-I0, P3): with -AddressSanitizer, Set-AsanTestEnvironment
# rewrites ASAN_OPTIONS and removes ASAN_ACTIVATION_OPTIONS in this caller
# process. The vcvars dump only replays variables the caller already had, so
# an initially absent ASAN_OPTIONS stayed canonical here after the wrapper
# returned or threw in the same PowerShell (I0 evidence
# asan-caller-restoration). Record the entry state, absence included, so the
# finally restores both variables exactly on every return and exception.
$savedAsanEnvironment = @{}
if ($AddressSanitizer) {
    foreach ($asanVariable in 'ASAN_OPTIONS', 'ASAN_ACTIVATION_OPTIONS') {
        $savedAsanEnvironment[$asanVariable] = [Environment]::GetEnvironmentVariable($asanVariable, 'Process')
    }
}

function Invoke-Checked([string]$Program, [string[]]$Arguments) {
    & $Program @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$Program failed with exit code $LASTEXITCODE" }
}

function Set-BuildEnvironment([string]$Name, [string]$Value) {
    if (-not $savedEnvironment.ContainsKey($Name)) {
        $savedEnvironment[$Name] = [Environment]::GetEnvironmentVariable($Name, 'Process')
    }
    [Environment]::SetEnvironmentVariable($Name, $Value, 'Process')
}

Push-Location $root
try {
    if (-not (Test-Path $venvPython)) {
        Invoke-Checked $Python @('-m', 'venv', (Join-Path $tooling 'build'))
    }
    if (-not (Test-Path (Join-Path $tooling 'build/Scripts/pip.exe'))) {
        Invoke-Checked $venvPython @('-m', 'ensurepip', '--upgrade')
    }
    Invoke-Checked $venvPython @('-m', 'pip', 'install', '--disable-pip-version-check', '-r', 'scripts/build-requirements.txt')

    if (-not $VisualStudioPath) {
        $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
        if (-not (Test-Path $vswhere)) { throw 'Visual Studio Installer / vswhere is required.' }
        $installations = & $vswhere -sort -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        foreach ($installation in $installations) {
            if (Test-Path (Join-Path $installation 'VC/Tools/MSVC/14.44.*')) {
                $VisualStudioPath = $installation
                break
            }
        }
    }
    if (-not $VisualStudioPath -or -not (Test-Path (Join-Path $VisualStudioPath 'VC/Tools/MSVC/14.44.*'))) {
        throw 'Install the MSVC v143 14.44 x64/x86 toolset and Windows SDK in Visual Studio Installer.'
    }

    $vcvars = Join-Path $VisualStudioPath 'VC/Auxiliary/Build/vcvarsall.bat'
    $envScript = Join-Path $tooling 'load-msvc.cmd'
    @('@echo off', "call `"$vcvars`" x64 -vcvars_ver=14.44 >nul", 'if errorlevel 1 exit /b %errorlevel%', 'set') |
        Set-Content -LiteralPath $envScript -Encoding Default
    $environmentLines = & $env:ComSpec /d /c "`"$envScript`""
    if ($LASTEXITCODE -ne 0) { throw 'vcvarsall failed.' }
    foreach ($line in $environmentLines) {
        if ($line -match '^([^=]+)=(.*)$') { Set-BuildEnvironment $Matches[1] $Matches[2] }
    }
    Set-BuildEnvironment 'PATH' ((Join-Path $tooling 'build/Scripts') + ';' + $env:PATH)
    Set-BuildEnvironment 'CONAN_HOME' (Join-Path $root 'out/conan2')
    Set-BuildEnvironment 'CMAKE_BUILD_PARALLEL_LEVEL' "$Jobs"

    Invoke-Checked 'cmake' @('--version')
    Invoke-Checked 'conan' @('--version')
    Invoke-Checked 'ninja' @('--version')
    Write-Output "MSVC $env:VCToolsVersion | Windows SDK $env:WindowsSDKVersion"

    if ($AddressSanitizer) {
        # Q1: fix the instrumented runtime policy for this whole session before
        # any child runs (build-time GoogleTest discovery included); the canary
        # re-applies the same values for its own probe runs.
        Set-AsanTestEnvironment
    }

    $profile = Join-Path $root 'conan/profiles/windows-msvc'
    Invoke-Checked 'conan' @('install', '.', '-pr:h', $profile, '-pr:b', $profile,
        '-s:h', "build_type=$Configuration", '-s:b', "build_type=$Configuration",
        '-c:h', "tools.microsoft.msbuild:installation_path=$VisualStudioPath",
        '-c:b', "tools.microsoft.msbuild:installation_path=$VisualStudioPath",
        # Versioned project presets already select each toolchain. Conan's auto-
        # include would introduce duplicate conan-debug presets for OFF/ON trees.
        '-c:h', 'tools.cmake.cmaketoolchain:user_presets=',
        '-c:h', "tools.build:jobs=$Jobs", '-of', $buildDir, '--build=missing')
    Invoke-Checked 'cmake' @('--fresh', '--preset', $preset)
    if ($ConfigureOnly) { return }
    if ($AddressSanitizer) {
        Invoke-Checked 'cmake' @('--build', '--preset', $preset, '--target', 'artist_asan_probe', '--parallel', "$Jobs")
        $preflightLogDir = Join-Path $buildDir ('asan-preflight/' + (Get-Date -Format 'yyyyMMdd-HHmmssfff'))
        Invoke-Checked $venvPython @('scripts/verify_asan.py', (Join-Path $buildDir 'artist_asan_probe.exe'),
            '--log-dir', $preflightLogDir)
        if ($AsanPreflightOnly) { return }
    }
    Invoke-Checked 'cmake' @('--build', '--preset', $preset, '--parallel', "$Jobs")
    $testArguments = @('--preset', $preset, '--output-junit', (Join-Path $buildDir 'ctest.xml'))
    if ($TestLabel -ne 'all') { $testArguments += @('-L', $TestLabel) }
    Invoke-Checked 'ctest' $testArguments
} finally {
    foreach ($name in $savedAsanEnvironment.Keys) {
        [Environment]::SetEnvironmentVariable($name, $savedAsanEnvironment[$name], 'Process')
    }
    foreach ($name in $savedEnvironment.Keys) {
        [Environment]::SetEnvironmentVariable($name, $savedEnvironment[$name], 'Process')
    }
    Pop-Location
}
