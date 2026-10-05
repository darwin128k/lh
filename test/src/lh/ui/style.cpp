#include <gtest/gtest.h>

#include <lh/ui/brush.h>
#include <lh/ui/color.h>
#include <lh/ui/pen.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>

TEST(ui_style, make_keeps_the_brush_and_the_pen)
{
    lh_ui_color_t red;
    lh_ui_color_t green;
    lh_ui_brush_t brush;
    lh_ui_pen_t pen;
    lh_ui_style_t style;
    const lh_ui_color_t *fill;
    const lh_ui_color_t *line;

    lh_ui_color_init(lh_addr_of(red), 255, 0, 0, 255);
    lh_ui_color_init(lh_addr_of(green), 0, 255, 0, 128);
    lh_ui_brush_init(lh_addr_of(brush), lh_addr_of(red));
    lh_ui_pen_init(lh_addr_of(pen), lh_addr_of(green), 2);
    lh_ui_style_init(lh_addr_of(style), lh_addr_of(brush), lh_addr_of(pen));
    fill = lh_ui_brush_get_color(lh_ui_style_get_brush(lh_addr_of(style)));
    line = lh_ui_pen_get_color(lh_ui_style_get_pen(lh_addr_of(style)));

    EXPECT_EQ(lh_ui_color_get_r(fill), 255);
    EXPECT_EQ(lh_ui_color_get_a(fill), 255);
    EXPECT_EQ(lh_ui_color_get_g(line), 255);
    EXPECT_EQ(lh_ui_color_get_a(line), 128);
    EXPECT_EQ(lh_ui_pen_get_width(lh_ui_style_get_pen(lh_addr_of(style))), 2);
}
