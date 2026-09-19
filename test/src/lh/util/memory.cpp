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

TEST(util_memory_bytes_eq, equal_prefix)
{
    const lh_uchar_t a[] = {1, 2, 3};
    const lh_uchar_t b[] = {1, 2, 9};
    EXPECT_TRUE(lh_memory_bytes_eq(a, b, 2));
    EXPECT_FALSE(lh_memory_bytes_eq(a, b, 3));
}

TEST(util_memory_bytes_eq, zero_length)
{
    const lh_uchar_t a[] = {1};
    EXPECT_TRUE(lh_memory_bytes_eq(a, a, 0));
}

TEST(util_memory_bytes_any_eq, finds_value_in_block)
{
    const lh_uchar_t a[] = {1, 2, 3, 4};
    EXPECT_TRUE(lh_memory_bytes_any_eq(a, 4, 3));
    EXPECT_FALSE(lh_memory_bytes_any_eq(a, 4, 9));
    EXPECT_FALSE(lh_memory_bytes_any_eq(a, 0, 1));
}

TEST(util_memory_bytes_any_eq_step, matches_on_grid_only)
{
    const lh_uchar_t hay[] = {'x', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    EXPECT_FALSE(lh_memory_bytes_any_eq_step(hay, 4, needle, 2, 2));
    EXPECT_TRUE(lh_memory_bytes_any_eq_step(hay + 1, 2, needle, 2, 2));
}

TEST(util_memory_bytes_any_ne, detects_mismatch)
{
    const lh_uchar_t a[] = {1, 2, 3};
    const lh_uchar_t b[] = {1, 2, 9};
    EXPECT_FALSE(lh_memory_bytes_any_ne(a, a, 3));
    EXPECT_TRUE(lh_memory_bytes_any_ne(a, b, 3));
    EXPECT_FALSE(lh_memory_bytes_any_ne(a, b, 2));
    EXPECT_FALSE(lh_memory_bytes_any_ne(a, b, 0));
}
