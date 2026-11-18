/**
 * @file box.cpp
 * @brief Tests for `lh/gx/style.h` and `lh/gx/box.h`.
 *
 * The card, drawn by the software backend into a small screen the test reads
 * back: a square box is its rectangle exactly, a border is its width on every
 * side, a rounded corner leaves the corner pixel alone and anti-aliases the
 * edge, and a box covers what is under it only where it is opaque and clear of
 * its corners.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/render.h>
#include <lh/gx/style/initializer.h>

namespace
{

constexpr lh_u32_t back_color = 0xFF000000U;
constexpr lh_u32_t fill_color = 0xFFFFFFFFU;
constexpr lh_u32_t edge_color = 0xFFFF0000U;

/* A 20 by 12 screen with one box on a black background box. */
struct Screen
{
    lh_u32_t words[20 * 12];
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_style_t back_style;
    lh_gx_style_t card_style;
    lh_gx_entity_t back;
    lh_gx_entity_t card;

    Screen(lh_gx_scalar_t border, lh_gx_scalar_t radius) : words{}
    {
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, 20U, 12U, lh_gx_pixmap_format_argb8888);
        lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
        lh_gx_backend_sw_canvas_init(&canvas);
        lh_gx_render_init(&render, &canvas, &sw, 20, 12);
        lh_gx_style_init(&back_style, back_color, 0U, 0, 0);
        lh_gx_style_init(&card_style, fill_color, edge_color, border, radius);
        lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 20, 12);
        lh_gx_entity_init(&card, &lh_gx_box_class, &card_style, 2, 2, 16, 8);
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
        lh_gx_entity_attach(&back, &card);
        lh_gx_damage_t drawn;

        lh_gx_render_draw(&render, &drawn);
    }

    lh_u32_t
    at(int x, int y) const
    {
        return words[y * 20 + x];
    }
};

lh_gx_rect_t
rect(lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t w, lh_gx_scalar_t h)
{
    lh_gx_rect_t r;

    lh_gx_rect_init(&r, x, y, w, h);
    return r;
}

TEST(gx_style, a_style_reads_back_and_a_theme_is_a_constant_array)
{
    static const lh_gx_style_t theme[] = {lh_gx_style_initializer(1U, 2U, 3, 4),
                                          lh_gx_style_initializer(5U, 6U, 0, 0)};
    lh_gx_style_t style;

    lh_gx_style_init(&style, 0xFF112233U, 0xFF445566U, 2, 8);
    EXPECT_EQ(lh_gx_style_get_fill(&style), 0xFF112233U);
    EXPECT_EQ(lh_gx_style_get_border(&style), 0xFF445566U);
    EXPECT_EQ(lh_gx_style_get_border_width(&style), 2);
    EXPECT_EQ(lh_gx_style_get_radius(&style), 8);
    EXPECT_EQ(lh_gx_style_get_border_width(&theme[0]), 3);
    EXPECT_EQ(lh_gx_style_get_fill(&theme[1]), 5U);
}

TEST(gx_box, a_square_box_is_its_rectangle_exactly)
{
    Screen screen(0, 0);

    EXPECT_EQ(screen.at(1, 2), back_color);
    EXPECT_EQ(screen.at(2, 2), fill_color);
    EXPECT_EQ(screen.at(17, 9), fill_color);
    EXPECT_EQ(screen.at(18, 9), back_color);
    EXPECT_EQ(screen.at(17, 10), back_color);
}

TEST(gx_box, a_border_is_its_width_on_every_side)
{
    Screen screen(2, 0);

    EXPECT_EQ(screen.at(2, 5), edge_color) << "left";
    EXPECT_EQ(screen.at(3, 5), edge_color);
    EXPECT_EQ(screen.at(4, 5), fill_color);
    EXPECT_EQ(screen.at(17, 5), edge_color) << "right";
    EXPECT_EQ(screen.at(15, 5), fill_color);
    EXPECT_EQ(screen.at(9, 2), edge_color) << "top";
    EXPECT_EQ(screen.at(9, 3), edge_color);
    EXPECT_EQ(screen.at(9, 4), fill_color);
    EXPECT_EQ(screen.at(9, 9), edge_color) << "bottom";
}

TEST(gx_box, a_rounded_corner_leaves_the_corner_and_softens_the_edge)
{
    Screen screen(0, 3); /* 16 by 8 at (2, 2): straight rows 5 and 6 between the corners */

    EXPECT_EQ(screen.at(2, 2), back_color) << "the corner pixel itself is outside the curve";
    EXPECT_EQ(screen.at(5, 2), fill_color) << "past the corner, the top edge is straight";
    EXPECT_EQ(screen.at(2, 5), fill_color) << "below the corner, the left edge is straight";
    EXPECT_EQ(screen.at(9, 6), fill_color) << "the middle";
    const lh_u32_t edge = screen.at(2, 3);

    EXPECT_NE(edge, back_color) << "on the curve: partly covered";
    EXPECT_NE(edge, fill_color);
    EXPECT_EQ(screen.at(17, 9), back_color) << "the opposite corner too";
}

TEST(gx_box, a_rounded_border_rings_the_corner)
{
    Screen screen(2, 4);

    EXPECT_EQ(screen.at(2, 2), back_color);
    EXPECT_EQ(screen.at(7, 2), edge_color) << "top border past the corner";
    EXPECT_EQ(screen.at(9, 6), fill_color);
    EXPECT_EQ(screen.at(7, 4), fill_color) << "inside the border";
}

TEST(gx_box, it_covers_only_where_opaque_and_clear_of_the_corners)
{
    lh_gx_style_t style;
    lh_gx_entity_t card;
    const lh_gx_rect_t place = rect(0, 0, 20, 10);
    const lh_gx_rect_t middle = rect(5, 2, 4, 4);
    const lh_gx_rect_t corner = rect(0, 0, 3, 3);
    const lh_gx_rect_t top_strip = rect(4, 0, 12, 2);

    lh_gx_style_init(&style, 0xFFFFFFFFU, 0U, 0, 4);
    lh_gx_entity_init(&card, &lh_gx_box_class, &style, 0, 0, 20, 10);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &middle), lh_bool_true);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &top_strip), lh_bool_true) << "between the corners";
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &corner), lh_bool_false);
    lh_gx_style_set_fill(&style, 0x80FFFFFFU);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &middle), lh_bool_false) << "translucent";
    lh_gx_style_set_fill(&style, 0xFFFFFFFFU);
    lh_gx_style_init(&style, 0xFFFFFFFFU, 0x80000000U, 2, 0);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &middle), lh_bool_false) << "translucent border";
}

/* A card is restyled: only its rectangle is drawn again, and what is under it is not. */
TEST(gx_box, a_restyled_card_is_redrawn_alone)
{
    Screen screen(0, 0);
    lh_gx_style_t green;
    lh_gx_damage_t drawn;

    lh_gx_style_init(&green, 0xFF00FF00U, 0U, 0, 0);
    screen.words[0] = 0x12345678U;
    lh_gx_entity_set_style(&screen.card, &green);
    lh_gx_entity_invalidate(&screen.card);
    lh_gx_render_draw(&screen.render, &drawn);

    EXPECT_EQ(lh_gx_damage_area(&drawn), 16U * 8U);
    EXPECT_EQ(screen.at(9, 6), 0xFF00FF00U);
    EXPECT_EQ(screen.words[0], 0x12345678U);
}

} // namespace
