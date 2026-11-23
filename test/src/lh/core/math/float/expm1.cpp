#include <gtest/gtest.h>

#include <lh/core/math/float/expm1.h>

#include <cmath>

namespace
{

TEST(math_float_expm1, zero_is_zero)
{
    EXPECT_EQ(lh_math_float_expm1(0.0f), 0.0f);
}

TEST(math_float_expm1, matches_the_library_over_the_whole_range)
{
    for (float x : {-8.0f, -3.5f, -1.0f, -0.125f, 0.125f, 1.0f, 3.5f, 8.0f})
    {
        const float expected = std::expm1(x);

        EXPECT_NEAR(lh_math_float_expm1(x), expected, std::fabs(expected) * 2e-6f + 1e-30f) << x;
    }
}

TEST(math_float_expm1, keeps_the_digits_of_a_small_argument)
{
    for (float x : {1e-6f, 1e-4f, 1e-3f, 0.01f, 0.1f, -1e-4f, -0.1f})
    {
        const float expected = std::expm1(x);

        /* The library rounds exp(x) before subtracting one, so the small
           arguments are where the two answers differ: this brick keeps the
           digits of x. */
        EXPECT_NEAR(lh_math_float_expm1(x), expected, std::fabs(expected) * 2e-6f + 1e-12f) << x;
    }
}

TEST(math_float_expm1, a_large_argument_does_not_fit_in_a_float)
{
    EXPECT_TRUE(std::isinf(lh_math_float_expm1(100.0f)));
}

TEST(math_float_expm1, a_small_argument_becomes_minus_one)
{
    EXPECT_EQ(lh_math_float_expm1(-200.0f), -1.0f);
}

TEST(math_float_expm1, a_nan_stays_a_nan)
{
    EXPECT_TRUE(std::isnan(lh_math_float_expm1(std::nanf(""))));
}

} // namespace
