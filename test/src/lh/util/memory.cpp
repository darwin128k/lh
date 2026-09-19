#include <gtest/gtest.h>

#include <lh/util/memory.h>

TEST(util_memory_size_rest, in_range_subtracts)
{
    EXPECT_EQ(lh_memory_size_rest(10u, 3u), 7u);
    EXPECT_EQ(lh_memory_size_rest(10u, 0u), 10u);
    EXPECT_EQ(lh_memory_size_rest(10u, 10u), 0u);
}

TEST(util_memory_size_rest, past_end_is_zero)
{
    EXPECT_EQ(lh_memory_size_rest(3u, 10u), 0u);
}

TEST(util_memory_scan_bound, remaining_address_space)
{
    char x = 0;
    EXPECT_EQ(lh_memory_scan_bound(&x), lh_addr_diff(LH_USIZE_T_MAX, lh_ptr_to_uaddr(&x)));
}
