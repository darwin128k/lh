#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
#include <lh/gl/fill.h>
#include <lh/gl/layer.h>
#include <lh/gl/pixmap.h>
#include <lh/gl/surface.h>

namespace
{

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

lh_bool_t
is_color(const lh_gl_color_t *color, lh_gl_color_channel_t r, lh_gl_color_channel_t g,
         lh_gl_color_channel_t b, lh_gl_color_channel_t a)
{
    return lh_gl_color_get_r(color) == r && lh_gl_color_get_g(color) == g &&
                   lh_gl_color_get_b(color) == b && lh_gl_color_get_a(color) == a
               ? lh_bool_true
               : lh_bool_false;
}

TEST(gl_fill, layer_stays_empty_and_the_fill_reads_its_color)
{
    lh_gl_color_t pixels[16];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 4, 4);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), origin, rect_of(0, 0, 4, 4));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 4, 4));
    lh_gl_color_t back;
    lh_gl_color_init(lh_addr_of(back), 200, 10, 10, 255);
    lh_gl_color_t spot;
    lh_gl_color_init(lh_addr_of(spot), 0, 180, 20, 100);
    lh_gl_fill_t background;
    lh_gl_fill_init(lh_addr_of(background), rect_of(0, 0, 4, 4), lh_addr_of(back));
    lh_gl_fill_t mark;
    lh_gl_fill_init(lh_addr_of(mark), rect_of(1, 0, 1, 1), lh_addr_of(spot));
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_fill_get_entity_mut(lh_addr_of(background)));
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_fill_get_entity_mut(lh_addr_of(mark)));
    lh_gl_rect_t clip = rect_of(0, 0, 4, 4);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 200, 10, 10, 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 0), 122, 77, 14, 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 0), 200, 10, 10, 255));
    EXPECT_FALSE(lh_gl_entity_class_get_opaque(lh_gl_fill_get_class()));
    EXPECT_EQ(lh_gl_entity_class_get_draw_cb(lh_gl_layer_get_class()), lh_null);
    lh_gl_fill_set_color(lh_addr_of(mark), lh_null);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 0), 200, 10, 10, 255));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_fill, one_color_paints_two_fills)
{
    lh_gl_color_t pixels[4];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 4, 1);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), origin, rect_of(0, 0, 4, 1));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 4, 1));
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_gl_fill_t left;
    lh_gl_fill_init(lh_addr_of(left), rect_of(0, 0, 1, 1), lh_addr_of(color));
    lh_gl_fill_t right;
    lh_gl_fill_init(lh_addr_of(right), rect_of(2, 0, 1, 1), lh_addr_of(color));
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_fill_get_entity_mut(lh_addr_of(left)));
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_fill_get_entity_mut(lh_addr_of(right)));
    lh_gl_color_set_r(lh_addr_of(color), 9);
    lh_gl_rect_t clip = rect_of(0, 0, 4, 1);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 9, 2, 3, 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 0), 9, 2, 3, 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 0), 0, 0, 0, 0));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_fill, set_color_invalidates_a_channel_write_does_not)
{
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 20, 20));
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 20, 20);
    lh_gl_surface_t surface;
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(layer));
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_gl_fill_t fill;
    lh_gl_fill_init(lh_addr_of(fill), rect_of(2, 2, 4, 4), lh_addr_of(color));
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_fill_get_entity_mut(lh_addr_of(fill)));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_color_set_a(lh_addr_of(color), 40);
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    lh_gl_color_t other;
    lh_gl_color_init(lh_addr_of(other), 4, 5, 6, 7);
    lh_gl_fill_set_color(lh_addr_of(fill), lh_addr_of(other));
    EXPECT_GT(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    EXPECT_EQ(lh_gl_fill_get_color(lh_addr_of(fill)), lh_addr_of(other));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

} // namespace
