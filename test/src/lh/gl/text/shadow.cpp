#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
#include <lh/gl/pixmap.h>
#include <lh/gl/text/shadow.h>

namespace
{

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

void
white(lh_gl_color_t *pixels, int count)
{
    for (int i = 0; i < count; ++i)
    {
        lh_gl_color_init(pixels + i, 255, 255, 255, 255);
    }
}

} // namespace

TEST(gl_text_shadow, a_shift_moves_the_glyph_and_leaves_its_place)
{
    lh_byte_t bits[1] = {255};
    lh_gl_glyph_t glyph;

    lh_gl_glyph_init(lh_addr_of(glyph), 1, 1, 1, -1, bits);
    lh_gl_font_t font;

    lh_gl_font_init(lh_addr_of(font), lh_addr_of(glyph), 'A', 1, 1, 1);
    lh_gl_color_t ink;

    lh_gl_color_init(lh_addr_of(ink), 0, 0, 0, 255);
    lh_gl_text_t line;

    lh_gl_text_init(lh_addr_of(line), rect_of(1, 1, 1, 1), lh_addr_of(font), lh_addr_of(ink), "A");
    lh_gl_shadow_t shadow;

    lh_gl_shadow_init(lh_addr_of(shadow));
    lh_gl_shadow_set_color(lh_addr_of(shadow), lh_addr_of(ink));
    lh_gl_shadow_set_offset(lh_addr_of(shadow), 1, 0);
    lh_gl_color_t scratch[8];
    lh_gl_text_shadow_t cast;

    lh_gl_text_shadow_init(lh_addr_of(cast), lh_addr_of(line), lh_addr_of(shadow), scratch, 8);
    lh_gl_color_t pixels[4 * 4];

    white(pixels, 16);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 4, 4);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), 1, 1);
    lh_gl_canvas_t canvas;

    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_place_t place;

    lh_gl_place_init(lh_addr_of(place), origin, rect_of(0, 0, 4, 4), nullptr);
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_text_shadow_draw(lh_gl_text_shadow_get_entity(lh_addr_of(cast)), lh_addr_of(canvas),
                           lh_addr_of(place));
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1)), 255);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 1)), 0);
    EXPECT_FALSE(lh_gl_text_shadow_pick(lh_gl_text_shadow_get_entity(lh_addr_of(cast)), origin, 0));
}

TEST(gl_text_shadow, a_spread_softens_the_glyph)
{
    lh_byte_t bits[1] = {255};
    lh_gl_glyph_t glyph;

    lh_gl_glyph_init(lh_addr_of(glyph), 1, 1, 1, -1, bits);
    lh_gl_font_t font;

    lh_gl_font_init(lh_addr_of(font), lh_addr_of(glyph), 'A', 1, 1, 1);
    lh_gl_color_t ink;

    lh_gl_color_init(lh_addr_of(ink), 0, 0, 0, 255);
    lh_gl_text_t line;

    lh_gl_text_init(lh_addr_of(line), rect_of(2, 2, 1, 1), lh_addr_of(font), lh_addr_of(ink), "A");
    lh_gl_shadow_t shadow;

    lh_gl_shadow_init(lh_addr_of(shadow));
    lh_gl_shadow_set_color(lh_addr_of(shadow), lh_addr_of(ink));
    lh_gl_shadow_set_spread(lh_addr_of(shadow), 1);
    lh_gl_color_t scratch[64];
    lh_gl_text_shadow_t cast;

    lh_gl_text_shadow_init(lh_addr_of(cast), lh_addr_of(line), lh_addr_of(shadow), scratch, 64);
    lh_gl_color_t pixels[8 * 8];

    white(pixels, 64);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 8, 8);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), 1, 1);
    lh_gl_canvas_t canvas;

    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_place_t place;

    lh_gl_place_init(lh_addr_of(place), origin, rect_of(0, 0, 8, 8), nullptr);
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_text_shadow_draw(lh_gl_text_shadow_get_entity(lh_addr_of(cast)), lh_addr_of(canvas),
                           lh_addr_of(place));
    EXPECT_LT(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 2)), 255);
    EXPECT_LT(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 2)), 255);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0)), 255);
}

TEST(gl_text_shadow, a_short_scratch_draws_nothing)
{
    lh_byte_t bits[1] = {255};
    lh_gl_glyph_t glyph;

    lh_gl_glyph_init(lh_addr_of(glyph), 1, 1, 1, -1, bits);
    lh_gl_font_t font;

    lh_gl_font_init(lh_addr_of(font), lh_addr_of(glyph), 'A', 1, 1, 1);
    lh_gl_color_t ink;

    lh_gl_color_init(lh_addr_of(ink), 0, 0, 0, 255);
    lh_gl_text_t line;

    lh_gl_text_init(lh_addr_of(line), rect_of(0, 0, 1, 1), lh_addr_of(font), lh_addr_of(ink), "A");
    lh_gl_shadow_t shadow;

    lh_gl_shadow_init(lh_addr_of(shadow));
    lh_gl_shadow_set_color(lh_addr_of(shadow), lh_addr_of(ink));
    lh_gl_color_t scratch[1];
    lh_gl_text_shadow_t cast;

    lh_gl_text_shadow_init(lh_addr_of(cast), lh_addr_of(line), lh_addr_of(shadow), scratch, 0);
    lh_gl_color_t pixels[1];

    white(pixels, 1);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 1, 1);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;

    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_place_t place;

    lh_gl_place_init(lh_addr_of(place), origin, rect_of(0, 0, 1, 1), nullptr);
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_text_shadow_draw(lh_gl_text_shadow_get_entity(lh_addr_of(cast)), lh_addr_of(canvas),
                           lh_addr_of(place));
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0)), 255);
}
