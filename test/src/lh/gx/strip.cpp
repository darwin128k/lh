/**
 * @file strip.cpp
 * @brief Tests for `lh/gx/strip.h`.
 *
 * A 40 by 30 screen — a background, two cards, a caption in the tiny font —
 * drawn in RGB565 through a band of 4 rows into a fake panel that keeps its
 * own frame, the way a display controller does. The panel's frame is the
 * screen drawn whole, pixel for pixel; a frame after a card moved sends only
 * the area that changed, and the panel is the new screen again.
 */

#include <gtest/gtest.h>

#include <lh/gx/box.h>
#include <lh/gx/render.h>
#include <lh/gx/strip.h>
#include <lh/gx/text.h>
#include <lh/test/gx/tiny_font.h>

#include <cstring>

namespace
{

constexpr int W = 40;
constexpr int H = 30;
constexpr int ROWS = 4;

/* A display controller: it keeps what it was sent, and counts it. */
struct Panel
{
    lh_u16_t frame[W * H] = {};
    int bands = 0;
    long pixels = 0;
};

lh_void
panel_show(lh_ptr context, const lh_gx_pixmap_t *band, lh_gx_point_t at,
           const lh_gx_damage_t *drawn)
{
    auto *panel = static_cast<Panel *>(context);

    panel->bands++;
    for (lh_u32_t i = 0U; i < lh_gx_damage_get_count(drawn); ++i)
    {
        lh_math_rect_t r;

        lh_gx_rect_to_rect(lh_gx_damage_get_rect(drawn, i), &r);
        const int x = lh_math_point_get_x(lh_math_rect_get_origin(&r));
        const int y = lh_math_point_get_y(lh_math_rect_get_origin(&r));
        const int w = lh_math_size_get_width(lh_math_rect_get_size(&r));
        const int h = lh_math_size_get_height(lh_math_rect_get_size(&r));

        for (int row = y; row < y + h; ++row)
        {
            const lh_byte_t *src =
                lh_gx_pixmap_get_address(band, static_cast<lh_u32_t>(x - lh_gx_point_get_x(&at)),
                                         static_cast<lh_u32_t>(row - lh_gx_point_get_y(&at)));

            std::memcpy(&panel->frame[row * W + x], src, static_cast<size_t>(w) * 2U);
        }
        panel->pixels += static_cast<long>(w) * h;
    }
}

struct Screen
{
    lh_test::gx_tiny_font tiny;
    lh_u16_t band_words[W * ROWS] = {};
    lh_memory_bounds_t band_bits;
    lh_gx_pixmap_t band;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_present_t present;
    lh_gx_style_t back_style;
    lh_gx_style_t card_style;
    lh_gx_entity_t back;
    lh_gx_entity_t card;
    lh_gx_entity_t other;
    lh_gx_entity_t caption;
    Panel panel;

    Screen()
    {
        lh_memory_bounds_init(&band_bits, band_words, sizeof(band_words));
        lh_gx_pixmap_init(&band, &band_bits, W, ROWS, lh_gx_pixmap_format_rgb565);
        lh_gx_backend_sw_init(&sw, &band, 0, 0);
        lh_gx_backend_sw_canvas_init(&canvas);
        lh_gx_render_init(&render, &canvas, &sw, W, H);
        lh_gx_present_init(&present);
        lh_gx_present_set_show(&present, panel_show);
        lh_gx_style_init(&back_style, 0xFF102030U, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&card_style, 0xFFE0A040U, 0xFF000000U, 1, 3, 0xFFFFFFFFU, &tiny.font);
        lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, W, H);
        lh_gx_entity_init(&card, &lh_gx_box_class, &card_style, 3, 5, 20, 11);
        lh_gx_entity_init(&other, &lh_gx_box_class, &card_style, 25, 18, 12, 9);
        lh_gx_entity_init(&caption, &lh_gx_text_class, &card_style, 4, 4, 6, 3);
        lh_gx_entity_set_content(&caption, const_cast<char *>("AA"));
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
        lh_gx_entity_attach(&back, &card);
        lh_gx_entity_attach(&back, &other);
        lh_gx_entity_attach(&card, &caption);
    }

    /* The screen drawn whole into a full RGB565 frame, the band set back after. */
    void
    whole(lh_u16_t *out)
    {
        lh_memory_bounds_t bits;
        lh_gx_pixmap_t full;
        lh_gx_rect_t all;

        lh_memory_bounds_init(&bits, out, W * H * 2);
        lh_gx_pixmap_init(&full, &bits, W, H, lh_gx_pixmap_format_rgb565);
        lh_gx_backend_sw_init(&sw, &full, 0, 0);
        lh_gx_rect_init(&all, 0, 0, W, H);
        lh_gx_render_paint_area(&render, &all);
        lh_gx_backend_sw_init(&sw, &band, 0, 0);
    }
};

TEST(gx_strip, the_panel_gets_the_screen_drawn_whole_through_a_small_band)
{
    Screen screen;
    lh_u16_t want[W * H] = {};

    ASSERT_TRUE(
        lh_gx_strip_draw(&screen.render, &screen.sw, &screen.band, &screen.present, &screen.panel));
    EXPECT_EQ(screen.panel.bands, (H + ROWS - 1) / ROWS) << "the first frame is the whole screen";
    EXPECT_EQ(screen.panel.pixels, W * H);
    screen.whole(want);
    EXPECT_EQ(std::memcmp(screen.panel.frame, want, sizeof(want)), 0);
    EXPECT_FALSE(
        lh_gx_strip_draw(&screen.render, &screen.sw, &screen.band, &screen.present, &screen.panel))
        << "nothing changed";
}

TEST(gx_strip, a_change_sends_only_what_changed)
{
    Screen screen;
    lh_u16_t want[W * H] = {};

    lh_gx_strip_draw(&screen.render, &screen.sw, &screen.band, &screen.present, &screen.panel);
    screen.panel.bands = 0;
    screen.panel.pixels = 0;
    lh_gx_rect_t moved;

    lh_gx_rect_init(&moved, 26, 3, 12, 9);
    lh_gx_entity_move(&screen.other, moved);
    ASSERT_TRUE(
        lh_gx_strip_draw(&screen.render, &screen.sw, &screen.band, &screen.present, &screen.panel));
    EXPECT_EQ(screen.panel.pixels, 2L * 12 * 9) << "where the card was and where it is";
    EXPECT_LT(screen.panel.pixels, W * H / 4);
    screen.whole(want);
    EXPECT_EQ(std::memcmp(screen.panel.frame, want, sizeof(want)), 0);
}

} // namespace
