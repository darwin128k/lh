#include <gtest/gtest.h>

#include <lh/math/float/round.h>

namespace
{

TEST(math_float_round, floor_to_int)
{
    EXPECT_EQ(lh_math_float_floor_to_int(0.0f), 0);
    EXPECT_EQ(lh_math_float_floor_to_int(-0.0f), 0);
    EXPECT_EQ(lh_math_float_floor_to_int(2.0f), 2);
    EXPECT_EQ(lh_math_float_floor_to_int(2.7f), 2);
    EXPECT_EQ(lh_math_float_floor_to_int(0.5f), 0);
    EXPECT_EQ(lh_math_float_floor_to_int(-0.5f), -1);
    EXPECT_EQ(lh_math_float_floor_to_int(-2.0f), -2);
    EXPECT_EQ(lh_math_float_floor_to_int(-2.3f), -3);
}

TEST(math_float_round, ceil_to_int)
{
    EXPECT_EQ(lh_math_float_ceil_to_int(0.0f), 0);
    EXPECT_EQ(lh_math_float_ceil_to_int(-0.0f), 0);
    EXPECT_EQ(lh_math_float_ceil_to_int(2.0f), 2);
    EXPECT_EQ(lh_math_float_ceil_to_int(2.1f), 3);
    EXPECT_EQ(lh_math_float_ceil_to_int(0.5f), 1);
    EXPECT_EQ(lh_math_float_ceil_to_int(-0.5f), 0);
    EXPECT_EQ(lh_math_float_ceil_to_int(-2.0f), -2);
    EXPECT_EQ(lh_math_float_ceil_to_int(-2.7f), -2);
}

TEST(math_float_round, floor_and_ceil_bracket_every_value)
{
    for (int i = -1000; i <= 1000; ++i)
    {
        const float x = static_cast<float>(i) * 0.37f;
        const int lo = lh_math_float_floor_to_int(x);
        const int hi = lh_math_float_ceil_to_int(x);

        EXPECT_LE(static_cast<float>(lo), x) << x;
        EXPECT_GE(static_cast<float>(hi), x) << x;
        EXPECT_LE(hi - lo, 1) << x;
        EXPECT_EQ(hi == lo, static_cast<float>(lo) == x) << x;
    }
}

} // namespace
