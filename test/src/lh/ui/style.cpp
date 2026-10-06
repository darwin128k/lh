#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/paint.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

TEST(ui_style, init_has_no_fill)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    EXPECT_TRUE(lh_null_eq(lh_ui_style_get_fill(lh_addr_of(style))));
}

TEST(ui_style, set_fill_keeps_paint_pointer)
{
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 4);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    EXPECT_EQ(lh_ui_style_get_fill(lh_addr_of(style)), lh_addr_of(paint));
    EXPECT_EQ(lh_ui_paint_get_color(lh_ui_style_get_fill(lh_addr_of(style))), lh_addr_of(color));
}

TEST(ui_style, set_fill_null_clears)
{
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;

    lh_ui_color_init(lh_addr_of(color), 10, 20, 30, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_style_set_fill(lh_addr_of(style), lh_ptr_rcast(const lh_ui_paint_t, lh_null));
    EXPECT_TRUE(lh_null_eq(lh_ui_style_get_fill(lh_addr_of(style))));
}
