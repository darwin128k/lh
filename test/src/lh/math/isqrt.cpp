#include <gtest/gtest.h>

#include <lh/math/isqrt.h>

namespace
{

TEST(isqrt, a_root_is_the_largest_integer_whose_square_still_fits)
{
    EXPECT_EQ(lh_math_isqrt(0), 0);
    EXPECT_EQ(lh_math_isqrt(1), 1);
    EXPECT_EQ(lh_math_isqrt(3), 1);
    EXPECT_EQ(lh_math_isqrt(4), 2);
    EXPECT_EQ(lh_math_isqrt(8), 2);
    EXPECT_EQ(lh_math_isqrt(9), 3);
    EXPECT_EQ(lh_math_isqrt(255 * 255), 255);
    EXPECT_EQ(lh_math_isqrt(256 * 256), 256);
    EXPECT_EQ(lh_math_isqrt(256 * 256 - 1), 255);
}

TEST(isqrt, nothing_below_zero_has_a_root)
{
    EXPECT_EQ(lh_math_isqrt(-1), 0);
    EXPECT_EQ(lh_math_isqrt(-(1LL << 40)), 0);
}

TEST(isqrt, it_is_exact_for_every_value_a_coverage_or_a_blur_asks_about)
{
    // The float root is what starts it off, so the two steps that close the gap
    // are the whole contract. Sweeping every squared distance a fixed-point
    // rim produces is what proves they are enough.
    for (lh_sllong_t value = 0; value <= 40000; ++value)
    {
        const lh_int_t root = lh_math_isqrt(value);
        ASSERT_LE(static_cast<lh_sllong_t>(root) * root, value) << "at " << value;
        ASSERT_GT((root + 1) * (root + 1), value) << "at " << value;
    }
}

TEST(isqrt, it_is_exact_at_the_magnitudes_a_screen_never_reaches)
{
    // 256ths of a pixel out to a hundred-thousandths of one, squared: the answer
    // must be the exact one, because a pixel on the wrong side of a rim is
    // painted or not painted. Nothing here is past the range of an lh_int_t
    // answer, which is the range the function is documented to answer for.
    const lh_sllong_t scales[] = {256, 4096, 65536, 1048576, 1LL << 20};
    for (const lh_sllong_t scale : scales)
    {
        for (lh_sllong_t n = 0; n < 40; ++n)
        {
            const lh_sllong_t value = (n * n) * scale * scale;
            const lh_int_t root = lh_math_isqrt(value);
            EXPECT_EQ(static_cast<lh_sllong_t>(root) * root <= value, true) << value;
            EXPECT_GT(static_cast<lh_sllong_t>(root + 1) * (root + 1), value) << value;
            EXPECT_EQ(root, static_cast<lh_int_t>(n * scale)) << value;
        }
    }
}

TEST(isqrt, the_top_of_the_range_is_the_largest_long_it_can_answer_for)
{
    // The bound is the square of the largest lh_int_t, because the answer is an
    // lh_int_t: one past it the root no longer fits in what it is returned as.
    const lh_sllong_t top = LH_MATH_ISQRT_MAX;
    const lh_int_t biggest = lh_numeric_limit_smax(lh_int_t);
    EXPECT_EQ(lh_math_isqrt(top), biggest);
    EXPECT_EQ(lh_math_isqrt(top - 1), biggest - 1);
    EXPECT_EQ(lh_math_isqrt(top) - lh_math_isqrt(top - 1), 1);
    // And the bound really is that square, not a rounder number near it.
    EXPECT_EQ(top, static_cast<lh_sllong_t>(biggest) * biggest);
}

} // namespace
