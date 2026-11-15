#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
#include <lh/gl/layer.h>
#include <lh/gl/pixmap.h>
#include <lh/gl/text.h>

namespace
{

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

TEST(gl_text, one_glyph_paints_its_coverage)
{
    lh_byte_t bits[1] = {255};
    lh_gl_glyph_t glyph;
    lh_gl_glyph_init(lh_addr_of(glyph), 1, 1, 1, -1, bits);
    lh_gl_font_t font;
    lh_gl_font_init(lh_addr_of(font), lh_addr_of(glyph), 'A', 1, 1, 1);
    lh_gl_color_t pixels[1];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 1, 1);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 1, 1));
    lh_gl_color_t ink;
    lh_gl_color_init(lh_addr_of(ink), 200, 10, 10, 255);
    lh_gl_text_t text;
    lh_gl_text_init(lh_addr_of(text), rect_of(0, 0, 1, 1), lh_addr_of(font), lh_addr_of(ink), "A");
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_text_get_entity_mut(lh_addr_of(text)));
    lh_gl_rect_t clip = rect_of(0, 0, 1, 1);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0)), 200);
    lh_gl_text_set_text(lh_addr_of(text), "B");
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_EQ(lh_gl_color_get_a(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0)), 0);
    EXPECT_FALSE(lh_gl_entity_class_get_opaque(lh_gl_text_get_class()));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

} // namespace
