#include <gtest/gtest.h>

#include <lh/core/math/float/bits.h>
#include <lh/core/math/float/finite.h>

namespace
{

TEST(math_float_bits, the_bits_of_a_number_are_its_own)
{
    EXPECT_EQ(lh_math_float_to_bits(1.0f), 0x3F800000U);
    EXPECT_EQ(lh_math_float_to_bits(-2.0f), 0xC0000000U);
    EXPECT_EQ(lh_math_float_to_bits(0.0f), 0x00000000U);
}

TEST(math_float_bits, a_number_comes_back_from_its_bits)
{
    EXPECT_FLOAT_EQ(lh_math_float_from_bits(0x3F800000U), 1.0f);
    EXPECT_FLOAT_EQ(lh_math_float_from_bits(0xC0000000U), -2.0f);
    EXPECT_EQ(lh_math_float_from_bits(0x00000000U), 0.0f);
}

TEST(math_float_bits, the_round_trip_keeps_the_value)
{
    for (float value : {0.5f, 1.0f, 3.14159f, -17.25f, 1e-30f, 1e30f})
    {
        EXPECT_EQ(lh_math_float_from_bits(lh_math_float_to_bits(value)), value) << value;
    }
}

TEST(math_float_bits, the_infinity_and_the_nan_are_what_the_format_says)
{
    EXPECT_FALSE(lh_math_float_is_finite(lh_math_float_from_bits(LH_MATH_FLOAT_INFINITY_BITS)));
    EXPECT_FALSE(lh_math_float_is_finite(lh_math_float_from_bits(LH_MATH_FLOAT_NAN_BITS)));
    EXPECT_EQ(lh_math_float_to_bits(lh_math_float_from_bits(LH_MATH_FLOAT_INFINITY_BITS)),
              LH_MATH_FLOAT_INFINITY_BITS);
    EXPECT_EQ(lh_math_float_to_bits(lh_math_float_from_bits(LH_MATH_FLOAT_NAN_BITS)),
              LH_MATH_FLOAT_NAN_BITS);
}

} // namespace
