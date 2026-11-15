#include <gtest/gtest.h>

#include <lh/gl/brush.h>

#include <type_traits>

TEST(ui_brush, is_the_same_type_as_paint)
{
    EXPECT_TRUE((std::is_same<lh_gl_brush_t, lh_gl_paint_t>::value));
}

TEST(ui_brush, make_empty_and_init_are_usable)
{
    lh_gl_brush_t a;

    lh_gl_paint_init(lh_addr_of(a));
    lh_gl_brush_t b;
    lh_gl_brush_init(lh_addr_of(b));
    (void)a;
    (void)b;
}
