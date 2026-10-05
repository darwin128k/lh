#include <gtest/gtest.h>

#include <lh/ui/brush.h>
#include <lh/ui/color.h>
#include <lh/ui/pen.h>
#include <lh/util/addr.h>

TEST(ui_brush, make_keeps_the_color)
{
    lh_ui_color_t red;
    lh_ui_brush_t brush;
    const lh_ui_color_t *color;

    lh_ui_color_init(lh_addr_of(red), 255, 0, 0, 255);
    lh_ui_brush_init(lh_addr_of(brush), lh_addr_of(red));
    color = lh_ui_brush_get_color(lh_addr_of(brush));

    EXPECT_EQ(lh_ui_color_get_r(color), 255);
    EXPECT_EQ(lh_ui_color_get_g(color), 0);
    EXPECT_EQ(lh_ui_color_get_a(color), 255);
}

TEST(ui_pen, make_keeps_color_and_width)
{
    lh_ui_color_t green;
    lh_ui_pen_t pen;
    const lh_ui_color_t *color;

    lh_ui_color_init(lh_addr_of(green), 0, 255, 0, 128);
    lh_ui_pen_init(lh_addr_of(pen), lh_addr_of(green), 3);
    color = lh_ui_pen_get_color(lh_addr_of(pen));

    EXPECT_EQ(lh_ui_color_get_g(color), 255);
    EXPECT_EQ(lh_ui_color_get_a(color), 128);
    EXPECT_EQ(lh_ui_pen_get_width(lh_addr_of(pen)), 3);
}
