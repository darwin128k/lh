#include <gtest/gtest.h>

#include <lh/entity.h>
#include <lh/math/rect.h>
#include <lh/ui/brush.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/pen.h>
#include <lh/util/addr.h>

namespace
{

const lh_ui_color_t *
pixel(const lh_ui_canvas_t *canvas, lh_math_coord_t x, lh_math_coord_t y)
{
    return lh_ui_canvas_get_pixel(canvas, x, y);
}

} /* namespace */

TEST(entity, make_keeps_the_rect)
{
    const lh_math_rect_t rect = lh_math_rect_make(1, 2, 3, 4);
    lh_entity_t entity;
    lh_entity_init(lh_addr_of(entity), rect);
    const lh_math_rect_t stored = lh_entity_get_rect(lh_addr_of(entity));

    EXPECT_EQ(lh_math_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_entity_get_class(lh_addr_of(entity)), lh_addr_of(lh_entity_class));
}

TEST(entity, draw_fills_then_strokes_its_rect)
{
    lh_ui_color_t pixels[6 * 6] = {};
    lh_ui_canvas_t canvas;
    lh_entity_t entity;
    lh_ui_color_t fill;
    lh_ui_color_t line;
    lh_ui_brush_t brush;
    lh_ui_pen_t pen;
    lh_entity_init(lh_addr_of(entity), lh_math_rect_make(1, 1, 4, 4));
    lh_ui_color_init(lh_addr_of(fill), 255, 0, 0, 255);
    lh_ui_color_init(lh_addr_of(line), 0, 255, 0, 255);
    lh_ui_brush_init(lh_addr_of(brush), lh_addr_of(fill));
    lh_ui_pen_init(lh_addr_of(pen), lh_addr_of(line), 1);

    lh_ui_canvas_init(lh_addr_of(canvas), pixels, 6, 6, 6);
    lh_entity_draw(lh_addr_of(entity), lh_addr_of(canvas), lh_addr_of(brush), lh_addr_of(pen));

    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 2, 2)), 255);
    EXPECT_EQ(lh_ui_color_get_g(pixel(lh_addr_of(canvas), 2, 2)), 0);
    EXPECT_EQ(lh_ui_color_get_g(pixel(lh_addr_of(canvas), 1, 1)), 255);
    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 0, 0)), 0);
    EXPECT_EQ(lh_ui_color_get_g(pixel(lh_addr_of(canvas), 5, 5)), 0);
}

