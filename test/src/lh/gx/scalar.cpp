/**
 * @file scalar.cpp
 * @brief Tests for `lh/gx/scalar.h`.
 *
 * The scalar is the unit of the gx plane: a point's `x` and `y`, a size's
 * `width` and `height`. These tests are about the two numbers that decide
 * where a thing sits — the room left over in a box, and the offset that
 * centres a thing in it — and about the one rule both FPU paths obey: the
 * same pixels.
 */

#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/core/util/addr.h>
#include <lh/gx/scalar.h>
#include <lh/test/out.h>

namespace
{

TEST(gx_scalar, slack_is_the_room_that_is_left)
{
    EXPECT_EQ(lh_gx_scalar_slack(lh_gx_scalar(10), lh_gx_scalar(4)), lh_gx_scalar(6));
    EXPECT_EQ(lh_gx_scalar_slack(lh_gx_scalar(4), lh_gx_scalar(4)), lh_gx_scalar(0));
    EXPECT_EQ(lh_gx_scalar_slack(lh_gx_scalar(4), lh_gx_scalar(10)), lh_gx_scalar(0));
}

TEST(gx_scalar, centre_leaves_the_half_pixel_on_the_far_side)
{
    EXPECT_EQ(lh_gx_scalar_centre(lh_gx_scalar(10), lh_gx_scalar(4)), lh_gx_scalar(3));
    EXPECT_EQ(lh_gx_scalar_centre(lh_gx_scalar(11), lh_gx_scalar(4)), lh_gx_scalar(4));
    EXPECT_EQ(lh_gx_scalar_centre(lh_gx_scalar(3), lh_gx_scalar(10)), lh_gx_scalar(0));
}

TEST(gx_scalar, of_casts_an_expression)
{
    const lh_gx_scalar_t value = 7;

    EXPECT_EQ(lh_gx_scalar_of(value + 1), lh_gx_scalar(8));
}

TEST(gx_scalar, trunc_drops_the_fraction_toward_zero)
{
#if LH_LIBRARY_OPTION_MATH_FPU
    EXPECT_EQ(lh_test::out(lh_gx_scalar_trunc, lh_gx_scalar(3.7)), lh_gx_scalar(3));
    EXPECT_EQ(lh_test::out(lh_gx_scalar_trunc, lh_gx_scalar(-3.7)), lh_gx_scalar(-3));
#else
    EXPECT_EQ(lh_test::out(lh_gx_scalar_trunc, lh_gx_scalar(3)), lh_gx_scalar(3));
    EXPECT_EQ(lh_test::out(lh_gx_scalar_trunc, lh_gx_scalar(-3)), lh_gx_scalar(-3));
#endif
}

TEST(gx_scalar, floor_and_ceil_agree_on_a_whole_number)
{
    EXPECT_EQ(lh_gx_scalar_floor_s32(lh_gx_scalar(7)), 7);
    EXPECT_EQ(lh_gx_scalar_ceil_s32(lh_gx_scalar(7)), 7);
}

} // namespace
