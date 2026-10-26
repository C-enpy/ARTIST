#include "wait_policy.hpp"

static_assert(artist::qa::close_probe::can_reap_process(WAIT_OBJECT_0));
static_assert(!artist::qa::close_probe::can_reap_process(WAIT_TIMEOUT));
static_assert(!artist::qa::close_probe::can_reap_process(WAIT_FAILED));
static_assert(!artist::qa::close_probe::can_reap_process(WAIT_ABANDONED_0));
