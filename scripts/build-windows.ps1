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
    [switch]$AllowDependencyResolution
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

# Real effect boundaries and their validated inputs are a separate slice.
# Keep the actual helper closed until that wiring is reviewed and qualified.
# The test-owned fixture extracts only the marked dispatch region below.
if ($PSBoundParameters.ContainsKey('VerificationPhase')) {
    throw 'Verification phases are not implemented yet; no bootstrap, build or test was started.'
}

# BEGIN ARTIST_VERIFICATION_DISPATCH
if ($PSBoundParameters.ContainsKey('VerificationPhase')) {
    if ($VerificationPhase -eq 'prepare' -and -not $AllowDependencyResolution) {
        throw 'Verification prepare requires AllowDependencyResolution before any effects.'
    }

    $originalEnvironment = Get-VerificationProcessEnvironment
    try {
        $msvcEnvironment = Get-VerificationMsvcEnvironment
        Set-VerificationProcessEnvironment $msvcEnvironment

        if ($VerificationPhase -eq 'prepare') {
            $profile = $root + '/conan/profiles/windows-msvc'
            $arguments = @('install', '.', '-pr:h', $profile, '-pr:b', $profile,
                '-s:h', "build_type=$Configuration", '-s:b', "build_type=$Configuration",
                '-c:h', "tools.microsoft.msbuild:installation_path=$VisualStudioPath",
                '-c:b', "tools.microsoft.msbuild:installation_path=$VisualStudioPath",
                '-c:h', 'tools.cmake.cmaketoolchain:user_presets=',
                '-c:h', "tools.build:jobs=$Jobs", '-of', $VerificationBuildDirectory,
                '--build=missing')
            $childCode = Invoke-VerificationChild 'conan' $arguments
            if ($childCode -ne 0) { exit $childCode }

            $preset = 'windows-msvc-debug'
            if ($Configuration -eq 'Release') { $preset = 'windows-msvc-release' }
            $arguments = @('--fresh', '--preset', $preset, '-B', $VerificationBuildDirectory,
                '--toolchain', ($VerificationBuildDirectory + '/conan_toolchain.cmake'),
                '-DARTIST_BUILD_VERIFICATION=ON')
            if ($VerificationTargets -ccontains 'ArtistLib_tests' -or
                $VerificationTargets -ccontains 'ArtistLib_component_tests') {
                $arguments += @('-DBUILD_TESTING=ON', '-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE=PRE_TEST')
            }
            else { $arguments += '-DBUILD_TESTING=OFF' }
            $childCode = Invoke-VerificationChild 'cmake' $arguments
        }
        elseif ($VerificationPhase -eq 'build') {
            $arguments = @('--build', $VerificationBuildDirectory, '--target') +
                $VerificationTargets + @('--parallel', "$Jobs")
            $childCode = Invoke-VerificationChild 'cmake' $arguments
        }
        elseif ($VerificationPhase -eq 'run') {
            $childCode = Invoke-VerificationChild $VerificationExecutable $VerificationArguments
        }
        else {
            throw 'Unsupported verification phase; no legacy fallback is permitted.'
        }
        exit $childCode
    }
    finally {
        Set-VerificationProcessEnvironment $originalEnvironment
    }
}
# END ARTIST_VERIFICATION_DISPATCH
if ($AddressSanitizer -and $Configuration -ne 'Debug') {
    throw 'AddressSanitizer currently supports Debug only; use its separate build tree.'
}
if ($AsanPreflightOnly -and (-not $AddressSanitizer -or $ConfigureOnly)) {
    throw 'AsanPreflightOnly requires AddressSanitizer and cannot be combined with ConfigureOnly.'
}
$root = Split-Path $PSScriptRoot -Parent
$tooling = Join-Path $root 'out/tooling'
$venvPython = Join-Path $tooling 'build/Scripts/python.exe'
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
