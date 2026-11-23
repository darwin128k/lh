#include <gtest/gtest.h>

#include <lh/core/math/float/log.h>

#include <cmath>
#include <limits>

namespace
{

TEST(math_float_log, one_is_zero)
{
    EXPECT_EQ(lh_math_float_log(1.0f), 0.0f);
}

TEST(math_float_log, matches_the_library_over_the_whole_range)
{
    for (float x : {1e-30f, 1e-6f, 0.001f, 0.5f, 1.0f, 2.0f, 10.0f, 1000.0f, 1e30f})
    {
        const float expected = std::log(x);

        EXPECT_NEAR(lh_math_float_log(x), expected, std::fabs(expected) * 2e-6f + 1e-7f) << x;
    }
}

TEST(math_float_log, a_subnormal_number_has_its_log_too)
{
    const float smallest = std::numeric_limits<float>::denorm_min();

    EXPECT_NEAR(lh_math_float_log(smallest), std::log(smallest), 1e-3f);
}

TEST(math_float_log, zero_gives_minus_an_infinity)
{
    EXPECT_TRUE(std::isinf(lh_math_float_log(0.0f)));
    EXPECT_LT(lh_math_float_log(0.0f), 0.0f);
}

TEST(math_float_log, a_negative_number_gives_a_nan)
{
    EXPECT_TRUE(std::isnan(lh_math_float_log(-1.0f)));
}

TEST(math_float_log, an_infinity_is_its_own_logarithm)
{
    EXPECT_TRUE(std::isinf(lh_math_float_log(std::numeric_limits<float>::infinity())));
    EXPECT_GT(lh_math_float_log(std::numeric_limits<float>::infinity()), 0.0f);
}

TEST(math_float_log, a_nan_stays_a_nan)
{
    EXPECT_TRUE(std::isnan(lh_math_float_log(std::nanf(""))));
}

} // namespace
