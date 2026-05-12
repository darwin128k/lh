#include <gtest/gtest.h>
#include <lh/util/math.h>

/* ── add_over_max_exclusive ─────────────────────────────────────────────── */

TEST(math_add_over_max_exclusive, unsigned_no_overflow)
{
    EXPECT_FALSE(lh_math_add_over_max_exclusive(2u, 1u, 5u)); // 3 < 5
    EXPECT_FALSE(lh_math_add_over_max_exclusive(4u, 1u, 5u)); // 5 == 5, exclusive: ok
    EXPECT_FALSE(lh_math_add_over_max_exclusive(0u, 0u, 5u)); // b=0: guard false
}

TEST(math_add_over_max_exclusive, unsigned_overflow)
{
    EXPECT_TRUE(lh_math_add_over_max_exclusive(5u, 1u, 5u)); // 6 > 5
    EXPECT_TRUE(lh_math_add_over_max_exclusive(3u, 3u, 5u)); // 6 > 5
}

TEST(math_add_over_max_exclusive, unsigned_b_exceeds_max)
{
    EXPECT_TRUE(lh_math_add_over_max_exclusive(0u, 3u, 2u)); // b > max: 3 > 2
    EXPECT_TRUE(lh_math_add_over_max_exclusive(1u, 3u, 2u)); // b > max: 4 > 2
    EXPECT_TRUE(lh_math_add_over_max_exclusive(0u, 1u, 0u)); // b > max=0: 1 > 0
}

TEST(math_add_over_max_exclusive, signed_negative_range_no_overflow)
{
    EXPECT_FALSE(lh_math_add_over_max_exclusive(-7, 1, -2)); // -6 < -2
    EXPECT_FALSE(lh_math_add_over_max_exclusive(-3, 1, -2)); // -2 == -2, exclusive: ok
}

TEST(math_add_over_max_exclusive, signed_negative_range_overflow)
{
    EXPECT_TRUE(lh_math_add_over_max_exclusive(-3, 2, -2)); // -1 > -2
    EXPECT_TRUE(lh_math_add_over_max_exclusive(-2, 1, -2)); // -1 > -2
}

/* ── add_over_max_inclusive ─────────────────────────────────────────────── */

TEST(math_add_over_max_inclusive, unsigned_no_overflow)
{
    EXPECT_FALSE(lh_math_add_over_max_inclusive(2u, 1u, 5u)); // 3 < 5
    EXPECT_FALSE(lh_math_add_over_max_inclusive(3u, 1u, 5u)); // 4 < 5
}

TEST(math_add_over_max_inclusive, unsigned_at_boundary)
{
    EXPECT_TRUE(lh_math_add_over_max_inclusive(4u, 1u, 5u)); // 5 >= 5, inclusive: overflow
    EXPECT_TRUE(lh_math_add_over_max_inclusive(5u, 0u, 5u)); // b=0: a >= max
}

TEST(math_add_over_max_inclusive, unsigned_overflow)
{
    EXPECT_TRUE(lh_math_add_over_max_inclusive(5u, 1u, 5u)); // 6 >= 5
}

TEST(math_add_over_max_inclusive, unsigned_b_exceeds_max)
{
    EXPECT_TRUE(lh_math_add_over_max_inclusive(0u, 3u, 2u)); // b > max: 3 >= 2
    EXPECT_TRUE(lh_math_add_over_max_inclusive(1u, 3u, 2u)); // b > max: 4 >= 2
    EXPECT_TRUE(lh_math_add_over_max_inclusive(0u, 1u, 0u)); // b > max=0: 1 >= 0
}

TEST(math_add_over_max_inclusive, signed_negative_range_no_overflow)
{
    EXPECT_FALSE(lh_math_add_over_max_inclusive(-7, 1, -2)); // -6 < -2
    EXPECT_FALSE(lh_math_add_over_max_inclusive(-4, 1, -2)); // -3 < -2
}

TEST(math_add_over_max_inclusive, signed_negative_range_overflow)
{
    EXPECT_TRUE(lh_math_add_over_max_inclusive(-3, 1, -2)); // -2 >= -2, inclusive
    EXPECT_TRUE(lh_math_add_over_max_inclusive(-3, 2, -2)); // -1 >= -2
}

/* ── add_below_min_exclusive ────────────────────────────────────────────── */

TEST(math_add_below_min_exclusive, signed_no_underflow)
{
    EXPECT_FALSE(lh_math_add_below_min_exclusive(-7, -2, -10)); // -9 > -10
    EXPECT_FALSE(lh_math_add_below_min_exclusive(-9, -1, -10)); // -10 == -10, exclusive: ok
    EXPECT_FALSE(lh_math_add_below_min_exclusive(-7, 1, -10));  // b>0: guard false
}

TEST(math_add_below_min_exclusive, signed_underflow)
{
    EXPECT_TRUE(lh_math_add_below_min_exclusive(-9, -2, -10));  // -11 < -10
    EXPECT_TRUE(lh_math_add_below_min_exclusive(-10, -1, -10)); // -11 < -10
}

TEST(math_add_below_min_exclusive, signed_positive_range)
{
    EXPECT_FALSE(lh_math_add_below_min_exclusive(5, -2, 3)); // 3 == 3, exclusive: ok
    EXPECT_TRUE(lh_math_add_below_min_exclusive(5, -3, 3));  // 2 < 3
}

/* ── add_below_min_inclusive ────────────────────────────────────────────── */

TEST(math_add_below_min_inclusive, signed_no_underflow)
{
    EXPECT_FALSE(lh_math_add_below_min_inclusive(-7, -2, -10)); // -9 > -10
    EXPECT_FALSE(lh_math_add_below_min_inclusive(-7, 1, -10));  // b>0: guard false
}

TEST(math_add_below_min_inclusive, signed_at_boundary)
{
    EXPECT_TRUE(lh_math_add_below_min_inclusive(-9, -1, -10));  // -10 <= -10, inclusive
    EXPECT_TRUE(lh_math_add_below_min_inclusive(-10, 0, -10));  // b=0: a <= min
}

TEST(math_add_below_min_inclusive, signed_underflow)
{
    EXPECT_TRUE(lh_math_add_below_min_inclusive(-9, -2, -10));  // -11 < -10
    EXPECT_TRUE(lh_math_add_below_min_inclusive(-10, -1, -10)); // -11 < -10
}

TEST(math_add_below_min_inclusive, signed_positive_range)
{
    EXPECT_FALSE(lh_math_add_below_min_inclusive(5, -1, 3)); // 4 > 3
    EXPECT_TRUE(lh_math_add_below_min_inclusive(5, -2, 3));  // 3 <= 3, inclusive
    EXPECT_TRUE(lh_math_add_below_min_inclusive(5, -3, 3));  // 2 < 3
}
