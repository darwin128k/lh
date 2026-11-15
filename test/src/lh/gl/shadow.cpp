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
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 1, 1, lh_addr_of(box), 0), 0);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 4, 2, lh_addr_of(box), 0), 127);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 5, 2, lh_addr_of(box), 0), 63);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 6, 2, lh_addr_of(box), 0), 0);
    lh_gl_shadow_set_offset(lh_addr_of(shadow), 2, 0);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 0, 1, lh_addr_of(box), 0), 0);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 5, 1, lh_addr_of(box), 0), 191);
    lh_gl_shadow_set_offset(lh_addr_of(shadow), 0, 0);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 1, 1, lh_addr_of(box), 2), 0);
    EXPECT_EQ(lh_gl_shadow_alpha_at(lh_addr_of(shadow), 0, 0, lh_addr_of(box), 2), 127);
}

TEST(gl_shadow, row_outside_is_the_distance)
{
    const lh_gl_scalar_t center_x = 48;
    const lh_gl_scalar_t center_y = 45;
    const lh_gl_scalar_t half_width = 36;
    const lh_gl_scalar_t half_height = 33;
    const lh_gl_scalar_t corner = 8;
    const lh_gl_scalar_t width = 96;

    for (lh_gl_scalar_t y = 0; y < 94; ++y)
    {
        lh_gl_scalar_t down = y >= center_y ? y - center_y : center_y - y;
        lh_gl_scalar_t dy = down - (half_height - corner);
        lh_gl_scalar_t left_end = 0;
        lh_gl_scalar_t right_begin = 0;

        lh_gl_shadow_row_outside(center_x, half_width, dy, corner, width, lh_addr_of(left_end),
                                 lh_addr_of(right_begin));
        for (lh_gl_scalar_t x = 0; x < width; ++x)
        {
            lh_bool_t outside =
                lh_gl_shadow_distance_at(x, y, center_x, center_y, half_width, half_height,
                                         corner) >= 0
                    ? lh_bool_true
                    : lh_bool_false;
            lh_bool_t covered = (x < left_end || x >= right_begin) ? lh_bool_true : lh_bool_false;

            EXPECT_EQ(covered, outside);
        }
    }
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
    lh_gl_canvas_init(lh_addr_of(canvas));
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

TEST(gl_shadow_fill, a_shifted_round_box_matches_the_distance)
{
    lh_gl_color_t pixels[120 * 120];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 120, 120);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 120, 120));
    lh_gl_shadow_t shadow;
    lh_gl_shadow_init(lh_addr_of(shadow));
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 0, 0, 0, 128);
    lh_gl_shadow_set_color(lh_addr_of(shadow), lh_addr_of(color));
    lh_gl_shadow_set_spread(lh_addr_of(shadow), 12);
    lh_gl_shadow_set_offset(lh_addr_of(shadow), 0, 4);
    lh_gl_rect_t box = rect_of(16, 16, 72, 66);
    lh_gl_shadow_fill_t fill;
    lh_gl_shadow_fill_init(lh_addr_of(fill), box, lh_addr_of(shadow));
    lh_gl_shadow_fill_set_radius(lh_addr_of(fill), 8);
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_shadow_fill_get_entity_mut(lh_addr_of(fill)));
    lh_gl_rect_t clip = rect_of(0, 0, 120, 120);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    const lh_gl_rect_t *area =
        lh_gl_entity_get_rect(lh_gl_shadow_fill_get_entity(lh_addr_of(fill)));
    lh_gl_scalar_t x0 = lh_gl_point_get_x(lh_gl_rect_get_origin(area));
    lh_gl_scalar_t y0 = lh_gl_point_get_y(lh_gl_rect_get_origin(area));
    lh_gl_scalar_t x1 = x0 + lh_gl_size_get_width(lh_gl_rect_get_size(area));
    lh_gl_scalar_t y1 = y0 + lh_gl_size_get_height(lh_gl_rect_get_size(area));

    for (lh_gl_scalar_t y = y0; y < y1; ++y)
    {
        for (lh_gl_scalar_t x = x0; x < x1; ++x)
        {
            EXPECT_EQ(lh_gl_color_get_a(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), x, y)),
                      lh_gl_shadow_alpha_at(lh_addr_of(shadow), x, y, lh_addr_of(box), 8));
        }
    }
    lh_gl_layer_deinit(lh_addr_of(layer));
}

} // namespace
