#include <gtest/gtest.h>
#include <lh/util/interval.h>

namespace
{

TEST(interval_closed, validity)
{
    EXPECT_TRUE(lh_interval_closed_is_valid(5, 5));
    EXPECT_TRUE(lh_interval_closed_is_valid(4, 5));
    EXPECT_FALSE(lh_interval_closed_is_valid(6, 5));
}

TEST(interval_lopen, validity)
{
    EXPECT_TRUE(lh_interval_lopen_is_valid(4, 5));
    EXPECT_FALSE(lh_interval_lopen_is_valid(5, 5));
    EXPECT_FALSE(lh_interval_lopen_is_valid(6, 5));
}

TEST(interval_ropen, validity)
{
    EXPECT_TRUE(lh_interval_ropen_is_valid(4, 5));
    EXPECT_FALSE(lh_interval_ropen_is_valid(5, 5));
    EXPECT_FALSE(lh_interval_ropen_is_valid(6, 5));
}

TEST(interval_open, validity)
{
    EXPECT_TRUE(lh_interval_open_is_valid(4, 5));
    EXPECT_FALSE(lh_interval_open_is_valid(5, 5));
    EXPECT_FALSE(lh_interval_open_is_valid(6, 5));
}

TEST(interval_closed, contains_value)
{
    EXPECT_TRUE(lh_interval_closed_contains_value(4, 6, 4));
    EXPECT_TRUE(lh_interval_closed_contains_value(4, 6, 5));
    EXPECT_TRUE(lh_interval_closed_contains_value(4, 6, 6));
    EXPECT_FALSE(lh_interval_closed_contains_value(4, 6, 3));
    EXPECT_FALSE(lh_interval_closed_contains_value(4, 6, 7));
}

TEST(interval_lopen, contains_value)
{
    EXPECT_FALSE(lh_interval_lopen_contains_value(4, 6, 4));
    EXPECT_TRUE(lh_interval_lopen_contains_value(4, 6, 5));
    EXPECT_TRUE(lh_interval_lopen_contains_value(4, 6, 6));
    EXPECT_FALSE(lh_interval_lopen_contains_value(4, 6, 3));
    EXPECT_FALSE(lh_interval_lopen_contains_value(4, 6, 7));
}

TEST(interval_ropen, contains_value)
{
    EXPECT_TRUE(lh_interval_ropen_contains_value(4, 6, 4));
    EXPECT_TRUE(lh_interval_ropen_contains_value(4, 6, 5));
    EXPECT_FALSE(lh_interval_ropen_contains_value(4, 6, 6));
    EXPECT_FALSE(lh_interval_ropen_contains_value(4, 6, 3));
    EXPECT_FALSE(lh_interval_ropen_contains_value(4, 6, 7));
}

TEST(interval_open, contains_value)
{
    EXPECT_FALSE(lh_interval_open_contains_value(4, 6, 4));
    EXPECT_TRUE(lh_interval_open_contains_value(4, 6, 5));
    EXPECT_FALSE(lh_interval_open_contains_value(4, 6, 6));
    EXPECT_FALSE(lh_interval_open_contains_value(4, 6, 3));
    EXPECT_FALSE(lh_interval_open_contains_value(4, 6, 7));
}

TEST(interval_closed, contains_range)
{
    EXPECT_TRUE(lh_interval_closed_contains_range(2, 8, 2, 8));
    EXPECT_TRUE(lh_interval_closed_contains_range(2, 8, 3, 7));
    EXPECT_FALSE(lh_interval_closed_contains_range(2, 8, 1, 7));
    EXPECT_FALSE(lh_interval_closed_contains_range(2, 8, 3, 9));
}

TEST(interval_lopen, contains_range)
{
    EXPECT_TRUE(lh_interval_lopen_contains_range(2, 8, 2, 8));
    EXPECT_TRUE(lh_interval_lopen_contains_range(2, 8, 3, 7));
    EXPECT_FALSE(lh_interval_lopen_contains_range(2, 8, 1, 7));
    EXPECT_FALSE(lh_interval_lopen_contains_range(2, 8, 3, 9));
}

TEST(interval_ropen, contains_range)
{
    EXPECT_TRUE(lh_interval_ropen_contains_range(2, 8, 2, 8));
    EXPECT_TRUE(lh_interval_ropen_contains_range(2, 8, 3, 7));
    EXPECT_FALSE(lh_interval_ropen_contains_range(2, 8, 1, 7));
    EXPECT_FALSE(lh_interval_ropen_contains_range(2, 8, 3, 9));
}

TEST(interval_open, contains_range)
{
    EXPECT_TRUE(lh_interval_open_contains_range(2, 8, 2, 8));
    EXPECT_TRUE(lh_interval_open_contains_range(2, 8, 3, 7));
    EXPECT_FALSE(lh_interval_open_contains_range(2, 8, 1, 7));
    EXPECT_FALSE(lh_interval_open_contains_range(2, 8, 3, 9));
}

TEST(interval_closed, overlaps_range)
{
    EXPECT_TRUE(lh_interval_closed_overlaps_range(2, 6, 4, 8));
    EXPECT_TRUE(lh_interval_closed_overlaps_range(2, 6, 6, 9));
    EXPECT_FALSE(lh_interval_closed_overlaps_range(2, 6, 7, 9));
}

TEST(interval_lopen, overlaps_range)
{
    EXPECT_TRUE(lh_interval_lopen_overlaps_range(2, 6, 4, 8));
    EXPECT_FALSE(lh_interval_lopen_overlaps_range(2, 6, 6, 9));
    EXPECT_FALSE(lh_interval_lopen_overlaps_range(2, 6, 7, 9));
}

TEST(interval_ropen, overlaps_range)
{
    EXPECT_TRUE(lh_interval_ropen_overlaps_range(2, 6, 4, 8));
    EXPECT_FALSE(lh_interval_ropen_overlaps_range(2, 6, 6, 9));
    EXPECT_FALSE(lh_interval_ropen_overlaps_range(2, 6, 7, 9));
}

TEST(interval_open, overlaps_range)
{
    EXPECT_TRUE(lh_interval_open_overlaps_range(2, 6, 4, 8));
    EXPECT_FALSE(lh_interval_open_overlaps_range(2, 6, 6, 9));
    EXPECT_FALSE(lh_interval_open_overlaps_range(2, 6, 7, 9));
}

TEST(interval_closed, is_add_overflow)
{
    EXPECT_FALSE(lh_interval_closed_is_add_overflow(5, 4, 0, 10));
    EXPECT_TRUE(lh_interval_closed_is_add_overflow(8, 3, 0, 10));
    EXPECT_TRUE(lh_interval_closed_is_add_overflow(1, -2, 0, 10));
}

TEST(interval_lopen, is_add_overflow)
{
    EXPECT_FALSE(lh_interval_lopen_is_add_overflow(5, 4, 0, 10));
    EXPECT_TRUE(lh_interval_lopen_is_add_overflow(8, 3, 0, 10));
    EXPECT_TRUE(lh_interval_lopen_is_add_overflow(1, -1, 0, 10));
}

TEST(interval_ropen, is_add_overflow)
{
    EXPECT_FALSE(lh_interval_ropen_is_add_overflow(4, 5, 0, 10));
    EXPECT_TRUE(lh_interval_ropen_is_add_overflow(5, 5, 0, 10));
    EXPECT_TRUE(lh_interval_ropen_is_add_overflow(1, -2, 0, 10));
}

TEST(interval_open, is_add_overflow)
{
    EXPECT_FALSE(lh_interval_open_is_add_overflow(4, 5, 0, 10));
    EXPECT_TRUE(lh_interval_open_is_add_overflow(5, 5, 0, 10));
    EXPECT_TRUE(lh_interval_open_is_add_overflow(1, -1, 0, 10));
}

TEST(interval_closed, is_sub_overflow)
{
    EXPECT_FALSE(lh_interval_closed_is_sub_overflow(8, 3, 0, 10));
    EXPECT_TRUE(lh_interval_closed_is_sub_overflow(1, 3, 0, 10));
}

TEST(interval_lopen, is_sub_overflow)
{
    EXPECT_FALSE(lh_interval_lopen_is_sub_overflow(8, 3, 0, 10));
    EXPECT_TRUE(lh_interval_lopen_is_sub_overflow(3, 3, 0, 10));
}

TEST(interval_ropen, is_sub_overflow)
{
    EXPECT_FALSE(lh_interval_ropen_is_sub_overflow(8, 3, 0, 10));
    EXPECT_TRUE(lh_interval_ropen_is_sub_overflow(1, -9, 0, 10));
}

TEST(interval_open, is_sub_overflow)
{
    EXPECT_FALSE(lh_interval_open_is_sub_overflow(8, 3, 0, 10));
    EXPECT_TRUE(lh_interval_open_is_sub_overflow(1, -9, 0, 10));
}

TEST(interval_closed, is_mul_overflow)
{
    EXPECT_FALSE(lh_interval_closed_is_mul_overflow(2, 5, 0, 10));
    EXPECT_TRUE(lh_interval_closed_is_mul_overflow(2, 6, 0, 10));
}

TEST(interval_lopen, is_mul_overflow)
{
    EXPECT_FALSE(lh_interval_lopen_is_mul_overflow(2, 5, 0, 10));
    EXPECT_TRUE(lh_interval_lopen_is_mul_overflow(0, 5, 0, 10));
}

TEST(interval_ropen, is_mul_overflow)
{
    EXPECT_FALSE(lh_interval_ropen_is_mul_overflow(2, 4, 0, 10));
    EXPECT_TRUE(lh_interval_ropen_is_mul_overflow(2, 5, 0, 10));
}

TEST(interval_open, is_mul_overflow)
{
    EXPECT_FALSE(lh_interval_open_is_mul_overflow(2, 4, 0, 10));
    EXPECT_TRUE(lh_interval_open_is_mul_overflow(1, 10, 0, 10));
}

TEST(interval_closed, is_div_overflow)
{
    EXPECT_FALSE(lh_interval_closed_is_div_overflow(10, 2, 0, 10));
    EXPECT_TRUE(lh_interval_closed_is_div_overflow(10, 0, 0, 10));
}

TEST(interval_lopen, is_div_overflow)
{
    EXPECT_FALSE(lh_interval_lopen_is_div_overflow(10, 2, 0, 10));
    EXPECT_TRUE(lh_interval_lopen_is_div_overflow(1, 2, 0, 10));
}

TEST(interval_ropen, is_div_overflow)
{
    EXPECT_FALSE(lh_interval_ropen_is_div_overflow(10, 2, 0, 10));
    EXPECT_TRUE(lh_interval_ropen_is_div_overflow(10, 1, 0, 10));
}

TEST(interval_open, is_div_overflow)
{
    EXPECT_FALSE(lh_interval_open_is_div_overflow(10, 2, 0, 10));
    EXPECT_TRUE(lh_interval_open_is_div_overflow(10, 1, 0, 10));
}

TEST(interval_negative_ranges, contains_and_overlap)
{
    EXPECT_TRUE(lh_interval_closed_contains_value(-10, -2, -6));
    EXPECT_FALSE(lh_interval_open_contains_value(-10, -2, -10));
    EXPECT_FALSE(lh_interval_open_contains_value(-10, -2, -2));
    EXPECT_TRUE(lh_interval_open_overlaps_range(-10, -2, -5, -1));
    EXPECT_FALSE(lh_interval_open_overlaps_range(-10, -2, -2, 3));
}

TEST(interval_negative_ranges, add_overflow_closed)
{
    EXPECT_FALSE(lh_interval_closed_is_add_overflow(-7, 1, -10, -2)); // -6 inside [-10, -2]
    EXPECT_TRUE(lh_interval_closed_is_add_overflow(-3, 2, -10, -2));  // -1 > -2
    EXPECT_TRUE(lh_interval_closed_is_add_overflow(-9, -2, -10, -2)); // -11 < -10
}

TEST(interval_negative_ranges, add_overflow_open)
{
    EXPECT_FALSE(lh_interval_open_is_add_overflow(-7, 1, -10, -2)); // -6 inside (-10, -2)
    EXPECT_TRUE(lh_interval_open_is_add_overflow(-3, 1, -10, -2));  // -2 hits open max
    EXPECT_TRUE(lh_interval_open_is_add_overflow(-9, -1, -10, -2)); // -10 hits open min
}

TEST(interval_negative_ranges, sub_overflow_closed)
{
    EXPECT_FALSE(lh_interval_closed_is_sub_overflow(-5, -2, -10, -2)); // -3 inside
    EXPECT_TRUE(lh_interval_closed_is_sub_overflow(-3, -2, -10, -2));  // -1 > -2
    EXPECT_TRUE(lh_interval_closed_is_sub_overflow(-9, 2, -10, -2));   // -11 < -10
}

TEST(interval_negative_ranges, sub_overflow_open)
{
    EXPECT_FALSE(lh_interval_open_is_sub_overflow(-5, -2, -10, -2)); // -3 inside
    EXPECT_TRUE(lh_interval_open_is_sub_overflow(-4, -2, -10, -2));  // -2 hits open max
    EXPECT_TRUE(lh_interval_open_is_sub_overflow(-9, 1, -10, -2));   // -10 hits open min
}

TEST(interval_negative_ranges, mul_overflow_negative_b)
{
    EXPECT_FALSE(lh_interval_closed_is_mul_overflow(-2, -2, 0, 10)); // 4 inside
    EXPECT_TRUE(lh_interval_closed_is_mul_overflow(-6, -2, 0, 10));  // 12 > 10
    EXPECT_TRUE(
        lh_interval_lopen_is_mul_overflow(1, -1, -10, -1)); // -1 hits closed upper bound for lopen
}

TEST(interval_negative_ranges, mul_overflow_mixed_sign_bounds)
{
    EXPECT_FALSE(lh_interval_closed_is_mul_overflow(-3, 2, -10, 10));  // -6 inside
    EXPECT_FALSE(lh_interval_closed_is_mul_overflow(-3, -2, -10, 10)); // 6 inside
    EXPECT_TRUE(lh_interval_open_is_mul_overflow(5, -2, -10, 10));     // -10 hits open min
    EXPECT_TRUE(lh_interval_open_is_mul_overflow(5, 2, -10, 10));      // 10 hits open max
}

TEST(interval_negative_ranges, div_overflow_negative_values)
{
    EXPECT_FALSE(lh_interval_closed_is_div_overflow(-10, 2, -10, -2));  // -5 inside
    EXPECT_FALSE(lh_interval_closed_is_div_overflow(-10, -2, -10, 10)); // 5 inside
    EXPECT_TRUE(lh_interval_closed_is_div_overflow(-10, 0, -10, 10));   // division by zero
    EXPECT_TRUE(lh_interval_open_is_div_overflow(-4, 2, -10, -2));      // -2 hits open max
}

// Binary search tests
namespace
{

typedef struct
{
    unsigned int from;
    unsigned int to;
} test_kv_t;

// Simple sorted table for testing: indices 0-4 contain data
static const test_kv_t test_table[] = {
    {10U, 100U}, // index 0
    {20U, 200U}, // index 1
    {30U, 300U}, // index 2
    {40U, 400U}, // index 3
    {50U, 500U}, // index 4
};

} // namespace

// Closed interval [0, n] tests
TEST(interval_closed, binary_search_found)
{
    test_kv_t result;
    int found;

    lh_interval_closed_binary_search(unsigned int, test_table, 4, 10U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 10U);
    EXPECT_EQ(result.to, 100U);

    lh_interval_closed_binary_search(unsigned int, test_table, 4, 30U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 30U);
    EXPECT_EQ(result.to, 300U);

    lh_interval_closed_binary_search(unsigned int, test_table, 4, 50U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 50U);
    EXPECT_EQ(result.to, 500U);
}

TEST(interval_closed, binary_search_not_found)
{
    test_kv_t result;
    int found;

    lh_interval_closed_binary_search(unsigned int, test_table, 4, 5U, from, result, found);
    EXPECT_EQ(found, 0);

    lh_interval_closed_binary_search(unsigned int, test_table, 4, 25U, from, result, found);
    EXPECT_EQ(found, 0);

    lh_interval_closed_binary_search(unsigned int, test_table, 4, 60U, from, result, found);
    EXPECT_EQ(found, 0);
}

TEST(interval_closed, binary_search_edge_cases)
{
    test_kv_t result;
    int found;

    // Single element table - found
    lh_interval_closed_binary_search(unsigned int, test_table, 0, 10U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 10U);

    // Single element table - not found
    lh_interval_closed_binary_search(unsigned int, test_table, 0, 20U, from, result, found);
    EXPECT_EQ(found, 0);
}

// Left-open interval (0, n] tests
TEST(interval_lopen, binary_search_found)
{
    test_kv_t result;
    int found;

    // Indices 1-4 are valid in (0, 4]
    lh_interval_lopen_binary_search(unsigned int, test_table, 4, 20U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 20U);
    EXPECT_EQ(result.to, 200U);

    lh_interval_lopen_binary_search(unsigned int, test_table, 4, 30U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 30U);
    EXPECT_EQ(result.to, 300U);

    lh_interval_lopen_binary_search(unsigned int, test_table, 4, 50U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 50U);
    EXPECT_EQ(result.to, 500U);
}

TEST(interval_lopen, binary_search_not_found)
{
    test_kv_t result;
    int found;

    // Index 0 is excluded in (0, 4]
    lh_interval_lopen_binary_search(unsigned int, test_table, 4, 10U, from, result, found);
    EXPECT_EQ(found, 0);

    lh_interval_lopen_binary_search(unsigned int, test_table, 4, 25U, from, result, found);
    EXPECT_EQ(found, 0);

    lh_interval_lopen_binary_search(unsigned int, test_table, 4, 60U, from, result, found);
    EXPECT_EQ(found, 0);
}

// Right-open interval [0, n) tests
TEST(interval_ropen, binary_search_found)
{
    test_kv_t result;
    int found;

    // Indices 0-4 are valid in [0, 5)
    lh_interval_ropen_binary_search(unsigned int, test_table, 5, 10U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 10U);
    EXPECT_EQ(result.to, 100U);

    lh_interval_ropen_binary_search(unsigned int, test_table, 5, 30U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 30U);
    EXPECT_EQ(result.to, 300U);

    lh_interval_ropen_binary_search(unsigned int, test_table, 5, 50U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 50U);
    EXPECT_EQ(result.to, 500U);
}

TEST(interval_ropen, binary_search_not_found)
{
    test_kv_t result;
    int found;

    lh_interval_ropen_binary_search(unsigned int, test_table, 5, 5U, from, result, found);
    EXPECT_EQ(found, 0);

    lh_interval_ropen_binary_search(unsigned int, test_table, 5, 25U, from, result, found);
    EXPECT_EQ(found, 0);

    lh_interval_ropen_binary_search(unsigned int, test_table, 5, 60U, from, result, found);
    EXPECT_EQ(found, 0);
}

TEST(interval_ropen, binary_search_edge_cases)
{
    test_kv_t result;
    int found;

    // Empty table
    lh_interval_ropen_binary_search(unsigned int, test_table, 0, 10U, from, result, found);
    EXPECT_EQ(found, 0);

    // Single element table - found
    lh_interval_ropen_binary_search(unsigned int, test_table, 1, 10U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 10U);

    // Single element table - not found
    lh_interval_ropen_binary_search(unsigned int, test_table, 1, 20U, from, result, found);
    EXPECT_EQ(found, 0);
}

// Open interval (0, n) tests
TEST(interval_open, binary_search_found)
{
    test_kv_t result;
    int found;

    // Indices 1-3 are valid in (0, 4)
    lh_interval_open_binary_search(unsigned int, test_table, 4, 20U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 20U);
    EXPECT_EQ(result.to, 200U);

    lh_interval_open_binary_search(unsigned int, test_table, 4, 30U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 30U);
    EXPECT_EQ(result.to, 300U);

    lh_interval_open_binary_search(unsigned int, test_table, 4, 40U, from, result, found);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(result.from, 40U);
    EXPECT_EQ(result.to, 400U);
}

TEST(interval_open, binary_search_not_found)
{
    test_kv_t result;
    int found;

    // Indices 0 and 4 are excluded in (0, 4)
    lh_interval_open_binary_search(unsigned int, test_table, 4, 10U, from, result, found);
    EXPECT_EQ(found, 0);

    lh_interval_open_binary_search(unsigned int, test_table, 4, 50U, from, result, found);
    EXPECT_EQ(found, 0);

    lh_interval_open_binary_search(unsigned int, test_table, 4, 25U, from, result, found);
    EXPECT_EQ(found, 0);
}

} // namespace
