#include <gtest/gtest.h>

#include <lh/core/math/float/tanh.h>

#include <cmath>

namespace
{

TEST(math_float_tanh, zero_is_zero)
{
    EXPECT_EQ(lh_math_float_tanh(0.0f), 0.0f);
}

TEST(math_float_tanh, matches_the_library_over_the_whole_range)
{
    for (float x : {-8.0f, -3.5f, -1.0f, -0.125f, 0.125f, 1.0f, 3.5f, 8.0f})
    {
        const float expected = std::tanh(x);

        EXPECT_NEAR(lh_math_float_tanh(x), expected, std::fabs(expected) * 2e-6f + 1e-7f) << x;
    }
}

TEST(math_float_tanh, keeps_the_digits_of_a_small_argument)
{
    for (float x : {1e-6f, 1e-4f, 1e-3f, 0.01f, 0.1f})
    {
        const float expected = std::tanh(x);

        EXPECT_NEAR(lh_math_float_tanh(x), expected, std::fabs(expected) * 2e-6f) << x;
    }
}

TEST(math_float_tanh, is_odd)
{
    for (float x : {0.25f, 1.0f, 4.0f})
    {
        EXPECT_FLOAT_EQ(lh_math_float_tanh(-x), -lh_math_float_tanh(x)) << x;
    }
}

TEST(math_float_tanh, a_large_argument_is_one)
{
    EXPECT_EQ(lh_math_float_tanh(20.0f), 1.0f);
    EXPECT_EQ(lh_math_float_tanh(-20.0f), -1.0f);
}

TEST(math_float_tanh, a_nan_stays_a_nan)
{
    EXPECT_TRUE(std::isnan(lh_math_float_tanh(std::nanf(""))));
}

} // namespace
