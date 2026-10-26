#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace artist::qa::close_probe {

// Intentionally declaration-only for the first, compile-contract RED tranche.
constexpr bool can_reap_process(DWORD wait_result) noexcept;

} // namespace artist::qa::close_probe
