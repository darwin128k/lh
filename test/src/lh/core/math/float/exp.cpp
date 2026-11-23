#include <gtest/gtest.h>

#include <lh/core/math/float/exp.h>

#include <cmath>

namespace
{

TEST(math_float_exp, zero_is_one)
{
    EXPECT_EQ(lh_math_float_exp(0.0f), 1.0f);
}

TEST(math_float_exp, matches_the_library_over_the_whole_range)
{
    for (float x : {-30.0f, -8.0f, -3.5f, -1.0f, -0.125f, 0.125f, 1.0f, 3.5f, 8.0f, 30.0f})
    {
        const float expected = std::exp(x);

        EXPECT_NEAR(lh_math_float_exp(x), expected, std::fabs(expected) * 2e-6f + 1e-30f) << x;
    }
}

TEST(math_float_exp, keeps_the_digits_of_a_small_argument)
{
    for (float x : {1e-6f, 1e-4f, 1e-3f, 0.01f, 0.1f})
    {
        const float expected = std::exp(x);

        EXPECT_NEAR(lh_math_float_exp(x), expected, std::fabs(expected) * 2e-6f) << x;
    }
}

TEST(math_float_exp, a_large_argument_does_not_fit_in_a_float)
{
    EXPECT_TRUE(std::isinf(lh_math_float_exp(100.0f)));
    EXPECT_FALSE(std::isinf(lh_math_float_exp(88.0f)));
}

TEST(math_float_exp, a_small_argument_becomes_zero)
{
    EXPECT_EQ(lh_math_float_exp(-200.0f), 0.0f);
    EXPECT_FALSE(lh_math_float_exp(-90.0f) == 0.0f);
}

TEST(math_float_exp, a_nan_stays_a_nan)
{
    EXPECT_TRUE(std::isnan(lh_math_float_exp(std::nanf(""))));
}

} // namespace
