#include "wait_policy.hpp"

// Check SDK availability without using the new constexpr capability.
static_assert(WAIT_OBJECT_0 == 0x00000000UL);
static_assert(WAIT_TIMEOUT == 0x00000102UL);
static_assert(WAIT_FAILED == 0xFFFFFFFFUL);
static_assert(WAIT_ABANDONED_0 == 0x00000080UL);
