#include <gtest/gtest.h>

#include <lh/float/sqrt.h>

#include <cmath>

namespace
{

TEST(float_sqrt, exact_squares)
{
    EXPECT_EQ(lh_float_sqrt(1.0f), 1.0f);
    EXPECT_EQ(lh_float_sqrt(4.0f), 2.0f);
    EXPECT_EQ(lh_float_sqrt(144.0f), 12.0f);
    EXPECT_EQ(lh_float_sqrt(0.25f), 0.5f);
}

TEST(float_sqrt, ordinary_values_are_the_correctly_rounded_root)
{
    for (float x : {2.0f, 3.0f, 10.0f, 0.001f, 12345.678f, 1e-30f, 1e30f})
    {
        EXPECT_FLOAT_EQ(lh_float_sqrt(x), std::sqrt(x)) << x;
    }
}

TEST(float_sqrt, zero_keeps_its_sign)
{
    EXPECT_EQ(lh_float_sqrt(0.0f), 0.0f);
    EXPECT_TRUE(std::signbit(lh_float_sqrt(-0.0f)));
}

TEST(float_sqrt, negative_is_nan)
{
    EXPECT_TRUE(std::isnan(lh_float_sqrt(-1.0f)));
    EXPECT_TRUE(std::isnan(lh_float_sqrt(-1e-20f)));
}

} // namespace
