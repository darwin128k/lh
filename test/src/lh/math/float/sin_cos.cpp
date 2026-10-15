#include <gtest/gtest.h>

#include <lh/math/float/sin_cos.h>

#include <cmath>

namespace
{

const float pi = 3.14159265358979f;

void
expect_sin_cos(float x, float tolerance)
{
    float s = 2.0f;
    float c = 2.0f;

    lh_math_float_sin_cos(x, &s, &c);
    EXPECT_NEAR(s, std::sin(static_cast<double>(x)), tolerance) << "sin " << x;
    EXPECT_NEAR(c, std::cos(static_cast<double>(x)), tolerance) << "cos " << x;
}

TEST(math_float_sin_cos, zero)
{
    float s = 2.0f;
    float c = 2.0f;

    lh_math_float_sin_cos(0.0f, &s, &c);
    EXPECT_EQ(s, 0.0f);
    EXPECT_EQ(c, 1.0f);
}

TEST(math_float_sin_cos, quadrant_angles)
{
    expect_sin_cos(pi / 2.0f, 1e-6f);
    expect_sin_cos(pi, 1e-6f);
    expect_sin_cos(3.0f * pi / 2.0f, 1e-6f);
    expect_sin_cos(2.0f * pi, 1e-6f);
    expect_sin_cos(pi / 6.0f, 1e-6f);
    expect_sin_cos(pi / 4.0f, 1e-6f);
}

TEST(math_float_sin_cos, negative_angles_are_odd_and_even)
{
    for (float x : {0.1f, 0.5f, 1.0f, 2.5f, 4.0f, 10.0f})
    {
        float s_pos;
        float c_pos;
        float s_neg;
        float c_neg;

        lh_math_float_sin_cos(x, &s_pos, &c_pos);
        lh_math_float_sin_cos(-x, &s_neg, &c_neg);
        EXPECT_FLOAT_EQ(s_neg, -s_pos) << x;
        EXPECT_FLOAT_EQ(c_neg, c_pos) << x;
        expect_sin_cos(-x, 2e-6f);
    }
}

TEST(math_float_sin_cos, a_sweep_matches_libm_within_a_few_ulp)
{
    for (int i = -2000; i <= 2000; ++i)
    {
        const float x = static_cast<float>(i) * 0.01f;

        expect_sin_cos(x, 2e-6f);
    }
}

TEST(math_float_sin_cos, large_angles_stay_accurate)
{
    expect_sin_cos(1000.0f, 1e-4f);
    expect_sin_cos(-12345.0f, 1e-3f);
}

} // namespace
