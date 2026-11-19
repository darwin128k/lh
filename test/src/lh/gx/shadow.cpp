/**
 * @file shadow.cpp
 * @brief Tests for `lh/gx/shadow.h`, the canvas's `fill_shadow` slot and a box that
 *        casts a shadow.
 *
 * A shadow reads back and knows how far it reaches. Through the software canvas
 * every pixel of it is the core's alpha in the shadow's colour, in a band moved
 * down the screen too. A card with a shadow draws it outside its own place, its
 * children are still cut to the place, and a card that moves takes its shadow
 * along: the old one is cleared. A canvas without the slot draws no shadow.
 */

#include <gtest/gtest.h>

#include <lh/core/pixel/shadow/ramp.h>
#include <lh/core/pixel/span.h>
#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/render.h>
#include <lh/gx/shadow/initializer.h>

namespace
{

constexpr lh_u32_t white = 0xFFFFFFFFU;
constexpr lh_u32_t card_color = 0xFF2050A0U;

lh_gx_rect_t
rect(lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t w, lh_gx_scalar_t h)
{
    lh_gx_rect_t r;

    lh_gx_rect_init(&r, x, y, w, h);
    return r;
}

/* What the shadow makes of a pixel of @p under: the core's alpha in its colour. */
lh_u32_t
shadowed(const lh_gx_shadow_t *shadow, const lh_gx_rect_t *box, lh_gx_scalar_t radius, int x, int y,
         lh_u32_t under)
{
    lh_pixel_shadow_t cast;
    const lh_byte_t alpha = [&]
    {
        lh_gx_shadow_get_pixel_shadow(shadow, box, radius, &cast);
        return lh_pixel_shadow_get_alpha(&cast, lh_pixel_shadow_get_row(&cast, y), x, y);
    }();
    lh_u32_t out = under;

    lh_pixel_span_cover_argb(&out, &alpha, 1U, lh_gx_shadow_get_color(shadow), 255U);
    return out;
}

TEST(gx_shadow, it_reads_back_and_knows_how_far_it_reaches)
{
    static const lh_gx_shadow_t constant = lh_gx_shadow_initializer(0x80000000U, 12, 0, 4);
    const lh_gx_rect_t box = rect(10, 20, 30, 40);
    lh_gx_shadow_t shadow;
    lh_gx_rect_t bounds;

    lh_gx_shadow_init(&shadow, 0x40102030U, 5, -2, 3);
    EXPECT_EQ(lh_gx_shadow_get_color(&shadow), 0x40102030U);
    EXPECT_EQ(lh_gx_shadow_get_spread(&shadow), 5);
    EXPECT_EQ(lh_gx_shadow_get_offset_x(&shadow), -2);
    EXPECT_EQ(lh_gx_shadow_get_offset_y(&shadow), 3);
    EXPECT_EQ(lh_gx_shadow_get_spread(&constant), 12);
    lh_gx_shadow_get_bounds(&shadow, &box, &bounds);
    EXPECT_TRUE(lh_gx_rect_eq(&bounds, &(const lh_gx_rect_t &)rect(10 - 2 - 6, 20 + 3 - 6, 42, 52)))
        << "shifted, grown by the spread and a pixel";
    lh_pixel_shadow_t cast;

    lh_gx_shadow_get_pixel_shadow(&shadow, &box, 4, &cast);
    EXPECT_EQ(lh_pixel_shadow_get_peak(&cast), 0x40);
    EXPECT_EQ(lh_pixel_shadow_get_spread(&cast), 5 * LH_PIXEL_ROUND_SUBPIXEL);
    EXPECT_EQ(lh_pixel_shadow_get_r(&cast), 4 * LH_PIXEL_ROUND_SUBPIXEL);
}

TEST(gx_shadow, the_software_canvas_lays_the_core_alphas_in_a_moved_band)
{
    static lh_u32_t words[60 * 10];
    lh_gx_shadow_t shadow;
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t band;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    const lh_gx_rect_t box = rect(15, 12, 24, 14);
    const lh_gx_rect_t clip = rect(0, 0, 60, 40);

    for (lh_u32_t &w : words)
    {
        w = white;
    }
    lh_gx_shadow_init(&shadow, 0xA0000000U, 6, 2, 3);
    lh_memory_bounds_init(&bits, words, sizeof(words));
    lh_gx_pixmap_init(&band, &bits, 60U, 10U, lh_gx_pixmap_format_argb8888);
    /* The band is rows 8 .. 17 of the screen. */
    lh_gx_backend_sw_init(&sw, &band, 0, 8);
    lh_gx_backend_sw_canvas_init(&canvas);
    ASSERT_TRUE(lh_gx_canvas_get_fill_shadow(&canvas) == lh_gx_backend_sw_fill_shadow);
    lh_gx_canvas_get_fill_shadow (&canvas)(&sw, &clip, &box, 5, &shadow);
    for (int y = 0; y < 10; ++y)
    {
        for (int x = 0; x < 60; ++x)
        {
            ASSERT_EQ(words[y * 60 + x], shadowed(&shadow, &box, 5, x, y + 8, white))
                << x << "," << y + 8;
        }
    }
    lh_gx_backend_sw_canvas_init_solid(&canvas);
    EXPECT_TRUE(lh_gx_canvas_get_fill_shadow(&canvas) == nullptr) << "a board's canvas casts none";
}

/* With a mask the backend lays a shadow from the alphas it kept: the same pixels as
   without, for a box elsewhere too; a mask too small for the shadow, and a shadow
   unlike the one kept, are worked out where they fall. */
TEST(gx_shadow, a_kept_mask_lays_the_same_pixels_for_every_shadow_like_it)
{
    static lh_u32_t with[60 * 40];
    static lh_u32_t without[60 * 40];
    static lh_byte_t bits[4096];
    lh_gx_shadow_t shadow;
    lh_memory_bounds_t with_bits;
    lh_memory_bounds_t without_bits;
    lh_gx_pixmap_t with_pixmap;
    lh_gx_pixmap_t without_pixmap;
    lh_gx_backend_sw_t kept;
    lh_gx_backend_sw_t live;
    lh_gx_backend_sw_shadow_t mask;
    const lh_gx_rect_t clip = rect(0, 0, 60, 40);
    const lh_gx_rect_t boxes[3] = {rect(10, 6, 20, 12), rect(31, 17, 20, 12), rect(5, 20, 14, 9)};

    for (int i = 0; i < 60 * 40; ++i)
    {
        with[i] = without[i] = white;
    }
    lh_gx_shadow_init(&shadow, 0x90000000U, 4, 1, 2);
    lh_memory_bounds_init(&with_bits, with, sizeof(with));
    lh_memory_bounds_init(&without_bits, without, sizeof(without));
    lh_gx_pixmap_init(&with_pixmap, &with_bits, 60U, 40U, lh_gx_pixmap_format_argb8888);
    lh_gx_pixmap_init(&without_pixmap, &without_bits, 60U, 40U, lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_init(&kept, &with_pixmap, 0, 0);
    lh_gx_backend_sw_init(&live, &without_pixmap, 0, 0);
    lh_gx_backend_sw_shadow_init(&mask, bits, sizeof(bits));
    lh_gx_backend_sw_set_shadow(&kept, &mask);
    EXPECT_TRUE(lh_gx_backend_sw_get_shadow(&kept) == &mask);
    EXPECT_FALSE(lh_gx_backend_sw_shadow_is_filled(&mask));
    for (const lh_gx_rect_t &box : boxes)
    {
        lh_gx_backend_sw_fill_shadow(&kept, &clip, &box, 3, &shadow);
        lh_gx_backend_sw_fill_shadow(&live, &clip, &box, 3, &shadow);
    }
    EXPECT_TRUE(lh_gx_backend_sw_shadow_is_filled(&mask));
    EXPECT_EQ(lh_gx_backend_sw_shadow_get_width(&mask), 14 + 10) << "the last shadow's bounds";
    for (int i = 0; i < 60 * 40; ++i)
    {
        ASSERT_EQ(with[i], without[i]) << i % 60 << "," << i / 60;
    }
    lh_gx_backend_sw_shadow_t small;
    lh_byte_t few[16];

    lh_gx_backend_sw_shadow_init(&small, few, sizeof(few));
    EXPECT_FALSE(lh_gx_backend_sw_shadow_can_hold(&small, 24, 22));
    EXPECT_TRUE(lh_gx_backend_sw_shadow_can_hold(&small, 4, 4));
}

/* A screen of 60 by 40, white, with one card at (15, 10), 24 by 14, casting a shadow,
   and a child of the card that reaches past it. */
struct Screen
{
    lh_u32_t words[60 * 40];
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_shadow_t shadow;
    lh_gx_style_t back_style;
    lh_gx_style_t card_style;
    lh_gx_style_t child_style;
    lh_gx_entity_t back;
    lh_gx_entity_t card;
    lh_gx_entity_t child;

    explicit Screen(bool with_slot) : words{}
    {
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, 60U, 40U, lh_gx_pixmap_format_argb8888);
        lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
        if (with_slot)
        {
            lh_gx_backend_sw_canvas_init(&canvas);
        }
        else
        {
            lh_gx_backend_sw_canvas_init_solid(&canvas);
        }
        lh_gx_render_init(&render, &canvas, &sw, 60, 40);
        lh_gx_shadow_init(&shadow, 0x80000000U, 5, 0, 3);
        lh_gx_style_init(&back_style, white, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&card_style, card_color, 0U, 0, 4, 0U, nullptr);
        lh_gx_style_set_shadow(&card_style, &shadow);
        lh_gx_style_init(&child_style, 0xFF00FF00U, 0U, 0, 0, 0U, nullptr);
        lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 60, 40);
        lh_gx_entity_init(&card, &lh_gx_box_class, &card_style, 15, 10, 24, 14);
        lh_gx_entity_init(&child, &lh_gx_box_class, &child_style, 20, 2, 10, 10);
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
        lh_gx_entity_attach(&back, &card);
        lh_gx_entity_attach(&card, &child);
        lh_gx_damage_t drawn;

        lh_gx_render_draw(&render, &drawn);
    }

    lh_u32_t
    at(int x, int y) const
    {
        return words[y * 60 + x];
    }
};

TEST(gx_shadow, a_card_casts_its_shadow_outside_its_place_and_cuts_its_children)
{
    Screen screen(true);
    const lh_gx_rect_t box = rect(15, 10, 24, 14);

    EXPECT_EQ(screen.at(27, 26), shadowed(&screen.shadow, &box, 4, 27, 26, white))
        << "below the card: its shadow";
    EXPECT_NE(screen.at(27, 26), white);
    EXPECT_EQ(screen.at(27, 35), white) << "past the spread: nothing";
    EXPECT_EQ(screen.at(20, 15), card_color) << "the card over its own shadow";
    EXPECT_EQ(screen.at(37, 20), 0xFF00FF00U) << "the child inside the card";
    EXPECT_EQ(screen.at(41, 20), shadowed(&screen.shadow, &box, 4, 41, 20, white))
        << "the child is cut to the card's place, not to its shadow";
    lh_gx_rect_t ink;

    lh_gx_entity_get_ink(&screen.card, &box, &ink);
    EXPECT_TRUE(lh_gx_rect_eq(&ink, &(const lh_gx_rect_t &)rect(9, 7, 36, 26)))
        << "the place and the shadow's bounds";
    lh_gx_entity_get_ink(&screen.back, &box, &ink);
    EXPECT_TRUE(lh_gx_rect_eq(&ink, &box)) << "no shadow: the place";
}

TEST(gx_shadow, a_moved_card_takes_its_shadow_along)
{
    Screen screen(true);
    const lh_gx_rect_t moved = rect(15, 2, 24, 14);
    lh_gx_damage_t drawn;

    lh_gx_entity_move(&screen.card, moved);
    lh_gx_render_draw(&screen.render, &drawn);
    EXPECT_EQ(screen.at(27, 30), white) << "where the old shadow was is cleared";
    EXPECT_EQ(screen.at(27, 18), shadowed(&screen.shadow, &moved, 4, 27, 18, white))
        << "below the card where it is now";
}

TEST(gx_shadow, a_canvas_without_the_slot_casts_none)
{
    Screen screen(false);

    EXPECT_EQ(screen.at(27, 26), white);
    EXPECT_EQ(screen.at(20, 15), card_color);
}

} // namespace
