/**
 * @file point.cpp
 * @brief Tests for `lh/gx/point.h`.
 *
 * The gx point is the math point under the name of this part. What is checked
 * here is that the name reaches the plane gx draws on: `x` to the right, `y`
 * down, and a value the caller owns.
 */

#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gx/point.h>
#include <lh/gx/scalar.h>
#include <lh/test/out.h>

namespace
{

TEST(gx_point, x_is_to_the_right_and_y_is_down)
{
    lh_gx_point_t point;

    lh_gx_point_init(lh_addr_of(point), lh_gx_scalar(120), lh_gx_scalar(45));

    EXPECT_EQ(lh_gx_point_get_x(lh_addr_of(point)), lh_gx_scalar(120));
    EXPECT_EQ(lh_gx_point_get_y(lh_addr_of(point)), lh_gx_scalar(45));
}

TEST(gx_point, a_point_is_a_value_the_caller_owns)
{
    lh_gx_point_t first;

    lh_gx_point_init(lh_addr_of(first), lh_gx_scalar(3), lh_gx_scalar(4));

    lh_gx_point_t second = first;

    lh_gx_point_set_x(lh_addr_of(first), lh_gx_scalar(9));

    EXPECT_EQ(lh_gx_point_get_x(lh_addr_of(second)), lh_gx_scalar(3));
    EXPECT_EQ(lh_gx_point_eq(lh_addr_of(first), lh_addr_of(second)), lh_bool_false);
}

TEST(gx_point, the_empty_point_is_the_origin)
{
    lh_gx_point_t point;

    lh_gx_point_init_empty(lh_addr_of(point));

    EXPECT_EQ(lh_gx_point_get_x(lh_addr_of(point)), lh_gx_scalar(0));
    EXPECT_EQ(lh_gx_point_get_y(lh_addr_of(point)), lh_gx_scalar(0));
}

} // namespace
