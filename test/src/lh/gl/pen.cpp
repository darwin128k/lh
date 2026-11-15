#include <gtest/gtest.h>

#include <lh/gl/pen.h>
#include <lh/gl/pen/initializer.h>

TEST(ui_pen, init_starts_empty_with_width_one)
{
    lh_gl_pen_t pen;

    lh_gl_pen_init(lh_addr_of(pen));
    EXPECT_TRUE(lh_gl_paint_is_empty(lh_gl_pen_get_paint(lh_addr_of(pen))));
    EXPECT_EQ(lh_gl_pen_get_width(lh_addr_of(pen)), 1);
}

TEST(ui_pen, set_paint_and_width)
{
    lh_gl_color_t color;

    lh_gl_color_init(lh_addr_of(color), 1, 2, 3, 4);
    lh_gl_paint_t paint;
    lh_gl_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_gl_pen_t pen;
    lh_gl_pen_init(lh_addr_of(pen));
    lh_gl_pen_set_paint_v(lh_addr_of(pen), lh_addr_of(paint));
    lh_gl_pen_set_width_v(lh_addr_of(pen), 3);
    EXPECT_FALSE(lh_gl_paint_is_empty(lh_gl_pen_get_paint(lh_addr_of(pen))));
    EXPECT_EQ(lh_gl_pen_get_width(lh_addr_of(pen)), 3);
}

TEST(ui_pen_initializer, fills_paint_then_width)
{
    const lh_gl_pen_t pen = lh_gl_pen_initializer(
        lh_initializer(lh_gl_paint_kind_solid, lh_initializer(0, 0, 0, 0xFF), nullptr), 3);

    EXPECT_EQ(pen.paint.kind, lh_gl_paint_kind_solid);
    EXPECT_EQ(pen.paint.color.a, 0xFF);
    EXPECT_EQ(pen.width, lh_gl_scalar(3));
}
