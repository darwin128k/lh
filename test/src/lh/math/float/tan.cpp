#include <gtest/gtest.h>

#include <lh/math/float/sin_cos.h>
#include <lh/math/float/tan.h>

#include <cmath>

namespace
{

TEST(math_float_tan, zero_and_quarter_turn)
{
    EXPECT_EQ(lh_math_float_tan(0.0f), 0.0f);
    EXPECT_NEAR(lh_math_float_tan(0.78539816f), 1.0f, 1e-6f);
    EXPECT_NEAR(lh_math_float_tan(-0.78539816f), -1.0f, 1e-6f);
}

TEST(math_float_tan, is_sine_over_cosine)
{
    for (float x : {0.1f, 0.55f, 1.2f, -2.0f, 3.0f, 10.0f})
    {
        float s;
        float c;
        lh_math_float_sin_cos(x, &s, &c);
        EXPECT_EQ(lh_math_float_tan(x), s / c) << x;
        EXPECT_NEAR(lh_math_float_tan(x), std::tan(x), 1e-5f * (1.0f + std::fabs(std::tan(x))))
            << x;
    }
}

} // namespace
