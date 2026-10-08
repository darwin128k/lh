#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/paint.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

TEST(ui_paint, init_is_empty)
{
    lh_ui_paint_t paint;

    lh_ui_paint_init(lh_addr_of(paint));
    EXPECT_EQ(lh_ui_paint_get_kind(lh_addr_of(paint)), lh_ui_paint_kind_none);
    EXPECT_TRUE(lh_ui_paint_is_empty(lh_addr_of(paint)));
    EXPECT_TRUE(lh_null_eq(lh_ui_paint_get_color(lh_addr_of(paint))));
}

TEST(ui_paint, init_color_copies_the_color)
{
    lh_ui_color_t red;
    lh_ui_paint_t paint;

    lh_ui_color_init(lh_addr_of(red), 1, 2, 3, 4);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(red));
    EXPECT_EQ(lh_ui_paint_get_kind(lh_addr_of(paint)), lh_ui_paint_kind_solid);
    EXPECT_FALSE(lh_ui_paint_is_empty(lh_addr_of(paint)));
    EXPECT_NE(lh_ui_paint_get_color(lh_addr_of(paint)), lh_addr_of(red));
    EXPECT_TRUE(lh_ui_color_equals(lh_ui_paint_get_color(lh_addr_of(paint)), lh_addr_of(red)));
}

TEST(ui_paint, color_change_after_init_does_not_reach_the_paint)
{
    lh_ui_color_t red;
    lh_ui_paint_t paint;

    lh_ui_color_init_hex(lh_addr_of(red), 0xFF0000FFu);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(red));
    lh_ui_color_set_r(lh_addr_of(red), 0);
    EXPECT_EQ(lh_ui_color_get_r(lh_ui_paint_get_color(lh_addr_of(paint))), 0xFF);
}

TEST(ui_paint, init_color_null_is_empty)
{
    lh_ui_paint_t paint;

    lh_ui_paint_init_color(lh_addr_of(paint), lh_null);
    EXPECT_TRUE(lh_ui_paint_is_empty(lh_addr_of(paint)));
    EXPECT_TRUE(lh_null_eq(lh_ui_paint_get_color(lh_addr_of(paint))));
}
