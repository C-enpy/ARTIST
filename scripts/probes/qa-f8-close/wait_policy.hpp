#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace artist::qa::close_probe {

// Collection requires an observed process-termination signal.
constexpr bool can_reap_process(DWORD wait_result) noexcept {
    return wait_result == WAIT_OBJECT_0;
}

} // namespace artist::qa::close_probe
