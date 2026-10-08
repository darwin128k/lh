#include <gtest/gtest.h>

#include <lh/numeric/types.h>
#include <lh/os/tick.h>
#include <lh/timer/tick.h>

namespace
{

/* A clock that reads zero is not a clock that is fast, and nothing above it can
   tell those two apart: every timer, every deadline and every measurement built
   on this returns zero and reports that it worked. This file shipped exactly
   that to every POSIX build — the real branch tested a macro whose header was
   included inside it, so it was never compiled in. So the first thing to pin is
   that the clock runs at all. */
TEST(os_tick, the_clock_reads_a_time_and_not_a_zero)
{
    const lh_u64_t us = lh_os_tick_us();

    EXPECT_GT(us, 0u);
    EXPECT_GT(lh_os_tick_ms(), 0);
    EXPECT_GE(lh_os_tick_us(), us);
}

TEST(os_tick, the_clock_goes_forward)
{
    const lh_u64_t before = lh_os_tick_us();
    lh_u64_t after = before;

    /* Bounded, so a clock that never moves fails the check instead of hanging the
       run — which is the other half of what a zero clock would look like. */
    for (int i = 0; i < 200000 && after == before; ++i)
    {
        after = lh_os_tick_us();
    }
    EXPECT_GT(after, before);
}

} // namespace