#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
#include <lh/gl/card.h>
#include <lh/gl/fill.h>
#include <lh/gl/font/caption.h>
#include <lh/gl/glass.h>
#include <lh/gl/layer.h>
#include <lh/gl/pixmap.h>
#include <lh/gl/shadow/fill.h>
#include <lh/gl/text/shadow.h>

namespace
{

constexpr int k_cards = 24;

lh_gl_color_t g_pixels[480 * 320];
lh_gl_color_t g_glass_scratch[72 * 80];
lh_gl_color_t g_letter_scratch[80 * 80];
char g_captions[k_cards][8];

struct card_slot
{
    lh_gl_card_t card;
    lh_gl_shadow_fill_t shade;
    lh_gl_glass_t glass;
    lh_gl_text_shadow_t cast;
};

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

} // namespace

TEST(gl_scene, a_card_is_round_and_its_caption_is_ink)
{
    lh_gl_color_t background;

    lh_gl_color_init(lh_addr_of(background), 20, 24, 29, 255);
    lh_gl_color_t blue;

    lh_gl_color_init(lh_addr_of(blue), 46, 107, 230, 180);
    lh_gl_color_t ink;

    lh_gl_color_init(lh_addr_of(ink), 255, 255, 255, 255);
    lh_gl_color_t shade;

    lh_gl_color_init(lh_addr_of(shade), 0, 0, 0, 160);
    lh_gl_color_t tint;

    lh_gl_color_init(lh_addr_of(tint), 255, 255, 255, 48);
    const lh_gl_font_t *font = lh_gl_font_caption();
    lh_gl_shadow_t drop;

    lh_gl_shadow_init(lh_addr_of(drop));
    lh_gl_shadow_set_color(lh_addr_of(drop), lh_addr_of(shade));
    lh_gl_shadow_set_spread(lh_addr_of(drop), 8);
    lh_gl_shadow_set_offset(lh_addr_of(drop), 0, 4);
    lh_gl_shadow_t letter;

    lh_gl_shadow_init(lh_addr_of(letter));
    lh_gl_shadow_set_color(lh_addr_of(letter), lh_addr_of(shade));
    lh_gl_shadow_set_offset(lh_addr_of(letter), 1, 1);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 480, 320);
    lh_gl_pixmap_t pixmap;

    lh_gl_pixmap_init(lh_addr_of(pixmap), g_pixels, size);
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;

    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;

    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 480, 320));
    lh_gl_fill_t back;

    lh_gl_fill_init(lh_addr_of(back), rect_of(0, 0, 480, 320), lh_addr_of(background));
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_fill_get_entity_mut(lh_addr_of(back)));
    card_slot slots[k_cards];

    for (int i = 0; i < k_cards; ++i)
    {
        lh_gl_scalar_t x = 8 + (i % 6) * 78;
        lh_gl_scalar_t y = 8 + (i / 6) * 78;
        lh_gl_rect_t box = rect_of(x, y, 72, 66);

        caption_of(i + 1, g_captions[i]);
        lh_gl_shadow_fill_init(lh_addr_of(slots[i].shade), box, lh_addr_of(drop));
        lh_gl_shadow_fill_set_radius(lh_addr_of(slots[i].shade), 8);
        lh_gl_shadow_fill_place(lh_addr_of(slots[i].shade));
        lh_gl_layer_add(lh_addr_of(layer),
                        lh_gl_shadow_fill_get_entity_mut(lh_addr_of(slots[i].shade)));
        lh_gl_glass_init(lh_addr_of(slots[i].glass), box, lh_addr_of(tint), g_glass_scratch,
                         sizeof g_glass_scratch / sizeof g_glass_scratch[0]);
        lh_gl_glass_set_radius(lh_addr_of(slots[i].glass), 8);
        lh_gl_glass_set_blur(lh_addr_of(slots[i].glass), 2);
        lh_gl_layer_add(lh_addr_of(layer), lh_gl_glass_get_entity_mut(lh_addr_of(slots[i].glass)));
        lh_gl_card_init(lh_addr_of(slots[i].card), box, lh_addr_of(blue), font, g_captions[i]);
        lh_gl_card_set_radius(lh_addr_of(slots[i].card), 8);
        lh_gl_card_set_ink(lh_addr_of(slots[i].card), lh_addr_of(ink));
        lh_gl_text_shadow_init(lh_addr_of(slots[i].cast),
                               lh_gl_card_get_line(lh_addr_of(slots[i].card)), lh_addr_of(letter),
                               g_letter_scratch,
                               sizeof g_letter_scratch / sizeof g_letter_scratch[0]);
        lh_gl_layer_t *host = lh_gl_card_get_layer_mut(lh_addr_of(slots[i].card));
        lh_gl_entity_t *line =
            lh_gl_text_get_entity_mut(lh_gl_card_get_line_mut(lh_addr_of(slots[i].card)));

        EXPECT_TRUE(lh_gl_layer_try_remove(host, line));
        lh_gl_layer_add(host, lh_gl_text_shadow_get_entity_mut(lh_addr_of(slots[i].cast)));
        lh_gl_layer_add(host, line);
        lh_gl_layer_add(lh_addr_of(layer), lh_gl_card_get_entity_mut(lh_addr_of(slots[i].card)));
    }
    lh_memory_std_set(g_pixels, 0, sizeof g_pixels);
    lh_gl_rect_t clip = rect_of(0, 0, 480, 320);

    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_NE(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 40)), 20);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 40, 40)), 51);
    EXPECT_EQ(lh_gl_color_get_g(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 40, 40)), 95);
    EXPECT_EQ(lh_gl_color_get_b(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 40, 40)), 184);
    EXPECT_EQ(lh_gl_color_get_a(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 40, 40)), 255);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 8, 8)), 18);
    EXPECT_NE(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 10, 8)), 255);
    EXPECT_NE(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 9, 9)), 255);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 18, 12)), 255);
    lh_gl_layer_deinit(lh_addr_of(layer));
}
