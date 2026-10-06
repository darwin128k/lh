#include <gtest/gtest.h>

#include <lh/ui/paint.h>
#include <lh/ui/pen.h>
#include <lh/util/addr.h>

TEST(ui_pen, init_starts_empty_with_width_one)
{
    lh_ui_pen_t pen;

    lh_ui_pen_init(lh_addr_of(pen));
    EXPECT_TRUE(lh_ui_paint_is_empty(lh_ui_pen_get_paint(lh_addr_of(pen))));
    EXPECT_EQ(lh_ui_pen_get_width(lh_addr_of(pen)), 1);
}

TEST(ui_pen, set_paint_and_width)
{
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_pen_t pen;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 4);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_pen_init(lh_addr_of(pen));
    lh_ui_pen_set_paint(lh_addr_of(pen), lh_addr_of(paint));
    lh_ui_pen_set_width(lh_addr_of(pen), 3);
    EXPECT_FALSE(lh_ui_paint_is_empty(lh_ui_pen_get_paint(lh_addr_of(pen))));
    EXPECT_EQ(lh_ui_pen_get_width(lh_addr_of(pen)), 3);
}
