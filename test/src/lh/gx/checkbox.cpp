/**
 * @file checkbox.cpp
 * @brief Tests for `lh/gx/checkbox.h`.
 *
 * A checkbox "A" at (2,2) 30 by 8 on a black 40 by 12 screen, drawn by the
 * software backend with the tiny font: the square is 8 by 8, the tick 4 by 4
 * inside it, the caption half a square right of it. A tap anywhere on the row
 * toggles it and calls the reaction; a release beside it does not; setting the
 * state from the program redraws only the square and calls nothing.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/checkbox.h>
#include <lh/gx/pointer.h>
#include <lh/gx/render.h>
#include <lh/test/gx/tiny_font.h>

#include <vector>

namespace
{

constexpr lh_gx_color_t black = 0xFF000000U;
constexpr lh_gx_color_t white = 0xFFFFFFFFU;
constexpr lh_gx_color_t blue = 0xFF0000FFU;
constexpr lh_gx_color_t red = 0xFFFF0000U;

lh_void
remember(lh_gx_checkbox_t *, lh_bool_t checked, lh_ptr context)
{
    static_cast<std::vector<bool> *>(context)->push_back(checked == lh_bool_true);
}

lh_gx_point_t
point(lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;

    lh_gx_point_init(&at, x, y);
    return at;
}

struct Screen
{
    lh_test::gx_tiny_font tiny;
    lh_u32_t words[40 * 12] = {};
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_style_t back_style;
    lh_gx_style_t field_style;
    lh_gx_style_t tick_style;
    lh_gx_entity_t back;
    lh_gx_checkbox_t box;
    lh_gx_pointer_t pointer;
    std::vector<bool> changes;

    Screen()
    {
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, 40U, 12U, lh_gx_pixmap_format_argb8888);
        lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
        lh_gx_backend_sw_canvas_init(&canvas);
        lh_gx_render_init(&render, &canvas, &sw, 40, 12);
        lh_gx_style_init(&back_style, black, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&field_style, white, 0U, 0, 0, red, &tiny.font);
        lh_gx_style_init(&tick_style, blue, 0U, 0, 0, 0U, nullptr);
        lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 40, 12);
        lh_gx_checkbox_init(&box, &field_style, &tick_style, 2, 2, 30, 8, "A", lh_bool_false,
                            remember, &changes);
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
        lh_gx_entity_attach(&back, lh_gx_checkbox_get_row_mut(&box));
        lh_gx_pointer_init(&pointer);
        draw();
    }

    lh_gx_damage_t
    draw()
    {
        lh_gx_damage_t drawn;

        lh_gx_render_draw(&render, &drawn);
        return drawn;
    }

    lh_u32_t
    at(int x, int y) const
    {
        return words[y * 40 + x];
    }

    lh_void
    tap(lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t to_x, lh_gx_scalar_t to_y)
    {
        const lh_gx_point_t down = point(x, y);
        const lh_gx_point_t up = point(to_x, to_y);

        lh_gx_pointer_press(&pointer, lh_gx_render_get_screen_mut(&render), &down);
        lh_gx_pointer_release(&pointer, &up);
    }
};

bool
place_is(const lh_gx_entity_t *entity, lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t w,
         lh_gx_scalar_t h)
{
    lh_gx_rect_t want;

    lh_gx_rect_init(&want, x, y, w, h);
    return lh_gx_rect_eq(lh_gx_entity_get_place(entity), &want) == lh_bool_true;
}

TEST(gx_checkbox, the_square_the_tick_and_the_caption_are_laid_out_in_the_row)
{
    Screen screen;

    EXPECT_TRUE(place_is(lh_gx_checkbox_get_square(&screen.box), 0, 0, 8, 8));
    EXPECT_TRUE(place_is(lh_gx_checkbox_get_tick(&screen.box), 2, 2, 4, 4))
        << "inset by a quarter of the square";
    /* Half a square right of it; "A" is 3 wide, a line 3 tall: (8-3+1)/2. */
    EXPECT_TRUE(place_is(lh_gx_checkbox_get_caption(&screen.box), 12, 3, 3, 3));
    EXPECT_FALSE(lh_gx_entity_is_visible(lh_gx_checkbox_get_tick(&screen.box)));
    EXPECT_EQ(lh_gx_checkbox_get_text(&screen.box), std::string("A"));
}

TEST(gx_checkbox, the_tick_is_drawn_only_while_checked)
{
    Screen screen;

    EXPECT_EQ(screen.at(2, 2), white) << "the square's corner";
    EXPECT_EQ(screen.at(6, 6), white) << "no tick";
    EXPECT_EQ(screen.at(10, 6), black) << "between the square and the caption";
    /* The caption at (14,5); ascent 2 below its top, so the "A" is rows 5 and 6. */
    EXPECT_EQ(screen.at(14, 5), red);

    screen.tap(6, 6, 6, 6);
    screen.draw();
    EXPECT_EQ(screen.at(6, 6), blue) << "the tick";
    EXPECT_EQ(screen.at(3, 3), white) << "the square around it";
}

TEST(gx_checkbox, a_tap_anywhere_on_the_row_toggles_it_and_calls_the_reaction)
{
    Screen screen;

    screen.tap(4, 4, 5, 5);
    EXPECT_TRUE(lh_gx_checkbox_is_checked(&screen.box));
    screen.tap(14, 5, 30, 8);
    EXPECT_FALSE(lh_gx_checkbox_is_checked(&screen.box)) << "on the caption, released on the row";
    EXPECT_EQ(screen.changes, (std::vector<bool>{true, false}));
}

TEST(gx_checkbox, a_release_beside_it_changes_nothing)
{
    Screen screen;

    screen.tap(4, 4, 36, 4);
    EXPECT_FALSE(lh_gx_checkbox_is_checked(&screen.box));
    EXPECT_TRUE(screen.changes.empty());
}

TEST(gx_checkbox, the_program_sets_the_state_without_the_reaction_and_redraws_the_square)
{
    Screen screen;

    lh_gx_checkbox_check(&screen.box, lh_bool_true);
    EXPECT_TRUE(lh_gx_checkbox_is_checked(&screen.box));
    EXPECT_TRUE(screen.changes.empty());
    const lh_gx_damage_t drawn = screen.draw();
    lh_gx_rect_t tick;

    lh_gx_rect_init(&tick, 4, 4, 4, 4);
    ASSERT_EQ(lh_gx_damage_get_count(&drawn), 1U);
    EXPECT_TRUE(lh_gx_rect_eq(lh_gx_damage_get_rect(&drawn, 0U), &tick))
        << "the tick's place on the screen: the row's (2,2) and its inset (2,2)";
    EXPECT_EQ(screen.at(6, 6), blue);
}

TEST(gx_checkbox, a_new_caption_is_laid_out_again)
{
    Screen screen;

    lh_gx_checkbox_set_text(&screen.box, "AA");
    EXPECT_TRUE(place_is(lh_gx_checkbox_get_caption(&screen.box), 12, 3, 6, 3));
    lh_gx_checkbox_set_text(&screen.box, nullptr);
    EXPECT_TRUE(
        lh_gx_rect_is_empty(lh_gx_entity_get_place(lh_gx_checkbox_get_caption(&screen.box))));
}

} // namespace
