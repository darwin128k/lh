/**
 * @file radio.cpp
 * @brief Tests for `lh/gx/radio.h` and `lh/gx/radio/group.h`.
 *
 * Three radios of one group, one above the other on a 40 by 30 screen. A tap
 * on a radio chooses it and unmarks the one chosen before; a tap on the radio
 * already chosen, or a release beside it, changes nothing; choosing from the
 * program calls no reaction and redraws only the two squares that changed.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/pointer.h>
#include <lh/gx/radio.h>
#include <lh/gx/render.h>

#include <vector>

namespace
{

constexpr lh_gx_color_t black = 0xFF000000U;
constexpr lh_gx_color_t white = 0xFFFFFFFFU;
constexpr lh_gx_color_t blue = 0xFF0000FFU;

lh_void
remember(lh_gx_radio_group_t *, lh_gx_radio_t *radio, lh_ptr context)
{
    static_cast<std::vector<lh_gx_radio_t *> *>(context)->push_back(radio);
}

lh_gx_point_t
point(lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;

    lh_gx_point_init(&at, x, y);
    return at;
}

/* Radio i at (2, 2 + 9 * i), 30 by 8; its mark at (4, 4 + 9 * i), 4 by 4. */
struct Screen
{
    lh_u32_t words[40 * 30] = {};
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_style_t back_style;
    lh_gx_style_t field_style;
    lh_gx_style_t mark_style;
    lh_gx_entity_t back;
    lh_gx_radio_group_t group;
    lh_gx_radio_t radios[3];
    lh_gx_pointer_t pointer;
    std::vector<lh_gx_radio_t *> changes;

    Screen()
    {
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, 40U, 30U, lh_gx_pixmap_format_argb8888);
        lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
        lh_gx_backend_sw_canvas_init(&canvas);
        lh_gx_render_init(&render, &canvas, &sw, 40, 30);
        lh_gx_style_init(&back_style, black, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&field_style, white, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&mark_style, blue, 0U, 0, 0, 0U, nullptr);
        lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 40, 30);
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
        lh_gx_radio_group_init(&group, remember, &changes);
        for (int i = 0; i < 3; ++i)
        {
            lh_gx_radio_init(&radios[i], &group, &field_style, &mark_style, 2, 2 + 9 * i, 30, 8,
                             nullptr);
            lh_gx_entity_attach(&back, lh_gx_radio_get_row_mut(&radios[i]));
        }
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
    mark(int i) const
    {
        return words[(6 + 9 * i) * 40 + 6];
    }

    lh_void
    tap(lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t to_x, lh_gx_scalar_t to_y)
    {
        const lh_gx_point_t down = point(x, y);
        const lh_gx_point_t up = point(to_x, to_y);

        lh_gx_pointer_press(&pointer, lh_gx_render_get_screen_mut(&render), &down);
        lh_gx_pointer_release(&pointer, &up);
    }

    lh_void
    tap_radio(int i)
    {
        tap(20, 5 + 9 * i, 20, 5 + 9 * i);
    }
};

TEST(gx_radio, a_new_group_has_nothing_chosen)
{
    Screen screen;

    EXPECT_TRUE(lh_gx_radio_group_get_selected(&screen.group) == nullptr);
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_FALSE(lh_gx_radio_is_selected(&screen.radios[i]));
        EXPECT_EQ(screen.mark(i), white);
        EXPECT_EQ(lh_gx_radio_get_group_mut(&screen.radios[i]), &screen.group);
        EXPECT_EQ(lh_gx_radio_from_entity(lh_gx_radio_get_row_mut(&screen.radios[i])),
                  &screen.radios[i]);
    }
}

TEST(gx_radio, a_tap_chooses_the_radio_and_unmarks_the_one_before)
{
    Screen screen;

    screen.tap_radio(0);
    screen.tap_radio(2);
    screen.draw();
    EXPECT_EQ(lh_gx_radio_group_get_selected(&screen.group), &screen.radios[2]);
    EXPECT_FALSE(lh_gx_radio_is_selected(&screen.radios[0]));
    EXPECT_TRUE(lh_gx_radio_is_selected(&screen.radios[2]));
    EXPECT_EQ(screen.mark(0), white);
    EXPECT_EQ(screen.mark(2), blue);
    EXPECT_EQ(screen.changes, (std::vector<lh_gx_radio_t *>{&screen.radios[0], &screen.radios[2]}));
}

TEST(gx_radio, a_tap_on_the_chosen_radio_or_a_release_beside_changes_nothing)
{
    Screen screen;

    screen.tap_radio(1);
    screen.tap_radio(1);
    screen.tap(20, 5, 38, 5);
    EXPECT_EQ(lh_gx_radio_group_get_selected(&screen.group), &screen.radios[1]);
    EXPECT_EQ(screen.changes.size(), 1U) << "the second tap and the release beside call nothing";
}

TEST(gx_radio, the_program_chooses_without_the_reaction_and_redraws_two_squares)
{
    Screen screen;

    lh_gx_radio_group_select(&screen.group, &screen.radios[0]);
    screen.draw();
    lh_gx_radio_group_select(&screen.group, &screen.radios[1]);
    EXPECT_TRUE(screen.changes.empty());
    const lh_gx_damage_t drawn = screen.draw();

    EXPECT_EQ(lh_gx_damage_get_count(&drawn), 2U) << "the mark hidden and the mark shown";
    EXPECT_EQ(screen.mark(0), white);
    EXPECT_EQ(screen.mark(1), blue);

    lh_gx_radio_group_select(&screen.group, nullptr);
    screen.draw();
    EXPECT_EQ(screen.mark(1), white) << "nothing chosen";
    EXPECT_TRUE(lh_gx_radio_group_choose(&screen.group, &screen.radios[2]));
    EXPECT_EQ(screen.changes.size(), 1U) << "choose is the user's: it calls the reaction";
}

} // namespace
