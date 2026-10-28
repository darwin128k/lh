#include <gtest/gtest.h>

#include <lh/math/float/finite.h>

#include <cmath>
#include <limits>

namespace
{

TEST(math_float_is_finite, numbers_are_finite)
{
    for (float x : {0.0f, -0.0f, 1.0f, -1.0f, 1e-45f, std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max(), std::numeric_limits<float>::min()})
    {
        EXPECT_TRUE(lh_math_float_is_finite(x)) << x;
    }
}

TEST(math_float_is_finite, nan_and_infinities_are_not)
{
    EXPECT_FALSE(lh_math_float_is_finite(std::numeric_limits<float>::quiet_NaN()));
    EXPECT_FALSE(lh_math_float_is_finite(std::numeric_limits<float>::infinity()));
    EXPECT_FALSE(lh_math_float_is_finite(-std::numeric_limits<float>::infinity()));
}

} // namespace
