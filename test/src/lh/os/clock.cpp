#include <gtest/gtest.h>

#include <lh/os/clock.h>
#include <lh/os/net.h>

namespace
{

TEST(os_clock_ms, is_monotonic_across_two_reads)
{
    lh_u64_t a = lh_os_clock_ms();
    lh_u64_t b = lh_os_clock_ms();
    EXPECT_GE(b, a);
}

} // namespace
