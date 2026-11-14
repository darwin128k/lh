#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
#include <lh/gl/fill.h>
#include <lh/gl/gradient/fill.h>
#include <lh/gl/layer.h>
#include <lh/gl/pixmap.h>
#include <lh/gl/text.h>

namespace
{

constexpr int kCards = 24;
constexpr int kGlyphs = 83;

lh_gl_color_t g_pixels[480 * 320];
lh_gl_glyph_t g_glyphs[kGlyphs];
lh_byte_t g_ink_bits[96];
char g_captions[kCards][8];

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

void
caption_of(int n, char *out)
{
    out[0] = 'C';
    out[1] = 'a';
    out[2] = 'r';
    out[3] = 'd';
    out[4] = ' ';
    if (n >= 10)
    {
        out[5] = static_cast<char>('0' + n / 10);
        out[6] = static_cast<char>('0' + n % 10);
        out[7] = 0;
        return;
    }
    out[5] = static_cast<char>('0' + n);
    out[6] = 0;
}

void
glyph_at(char code)
{
    lh_gl_glyph_init(lh_addr_of(g_glyphs[static_cast<int>(code - ' ')]), 8, 12, 8, -12, g_ink_bits);
}

} // namespace

TEST(gl_scene, a_card_is_round_and_its_caption_is_ink)
{
    lh_gl_color_t background;
    lh_gl_color_init(lh_addr_of(background), 20, 24, 29, 255);
    lh_gl_color_t blue;
    lh_gl_color_init(lh_addr_of(blue), 46, 107, 230, 255);
    lh_gl_color_t ink;
    lh_gl_color_init(lh_addr_of(ink), 255, 255, 255, 255);
    lh_gl_gradient_t gradient;
    lh_gl_gradient_init(lh_addr_of(gradient), lh_addr_of(blue), lh_addr_of(blue));
    lh_memory_std_set(g_glyphs, 0, sizeof g_glyphs);
    lh_memory_std_set(g_ink_bits, 255, sizeof g_ink_bits);
    lh_gl_glyph_init(lh_addr_of(g_glyphs[0]), 0, 0, 8, 0, lh_null);
    for (char code = '0'; code <= '9'; ++code)
    {
        glyph_at(code);
    }
    glyph_at('C');
    glyph_at('a');
    glyph_at('d');
    glyph_at('r');
    lh_gl_font_t font;
    lh_gl_font_init(lh_addr_of(font), g_glyphs, ' ', kGlyphs, 14, 12);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 480, 320);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), g_pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), origin, rect_of(0, 0, 480, 320));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 480, 320));
    lh_gl_fill_t back;
    lh_gl_fill_init(lh_addr_of(back), rect_of(0, 0, 480, 320), lh_addr_of(background));
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_fill_get_entity_mut(lh_addr_of(back)));
    lh_gl_gradient_fill_t faces[kCards];
    lh_gl_text_t lines[kCards];
    for (int i = 0; i < kCards; ++i)
    {
        lh_gl_scalar_t x = 8 + (i % 6) * 78;
        lh_gl_scalar_t y = 8 + (i / 6) * 78;
        caption_of(i + 1, g_captions[i]);
        lh_gl_gradient_fill_init(lh_addr_of(faces[i]), rect_of(x, y, 72, 66), lh_addr_of(gradient));
        lh_gl_gradient_fill_set_radius(lh_addr_of(faces[i]), 8);
        lh_gl_text_init(lh_addr_of(lines[i]), rect_of(x + 8, y + 8, 56, 16), lh_addr_of(font),
                        lh_addr_of(ink), g_captions[i]);
        lh_gl_layer_add(lh_addr_of(layer),
                        lh_gl_gradient_fill_get_entity_mut(lh_addr_of(faces[i])));
        lh_gl_layer_add(lh_addr_of(layer), lh_gl_text_get_entity_mut(lh_addr_of(lines[i])));
    }
    lh_memory_std_set(g_pixels, 0, sizeof g_pixels);
    lh_gl_rect_t clip = rect_of(0, 0, 480, 320);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 8, 8)), 20);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 40, 40)), 46);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 16, 16)), 255);
    lh_gl_layer_deinit(lh_addr_of(layer));
}
