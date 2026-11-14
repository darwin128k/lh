#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
#include <lh/gl/layer.h>
#include <lh/gl/pixmap.h>
#include <lh/gl/shadow/fill.h>

namespace
{

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

TEST(gl_shadow, inside_the_box_is_clear_and_the_edge_fades)
{
    lh_gl_shadow_t shadow;
    lh_gl_shadow_init(lh_addr_of(shadow));
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 0, 0, 0, 255);
    lh_gl_shadow_set_color(lh_addr_of(shadow), lh_addr_of(color));
    lh_gl_shadow_set_spread(lh_addr_of(shadow), 2);
    lh_gl_rect_t box = rect_of(0, 0, 4, 4);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 1, 1, lh_addr_of(box)), 0);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 4, 2, lh_addr_of(box)), 127);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 5, 2, lh_addr_of(box)), 63);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 6, 2, lh_addr_of(box)), 0);
    lh_gl_shadow_set_offset(lh_addr_of(shadow), 2, 0);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 0, 1, lh_addr_of(box)), 0);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 5, 1, lh_addr_of(box)), 191);
}

TEST(gl_shadow_fill, paints_the_fade_and_leaves_the_box)
{
    lh_gl_color_t pixels[64];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 8, 8);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), origin, rect_of(0, 0, 8, 8));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 8, 8));
    lh_gl_shadow_t shadow;
    lh_gl_shadow_init(lh_addr_of(shadow));
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 0, 0, 0, 255);
    lh_gl_shadow_set_color(lh_addr_of(shadow), lh_addr_of(color));
    lh_gl_shadow_set_spread(lh_addr_of(shadow), 2);
    lh_gl_shadow_fill_t fill;
    lh_gl_shadow_fill_init(lh_addr_of(fill), rect_of(3, 3, 2, 2), lh_addr_of(shadow));
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_shadow_fill_get_entity_mut(lh_addr_of(fill)));
    lh_gl_rect_t clip = rect_of(0, 0, 8, 8);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_EQ(lh_gl_color_get_a(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 3, 3)), 0);
    EXPECT_EQ(lh_gl_color_get_a(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 3)), 63);
    EXPECT_FALSE(lh_gl_entity_class_get_opaque(lh_gl_shadow_fill_get_class()));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

} // namespace
