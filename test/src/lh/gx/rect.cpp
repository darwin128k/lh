/**
 * @file rect.cpp
 * @brief Tests for `lh/gx/rect.h`.
 *
 * The gx rectangle is the math rectangle under the name of this part: a
 * top-left corner and an extent, half-open. What is checked here is that the
 * name reaches the plane gx draws on and that the gx parts compose — a gx
 * rectangle gives back a gx point and a gx size.
 */

#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gx/point.h>
#include <lh/gx/rect.h>
#include <lh/gx/scalar.h>
#include <lh/gx/size.h>
#include <lh/test/out.h>

namespace
{

TEST(gx_rect, a_rectangle_is_a_corner_and_an_extent)
{
    lh_gx_rect_t rect;

    lh_gx_rect_init(lh_addr_of(rect), lh_gx_scalar(10), lh_gx_scalar(20), lh_gx_scalar(30),
                    lh_gx_scalar(40));

    EXPECT_EQ(lh_gx_point_get_x(lh_gx_rect_get_origin(lh_addr_of(rect))), lh_gx_scalar(10));
    EXPECT_EQ(lh_gx_point_get_y(lh_gx_rect_get_origin(lh_addr_of(rect))), lh_gx_scalar(20));
    EXPECT_EQ(lh_gx_size_get_width(lh_gx_rect_get_size(lh_addr_of(rect))), lh_gx_scalar(30));
    EXPECT_EQ(lh_gx_size_get_height(lh_gx_rect_get_size(lh_addr_of(rect))), lh_gx_scalar(40));
}

TEST(gx_rect, the_far_corner_is_the_origin_offset_by_the_size)
{
    lh_gx_rect_t rect;

    lh_gx_rect_init(lh_addr_of(rect), lh_gx_scalar(10), lh_gx_scalar(20), lh_gx_scalar(30),
                    lh_gx_scalar(40));
    lh_gx_point_t far;

    lh_gx_rect_far(lh_addr_of(rect), lh_addr_of(far));

    EXPECT_EQ(lh_gx_point_get_x(lh_addr_of(far)), lh_gx_scalar(40));
    EXPECT_EQ(lh_gx_point_get_y(lh_addr_of(far)), lh_gx_scalar(60));
}

TEST(gx_rect, a_rectangle_is_half_open)
{
    lh_gx_rect_t rect;

    lh_gx_rect_init(lh_addr_of(rect), lh_gx_scalar(10), lh_gx_scalar(20), lh_gx_scalar(30),
                    lh_gx_scalar(40));
    lh_gx_point_t inside;

    lh_gx_point_init(lh_addr_of(inside), lh_gx_scalar(39), lh_gx_scalar(59));
    lh_gx_point_t outside;

    lh_gx_point_init(lh_addr_of(outside), lh_gx_scalar(40), lh_gx_scalar(60));

    EXPECT_EQ(lh_gx_rect_contains_point(lh_addr_of(rect), inside), lh_bool_true);
    EXPECT_EQ(lh_gx_rect_contains_point(lh_addr_of(rect), outside), lh_bool_false);
}

TEST(gx_rect, an_empty_rectangle_is_nothing)
{
    lh_gx_rect_t rect;

    lh_gx_rect_init_empty(lh_addr_of(rect));

    EXPECT_EQ(lh_gx_rect_is_empty(lh_addr_of(rect)), lh_bool_true);
}

} // namespace
