#include <gtest/gtest.h>

#include <lh/core/math/float/pow.h>

#include <cmath>

namespace
{

TEST(math_float_pow, one_to_any_power_is_one)
{
    EXPECT_EQ(lh_math_float_pow(1.0f, 7.0f), 1.0f);
    EXPECT_EQ(lh_math_float_pow(1.0f, -7.0f), 1.0f);
    EXPECT_EQ(lh_math_float_pow(1.0f, 0.5f), 1.0f);
}

TEST(math_float_pow, any_number_to_the_zero_is_one)
{
    EXPECT_EQ(lh_math_float_pow(3.0f, 0.0f), 1.0f);
    EXPECT_EQ(lh_math_float_pow(-3.0f, 0.0f), 1.0f);
    EXPECT_EQ(lh_math_float_pow(0.0f, 0.0f), 1.0f);
}

TEST(math_float_pow, a_whole_exponent_keeps_the_digits_of_the_base)
{
    EXPECT_EQ(lh_math_float_pow(2.0f, 10.0f), 1024.0f);
    EXPECT_EQ(lh_math_float_pow(3.0f, 3.0f), 27.0f);
    EXPECT_EQ(lh_math_float_pow(0.5f, 2.0f), 0.25f);
    EXPECT_EQ(lh_math_float_pow(-2.0f, 3.0f), -8.0f);
    EXPECT_EQ(lh_math_float_pow(-2.0f, 4.0f), 16.0f);
}

TEST(math_float_pow, matches_the_library_over_the_whole_range)
{
    for (float x : {0.5f, 2.0f, 10.0f, 0.75f, 123.5f})
    {
        for (float y : {-3.0f, -0.5f, 0.25f, 2.0f, 5.0f})
        {
            const float expected = std::pow(x, y);

            EXPECT_NEAR(lh_math_float_pow(x, y), expected,
                        std::fabs(expected) * 1e-5f + 1e-7f)
                << x << " " << y;
        }
    }
}

TEST(math_float_pow, a_negative_base_with_a_fractional_exponent_has_no_answer)
{
    EXPECT_TRUE(std::isnan(lh_math_float_pow(-2.0f, 0.5f)));
    EXPECT_TRUE(std::isnan(lh_math_float_pow(-2.0f, 1.5f)));
}

TEST(math_float_pow, zero_to_a_power)
{
    EXPECT_EQ(lh_math_float_pow(0.0f, 3.0f), 0.0f);
    EXPECT_TRUE(std::isinf(lh_math_float_pow(0.0f, -3.0f)));
}

TEST(math_float_pow, a_nan_stays_a_nan)
{
    EXPECT_TRUE(std::isnan(lh_math_float_pow(std::nanf(""), 2.0f)));
    EXPECT_TRUE(std::isnan(lh_math_float_pow(2.0f, std::nanf(""))));
}

} // namespace
