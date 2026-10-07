#include <gtest/gtest.h>

#include <lh/bool.h>
#include <lh/ui/axis.h>
#include <lh/ui/insets.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>

TEST(ui_insets, sides_and_axis_ends)
{
    lh_ui_insets_t in;

    lh_ui_insets_init(lh_addr_of(in), 1, 2, 3, 4);
    EXPECT_EQ(lh_ui_insets_get_start(lh_addr_of(in), lh_ui_axis_horizontal), lh_ui_scalar(1));
    EXPECT_EQ(lh_ui_insets_get_start(lh_addr_of(in), lh_ui_axis_vertical), lh_ui_scalar(2));
    EXPECT_EQ(lh_ui_insets_get_end(lh_addr_of(in), lh_ui_axis_horizontal), lh_ui_scalar(3));
    EXPECT_EQ(lh_ui_insets_get_end(lh_addr_of(in), lh_ui_axis_vertical), lh_ui_scalar(4));
    EXPECT_EQ(lh_ui_insets_is_zero(lh_addr_of(in)), lh_bool_false);
    lh_ui_insets_init_all(lh_addr_of(in), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_insets_is_zero(lh_addr_of(in)), lh_bool_true);
}

TEST(ui_insets, shrink_moves_each_side_in_and_stops_at_zero_size)
{
    lh_ui_insets_t in;
    lh_ui_rect_t rect;

    lh_ui_insets_init(lh_addr_of(in), 1, 2, 3, 4);
    lh_ui_rect_init(lh_addr_of(rect), 10, 20, 30, 40);
    lh_ui_rect_t inner = lh_ui_insets_shrink(lh_addr_of(in), lh_addr_of(rect));
    EXPECT_EQ(lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(inner))), lh_ui_scalar(11));
    EXPECT_EQ(lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(inner))), lh_ui_scalar(22));
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(inner))), lh_ui_scalar(26));
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(inner))), lh_ui_scalar(34));

    lh_ui_insets_init_all(lh_addr_of(in), lh_ui_scalar(50));
    inner = lh_ui_insets_shrink(lh_addr_of(in), lh_addr_of(rect));
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(inner))), lh_ui_scalar(0));
}
