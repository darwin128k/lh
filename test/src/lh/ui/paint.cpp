#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/paint.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

TEST(ui_paint, make_empty_has_no_color)
{
    lh_ui_paint_t paint;

    lh_ui_paint_init(lh_addr_of(paint));
    EXPECT_TRUE(lh_null_eq(lh_ui_paint_get_color(lh_addr_of(paint))));
}

TEST(ui_paint, make_keeps_the_color_pointer)
{
    lh_ui_color_t red;

    lh_ui_color_init(lh_addr_of(red), 1, 2, 3, 4);
    lh_ui_paint_t paint;
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(red));
    EXPECT_EQ(lh_ui_paint_get_color(lh_addr_of(paint)), lh_addr_of(red));
}

TEST(ui_paint, init_color_keeps_the_pointer)
{
    lh_ui_paint_t paint;
    lh_ui_color_t red;

    lh_ui_color_init_hex(lh_addr_of(red), 0xFF0000FFu);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(red));
    EXPECT_EQ(lh_ui_paint_get_color(lh_addr_of(paint)), lh_addr_of(red));
    lh_ui_paint_init_color(lh_addr_of(paint), lh_ptr_rcast(const lh_ui_color_t, lh_null));
    EXPECT_TRUE(lh_null_eq(lh_ui_paint_get_color(lh_addr_of(paint))));
}
