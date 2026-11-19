/**
 * @file box.cpp
 * @brief Tests for `lh/gx/style.h` and `lh/gx/box.h`.
 *
 * The card, drawn by the software backend into a small screen the test reads
 * back: a square box is its rectangle exactly, a border is its width on every
 * side, a rounded corner leaves the corner pixel alone and anti-aliases the
 * edge, each corner takes its own radius, and a box covers what is under it
 * only where it is opaque and clear of its corners.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>
#include <lh/core/pixel/round.h>
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
        lh_gx_style_init(&back_style, back_color, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&card_style, fill_color, edge_color, border, radius, 0U, nullptr);
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
    static const lh_gx_style_t theme[] = {
        lh_gx_style_initializer(1U, 2U, 3, lh_gx_corners_initializer_all(4), 0U, nullptr, nullptr),
        lh_gx_style_initializer(5U, 6U, 0, lh_gx_corners_initializer(1, 2, 3, 4), 0U, nullptr,
                                nullptr)};
    lh_gx_style_t style;

    lh_gx_style_init(&style, 0xFF112233U, 0xFF445566U, 2, 8, 0U, nullptr);
    EXPECT_EQ(lh_gx_style_get_fill(&style), 0xFF112233U);
    EXPECT_EQ(lh_gx_style_get_border(&style), 0xFF445566U);
    EXPECT_EQ(lh_gx_style_get_border_width(&style), 2);
    EXPECT_EQ(lh_gx_corners_get_top_left(lh_gx_style_get_radius(&style)), 8) << "every corner";
    EXPECT_EQ(lh_gx_corners_get_bottom_left(lh_gx_style_get_radius(&style)), 8);
    lh_gx_corners_set_top_right(lh_gx_style_get_radius_mut(&style), 0);
    EXPECT_EQ(lh_gx_corners_get_top_right(lh_gx_style_get_radius(&style)), 0) << "one corner";
    lh_gx_style_set_radius(&style, lh_gx_style_get_radius(&theme[1]));
    EXPECT_EQ(lh_gx_corners_get_bottom_right(lh_gx_style_get_radius(&style)), 3) << "a copy";
    lh_gx_style_set_radius_all(&style, 6);
    EXPECT_EQ(lh_gx_corners_get_top_right(lh_gx_style_get_radius(&style)), 6);
    EXPECT_EQ(lh_gx_style_get_border_width(&theme[0]), 3);
    EXPECT_EQ(lh_gx_corners_get_bottom_left(lh_gx_style_get_radius(&theme[0])), 4);
    EXPECT_EQ(lh_gx_style_get_fill(&theme[1]), 5U);
    EXPECT_EQ(lh_gx_corners_get_top_right(lh_gx_style_get_radius(&theme[1])), 2);
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
    EXPECT_EQ(screen.at(5, 5), fill_color)
        << "inside the ring, in the corner's own pixels: the border is a ring there too";
}

TEST(gx_box, a_large_corner_drawn_in_bands_is_its_coverage_pixel_by_pixel)
{
    /* A white box 96 square, radius 40, on black: a corner of 40 rows goes to the
       canvas in bands of LH_GX_BOX_CORNER_BYTES / 40 rows. Each corner pixel is
       white at its coverage, and the four corners agree. */
    static lh_u32_t words[96 * 96];

    ASSERT_GT(40, LH_GX_BOX_CORNER_BYTES / 40) << "more than one band";
    lh_memory_bounds_t bits;
    lh_memory_bounds_init(&bits, words, sizeof(words));
    lh_gx_pixmap_t pixmap;
    lh_gx_pixmap_init(&pixmap, &bits, 96U, 96U, lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_t sw;
    lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
    lh_gx_canvas_t canvas;
    lh_gx_backend_sw_canvas_init(&canvas);
    lh_gx_render_t render;
    lh_gx_render_init(&render, &canvas, &sw, 96, 96);
    lh_gx_style_t back_style;
    lh_gx_style_init(&back_style, back_color, 0U, 0, 0, 0U, nullptr);
    lh_gx_style_t style;
    lh_gx_style_init(&style, fill_color, 0U, 0, 40, 0U, nullptr);
    lh_gx_entity_t back;
    lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 96, 96);
    lh_gx_entity_t card;
    lh_gx_entity_init(&card, &lh_gx_box_class, &style, 0, 0, 96, 96);
    lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
    lh_gx_entity_attach(&back, &card);
    lh_gx_damage_t drawn;
    lh_gx_render_draw(&render, &drawn);

    const lh_s64_t one = LH_PIXEL_ROUND_SUBPIXEL;
    for (int y = 0; y < 40; ++y)
    {
        for (int x = 0; x < 40; ++x)
        {
            const lh_u32_t c = lh_pixel_round_coverage(0, 0, 96 * one, 96 * one, 40 * one, x, y);
            const lh_u32_t want = 0xFF000000U | (c << 16) | (c << 8) | c;

            ASSERT_EQ(words[y * 96 + x], want) << x << "," << y;
            ASSERT_EQ(words[y * 96 + (95 - x)], want) << "top right " << x << "," << y;
            ASSERT_EQ(words[(95 - y) * 96 + x], want) << "bottom left " << x << "," << y;
            ASSERT_EQ(words[(95 - y) * 96 + (95 - x)], want) << "bottom right " << x << "," << y;
        }
    }
}

TEST(gx_box, it_covers_only_where_opaque_and_clear_of_the_corners)
{
    const lh_gx_rect_t place = rect(0, 0, 20, 10);
    const lh_gx_rect_t middle = rect(5, 2, 4, 4);
    const lh_gx_rect_t corner = rect(0, 0, 3, 3);
    const lh_gx_rect_t top_strip = rect(4, 0, 12, 2);

    lh_gx_style_t style;
    lh_gx_style_init(&style, 0xFFFFFFFFU, 0U, 0, 4, 0U, nullptr);
    lh_gx_entity_t card;
    lh_gx_entity_init(&card, &lh_gx_box_class, &style, 0, 0, 20, 10);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &middle), lh_bool_true);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &top_strip), lh_bool_true) << "between the corners";
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &corner), lh_bool_false);
    lh_gx_style_set_fill(&style, 0x80FFFFFFU);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &middle), lh_bool_false) << "translucent";
    lh_gx_style_set_fill(&style, 0xFFFFFFFFU);
    lh_gx_style_init(&style, 0xFFFFFFFFU, 0x80000000U, 2, 0, 0U, nullptr);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &middle), lh_bool_false) << "translucent border";
}

/* A 40 by 30 screen, black, with one white box edged in red at (2, 2), 36 by 26,
   the corners given; drawn into @p words. */
void
draw_box(const lh_gx_corners_t *corners, lh_gx_scalar_t border, lh_u32_t *words)
{
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_style_t back_style;
    lh_gx_style_t style;
    lh_gx_entity_t back;
    lh_gx_entity_t card;
    lh_gx_damage_t drawn;

    lh_memory_bounds_init(&bits, words, 40 * 30 * 4);
    lh_gx_pixmap_init(&pixmap, &bits, 40U, 30U, lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
    lh_gx_backend_sw_canvas_init(&canvas);
    lh_gx_render_init(&render, &canvas, &sw, 40, 30);
    lh_gx_style_init(&back_style, back_color, 0U, 0, 0, 0U, nullptr);
    lh_gx_style_init(&style, fill_color, edge_color, border, 0, 0U, nullptr);
    lh_gx_style_set_radius(&style, corners);
    lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 40, 30);
    lh_gx_entity_init(&card, &lh_gx_box_class, &style, 2, 2, 36, 26);
    lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
    lh_gx_entity_attach(&back, &card);
    lh_gx_render_draw(&render, &drawn);
}

/* Each corner of its own radius is, in its own square, the corner of a box rounded
   alike all round to that radius; everywhere else the box is the square one. With
   and without a border, a radius smaller than the border among them. */
TEST(gx_box, each_corner_of_its_own_radius_is_that_radius_and_the_rest_is_square)
{
    static lh_u32_t mixed[40 * 30];
    static lh_u32_t alike[40 * 30];
    static lh_u32_t square[40 * 30];
    const int radii[4] = {0, 1, 7, 13}; /* top left, top right, bottom right, bottom left */

    for (lh_gx_scalar_t border = 0; border <= 3; border += 3)
    {
        lh_gx_corners_t corners;
        lh_gx_corners_t none;

        lh_gx_corners_init(&corners, radii[0], radii[1], radii[2], radii[3]);
        lh_gx_corners_init_all(&none, 0);
        draw_box(&corners, border, mixed);
        draw_box(&none, border, square);
        /* Which corner's square a pixel is in, or -1. */
        auto corner_of = [&](int x, int y)
        {
            const int x0 = 2, y0 = 2, x1 = 38, y1 = 28;

            if (x < x0 + radii[0] && y < y0 + radii[0])
            {
                return 0;
            }
            if (x >= x1 - radii[1] && y < y0 + radii[1])
            {
                return 1;
            }
            if (x >= x1 - radii[2] && y >= y1 - radii[2])
            {
                return 2;
            }
            if (x < x0 + radii[3] && y >= y1 - radii[3])
            {
                return 3;
            }
            return -1;
        };
        for (int y = 0; y < 30; ++y)
        {
            for (int x = 0; x < 40; ++x)
            {
                const int k = corner_of(x, y);

                if (k < 0)
                {
                    ASSERT_EQ(mixed[y * 40 + x], square[y * 40 + x])
                        << "border " << border << " at " << x << "," << y;
                    continue;
                }
                lh_gx_corners_t one;

                lh_gx_corners_init_all(&one, radii[k]);
                draw_box(&one, border, alike);
                ASSERT_EQ(mixed[y * 40 + x], alike[y * 40 + x])
                    << "border " << border << ", corner " << k << " at " << x << "," << y;
            }
        }
    }
}

TEST(gx_box, a_strip_is_rows_where_neither_the_corners_nor_the_border_change)
{
    lh_math_rect_t outer;
    lh_gx_style_t style;

    lh_math_rect_init(&outer, 10, 20, 30, 16); /* rows 20 to 35 */
    lh_gx_style_init(&style, fill_color, edge_color, 2, 0, 0U, nullptr);
    lh_gx_corners_init(lh_gx_style_get_radius_mut(&style), 5, 3, 0, 8);
    EXPECT_EQ(lh_gx_box_get_left_px(&style, &outer, 20), 15) << "the top left corner's rows";
    EXPECT_EQ(lh_gx_box_get_left_px(&style, &outer, 25), 10) << "between the left corners";
    EXPECT_EQ(lh_gx_box_get_left_px(&style, &outer, 28), 18) << "the bottom left corner's rows";
    EXPECT_EQ(lh_gx_box_get_right_px(&style, &outer, 22), 37) << "the top right corner's rows";
    EXPECT_EQ(lh_gx_box_get_right_px(&style, &outer, 23), 40);
    EXPECT_EQ(lh_gx_box_get_right_px(&style, &outer, 35), 40) << "a square corner";
    EXPECT_EQ(lh_gx_box_get_strip_end_px(&style, &outer, 20), 22) << "the border rows end";
    EXPECT_EQ(lh_gx_box_get_strip_end_px(&style, &outer, 22), 23) << "the top right corner ends";
    EXPECT_EQ(lh_gx_box_get_strip_end_px(&style, &outer, 23), 25) << "the top left corner ends";
    EXPECT_EQ(lh_gx_box_get_strip_end_px(&style, &outer, 25), 28) << "the bottom left begins";
    EXPECT_EQ(lh_gx_box_get_strip_end_px(&style, &outer, 28), 34) << "the border rows begin";
    EXPECT_EQ(lh_gx_box_get_strip_end_px(&style, &outer, 34), 36) << "the bottom edge";
}

TEST(gx_box, a_box_rounded_on_top_covers_its_bottom_corners)
{
    const lh_gx_rect_t place = rect(0, 0, 20, 10);
    const lh_gx_rect_t top_left = rect(0, 0, 3, 3);
    const lh_gx_rect_t bottom_left = rect(0, 7, 3, 3);
    const lh_gx_rect_t bottom_strip = rect(0, 8, 20, 2);
    lh_gx_style_t style;
    lh_gx_entity_t card;

    lh_gx_style_init(&style, 0xFFFFFFFFU, 0U, 0, 0, 0U, nullptr);
    lh_gx_corners_init(lh_gx_style_get_radius_mut(&style), 4, 4, 0, 0);
    lh_gx_entity_init(&card, &lh_gx_box_class, &style, 0, 0, 20, 10);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &top_left), lh_bool_false);
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &bottom_left), lh_bool_true) << "a square corner";
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &bottom_strip), lh_bool_true) << "edge to edge";
    EXPECT_EQ(lh_gx_box_is_clear_of_corners(&style, &place, &bottom_strip), lh_bool_true);
    EXPECT_EQ(lh_gx_box_is_clear_of_corners(&style, &place, &top_left), lh_bool_false);
}

/* A card filled with a gradient: the inside is the gradient laid over the whole card,
   pixel for pixel, the strips and the corners joining into one; the border keeps its
   colour; it covers what is under it only while every stop is opaque. */
TEST(gx_box, a_gradient_fills_the_inside_as_one_over_the_whole_card)
{
    static lh_u32_t words[40 * 30];
    const lh_u32_t stops[2] = {0xFF2050A0U, 0xFFF0E010U};
    lh_pixel_gradient_t gradient;
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_style_t back_style;
    lh_gx_style_t style;
    lh_gx_entity_t back;
    lh_gx_entity_t card;
    lh_gx_damage_t drawn;
    lh_gx_brush_t brush;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, stops, lh_null, 2);
    lh_pixel_gradient_set_linear(&gradient, 120);
    lh_memory_bounds_init(&bits, words, sizeof(words));
    lh_gx_pixmap_init(&pixmap, &bits, 40U, 30U, lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
    lh_gx_backend_sw_canvas_init(&canvas);
    lh_gx_render_init(&render, &canvas, &sw, 40, 30);
    lh_gx_style_init(&back_style, back_color, 0U, 0, 0, 0U, nullptr);
    lh_gx_style_init(&style, fill_color, edge_color, 2, 6, 0U, nullptr);
    lh_gx_style_set_gradient(&style, &gradient);
    EXPECT_TRUE(lh_gx_style_get_gradient(&style) == &gradient);
    lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 40, 30);
    lh_gx_entity_init(&card, &lh_gx_box_class, &style, 2, 2, 36, 26);
    lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
    lh_gx_entity_attach(&back, &card);
    lh_gx_render_draw(&render, &drawn);
    lh_gx_brush_init_gradient(&brush, &gradient, nullptr, rect(2, 2, 36, 26));
    /* Inside the border and clear of the corners' squares. */
    for (int y = 4; y < 26; ++y)
    {
        for (int x = 4; x < 36; ++x)
        {
            const bool corner = (x < 8 || x >= 32) && (y < 8 || y >= 22);

            if (!corner)
            {
                ASSERT_EQ(words[y * 40 + x], lh_gx_brush_get_color_at(&brush, x, y))
                    << x << "," << y;
            }
        }
    }
    EXPECT_EQ(words[2 * 40 + 20], edge_color) << "the border keeps its colour";
    EXPECT_EQ(words[2 * 40 + 2], back_color) << "the corner pixel is outside the curve";
    /* Inside a corner's ring the fill is the gradient too, at full coverage. */
    EXPECT_EQ(words[6 * 40 + 6], lh_gx_brush_get_color_at(&brush, 6, 6));
    const lh_gx_rect_t place = rect(2, 2, 36, 26);
    const lh_gx_rect_t middle = rect(10, 10, 4, 4);

    EXPECT_TRUE(lh_gx_style_is_fill_opaque(&style));
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &middle), lh_bool_true);
    const lh_u32_t half[2] = {0xFF2050A0U, 0x80F0E010U};

    lh_pixel_gradient_init_stops(&gradient, half, lh_null, 2);
    EXPECT_FALSE(lh_gx_style_is_fill_opaque(&style));
    EXPECT_EQ(lh_gx_box_covers(&card, &place, &middle), lh_bool_false) << "a translucent stop";
}

/* A card is restyled: only its rectangle is drawn again, and what is under it is not. */
TEST(gx_box, a_restyled_card_is_redrawn_alone)
{
    Screen screen(0, 0);
    lh_gx_style_t green;

    lh_gx_style_init(&green, 0xFF00FF00U, 0U, 0, 0, 0U, nullptr);
    screen.words[0] = 0x12345678U;
    lh_gx_entity_set_style(&screen.card, &green);
    lh_gx_entity_invalidate(&screen.card);
    lh_gx_damage_t drawn;
    lh_gx_render_draw(&screen.render, &drawn);

    EXPECT_EQ(lh_gx_damage_area(&drawn), 16U * 8U);
    EXPECT_EQ(screen.at(9, 6), 0xFF00FF00U);
    EXPECT_EQ(screen.words[0], 0x12345678U);
}

} // namespace
