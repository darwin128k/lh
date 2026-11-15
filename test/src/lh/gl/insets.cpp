#include <gtest/gtest.h>

#include <lh/gl/insets.h>
#include <lh/test/out.h>
#include <lh/gl/insets/initializer.h>

TEST(ui_insets, sides_and_axis_ends)
{
    lh_gl_insets_t in;

    lh_gl_insets_init(lh_addr_of(in), 1, 2, 3, 4);
    EXPECT_EQ(lh_gl_insets_get_start(lh_addr_of(in), lh_gl_axis_horizontal), lh_gl_scalar(1));
    EXPECT_EQ(lh_gl_insets_get_start(lh_addr_of(in), lh_gl_axis_vertical), lh_gl_scalar(2));
    EXPECT_EQ(lh_gl_insets_get_end(lh_addr_of(in), lh_gl_axis_horizontal), lh_gl_scalar(3));
    EXPECT_EQ(lh_gl_insets_get_end(lh_addr_of(in), lh_gl_axis_vertical), lh_gl_scalar(4));
    EXPECT_EQ(lh_gl_insets_is_zero(lh_addr_of(in)), lh_bool_false);
    lh_gl_insets_init_all(lh_addr_of(in), lh_gl_scalar(0));
    EXPECT_EQ(lh_gl_insets_is_zero(lh_addr_of(in)), lh_bool_true);
}

TEST(ui_insets, shrink_moves_each_side_in_and_stops_at_zero_size)
{
    lh_gl_insets_t in;

    lh_gl_insets_init(lh_addr_of(in), 1, 2, 3, 4);
    lh_gl_rect_t rect;
    lh_gl_rect_init(lh_addr_of(rect), 10, 20, 30, 40);
    lh_gl_rect_t inner = lh_test::out(lh_gl_insets_shrink, lh_addr_of(in), lh_addr_of(rect));
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(inner))), lh_gl_scalar(11));
    EXPECT_EQ(lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(inner))), lh_gl_scalar(22));
    EXPECT_EQ(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(inner))), lh_gl_scalar(26));
    EXPECT_EQ(lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(inner))), lh_gl_scalar(34));

    lh_gl_insets_init_all(lh_addr_of(in), lh_gl_scalar(50));
    inner = lh_test::out(lh_gl_insets_shrink, lh_addr_of(in), lh_addr_of(rect));
    EXPECT_EQ(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(inner))), lh_gl_scalar(0));
}

TEST(ui_insets_initializer, fills_left_top_right_bottom)
{
    const lh_gl_insets_t in = lh_gl_insets_initializer(1, 2, 3, 4);

    EXPECT_EQ(in.left, lh_gl_scalar(1));
    EXPECT_EQ(in.top, lh_gl_scalar(2));
    EXPECT_EQ(in.right, lh_gl_scalar(3));
    EXPECT_EQ(in.bottom, lh_gl_scalar(4));
}
