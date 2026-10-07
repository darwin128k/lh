#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/paint.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

TEST(ui_style, init_has_empty_fill)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    EXPECT_TRUE(lh_ui_paint_is_empty(lh_ui_style_get_fill(lh_addr_of(style))));
    EXPECT_TRUE(lh_null_eq(lh_ui_style_get_fill_color(lh_addr_of(style))));
}

TEST(ui_style, set_fill_copies_paint)
{
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 4);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    EXPECT_FALSE(lh_ui_paint_is_empty(lh_ui_style_get_fill(lh_addr_of(style))));
    EXPECT_EQ(lh_ui_color_get_r(lh_ui_style_get_fill_color(lh_addr_of(style))), 1);
    EXPECT_EQ(lh_ui_color_get_g(lh_ui_style_get_fill_color(lh_addr_of(style))), 2);
    EXPECT_EQ(lh_ui_color_get_b(lh_ui_style_get_fill_color(lh_addr_of(style))), 3);
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
    EXPECT_TRUE(lh_ui_paint_is_empty(lh_ui_style_get_fill(lh_addr_of(style))));
    EXPECT_TRUE(lh_null_eq(lh_ui_style_get_fill_color(lh_addr_of(style))));
}

TEST(ui_style, radius_starts_square_and_keeps_the_value)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    EXPECT_EQ(lh_ui_style_get_radius(lh_addr_of(style)), lh_ui_scalar(0));
    lh_ui_style_set_radius(lh_addr_of(style), lh_ui_scalar(6));
    EXPECT_EQ(lh_ui_style_get_radius(lh_addr_of(style)), lh_ui_scalar(6));
    lh_ui_style_set_radius(lh_addr_of(style), LH_UI_RADIUS_CIRCLE);
    EXPECT_EQ(lh_ui_style_get_radius(lh_addr_of(style)), LH_UI_RADIUS_CIRCLE);
}

TEST(ui_style, padding_starts_at_zero_and_keeps_what_is_set)
{
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    EXPECT_EQ(lh_ui_style_get_padding(lh_addr_of(style)), lh_ui_scalar(0));
    lh_ui_style_set_padding(lh_addr_of(style), lh_ui_scalar(12));
    EXPECT_EQ(lh_ui_style_get_padding(lh_addr_of(style)), lh_ui_scalar(12));
}
