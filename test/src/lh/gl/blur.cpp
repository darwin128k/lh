#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/gl/blur.h>
#include <lh/gl/canvas.h>
#include <lh/gl/glass.h>
#include <lh/gl/layer.h>
#include <lh/gl/pixmap.h>

namespace
{

const int k_width = 16;
const int k_height = 8;

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

void
fill_column(lh_gl_color_t *pixels, int x, lh_gl_color_channel_t r, lh_gl_color_channel_t a)
{
    for (int y = 0; y < k_height; ++y)
    {
        lh_gl_color_init(pixels + y * k_width + x, r, r, r, a);
    }
}

} // namespace

TEST(gl_blur, a_flat_picture_stays_flat)
{
    lh_gl_color_t pixels[k_width * k_height];

    for (int i = 0; i < k_width * k_height; ++i)
    {
        lh_gl_color_init(pixels + i, 0x40, 0x40, 0x40, 255);
    }
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), k_width, k_height);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_rect_t rect = rect_of(2, 1, 10, 5);
    lh_gl_color_t scratch[k_width * (k_height + 8)];
    lh_gl_blur_rect(lh_addr_of(pixmap), lh_addr_of(rect), 2, scratch);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 5, 3)), 0x40);
}

TEST(gl_blur, an_edge_becomes_a_ramp)
{
    lh_gl_color_t pixels[k_width * k_height];

    for (int x = 0; x < k_width; ++x)
    {
        fill_column(pixels, x, x < k_width / 2 ? 0 : 255, 255);
    }
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), k_width, k_height);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_rect_t rect = rect_of(0, 0, k_width, k_height);
    lh_gl_color_t scratch[k_width * (k_height + 8)];
    lh_gl_blur_rect(lh_addr_of(pixmap), lh_addr_of(rect), 3, scratch);
    const lh_gl_color_t *at_step = lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), k_width / 2 - 1, 3);
    const lh_gl_color_t *past_step = lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), k_width / 2, 3);
    lh_u8_t middle = lh_gl_color_get_r(at_step);
    lh_u8_t near = lh_gl_color_get_r(past_step);

    EXPECT_GT(middle, 0);
    EXPECT_LT(middle, 255);
    EXPECT_EQ(middle + near, 255);
    const lh_gl_color_t *untouched = lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), k_width / 2 - 4, 3);

    EXPECT_EQ(lh_gl_color_get_r(untouched), 0);
}

TEST(gl_blur, a_hidden_colour_does_not_tint)
{
    lh_gl_color_t pixels[2];

    lh_gl_color_init(pixels + 0, 255, 0, 0, 0);
    lh_gl_color_init(pixels + 1, 0, 0, 0, 255);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 2, 1);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_rect_t rect = rect_of(0, 0, 2, 1);
    lh_gl_color_t scratch[8];
    lh_gl_blur_rect(lh_addr_of(pixmap), lh_addr_of(rect), 1, scratch);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0)), 0);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 0)), 0);
}

TEST(gl_glass, a_rounded_corner_stays_and_the_middle_takes_the_tint)
{
    lh_gl_color_t pixels[8 * 8];

    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 8, 8);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_rect_t panel = rect_of(1, 1, 6, 6);
    lh_gl_color_t tint;
    for (int i = 0; i < 64; ++i)
    {
        lh_gl_color_init(pixels + i, 255, 255, 255, 255);
    }
    lh_gl_color_init(lh_addr_of(tint), 0, 0, 0, 255);
    lh_gl_glass(lh_addr_of(pixmap), lh_addr_of(panel), 3, 0, lh_addr_of(tint), nullptr, nullptr);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1)), 255);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 4, 4)), 0);
}

TEST(gl_glass, a_rounded_blur_keeps_the_missed_corner)
{
    lh_gl_color_t pixels[8 * 8];

    for (int i = 0; i < 64; ++i)
    {
        lh_gl_color_init(pixels + i, 255, 255, 255, 255);
    }
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 8, 8);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_rect_t panel = rect_of(1, 1, 6, 6);
    lh_gl_color_t tint;

    lh_gl_color_init(lh_addr_of(tint), 0, 0, 0, 255);
    lh_gl_color_t scratch[8 * 16];
    lh_gl_glass(lh_addr_of(pixmap), lh_addr_of(panel), 3, 1, lh_addr_of(tint), scratch, nullptr);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1)), 255);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 4, 4)), 0);
}

TEST(gl_glass, a_short_scratch_draws_nothing)
{
    lh_gl_color_t pixels[4 * 4];

    for (int i = 0; i < 16; ++i)
    {
        lh_gl_color_init(pixels + i, 10, 20, 30, 255);
    }
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 4, 4);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_rect_t clip = rect_of(0, 0, 4, 4);
    lh_gl_canvas_t canvas;

    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;

    lh_gl_layer_init(lh_addr_of(layer), clip);
    lh_gl_color_t tint;

    lh_gl_color_init(lh_addr_of(tint), 0, 0, 0, 128);
    lh_gl_color_t scratch[1];
    lh_gl_glass_t panel;
    lh_gl_glass_init(lh_addr_of(panel), clip, lh_addr_of(tint), scratch, 1);
    lh_gl_glass_set_blur(lh_addr_of(panel), 2);
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_glass_get_entity_mut(lh_addr_of(panel)));
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1)), 10);
    EXPECT_FALSE(lh_gl_entity_class_get_opaque(lh_gl_glass_get_class()));
    lh_gl_layer_deinit(lh_addr_of(layer));
}
