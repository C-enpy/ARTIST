# ASan harness environment normalization (Q1, known-incidents).
#
# The canary (verify_asan.py) sets its own ASAN_OPTIONS for its child probe
# runs. Nothing else controlled the arbitrary environment inherited by every
# other instrumented process the harness launches: GoogleTest discovery, the
# CTest worker, and each test process. A permissive parent (ASAN_OPTIONS with
# exitcode=0, halt_on_error=0, or a hidden log_path report) could mask an
# instrumented failure inside a silently passing campaign: the probe exit 86
# was proven to collapse to 0 under such an inherited policy (see
# docs/_meta/2026-10-16-asan-harness-hardening evidence).
#
# Applying this module while -AddressSanitizer is active resets the runtime
# policy to the exact canary values for the whole ASan session of the harness.
# Non-ASan runs are left untouched: they never load an instrumented runtime.

$script:CanonicalAsanOptions = 'halt_on_error=1:abort_on_error=0:exitcode=86:detect_leaks=0'

function Get-CanonicalAsanOptions
{
    return $script:CanonicalAsanOptions
}

function Set-AsanTestEnvironment
{
    # Resets ASAN_OPTIONS to the canary policy in the current process
    # environment (inherited by every child the harness starts afterwards:
    # build-time GoogleTest discovery, the canary itself, ctest and its test
    # processes). Removes ASAN_ACTIVATION_OPTIONS, the deferred-injection
    # channel. Emits one journal line per decision so the run log carries the
    # inherited -> reset transition without any session parsing.
    $inherited = [Environment]::GetEnvironmentVariable('ASAN_OPTIONS', 'Process')
    [Environment]::SetEnvironmentVariable('ASAN_OPTIONS', $script:CanonicalAsanOptions, 'Process')
    if ($null -eq $inherited) {
        Write-Output "ASan harness env: no inherited ASAN_OPTIONS; set canonical '$script:CanonicalAsanOptions'"
    }
    elseif ($inherited -eq $script:CanonicalAsanOptions) {
        Write-Output "ASan harness env: ASAN_OPTIONS already canonical"
    }
    else {
        Write-Output "ASan harness env: ASAN_OPTIONS inherited '$inherited' reset to canonical '$script:CanonicalAsanOptions'"
    }

    $activation = [Environment]::GetEnvironmentVariable('ASAN_ACTIVATION_OPTIONS', 'Process')
    if ($null -ne $activation) {
        [Environment]::SetEnvironmentVariable('ASAN_ACTIVATION_OPTIONS', $null, 'Process')
        Write-Output "ASan harness env: ASAN_ACTIVATION_OPTIONS '$activation' removed (secondary injection channel)"
    }
}
