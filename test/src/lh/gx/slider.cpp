/**
 * @file slider.cpp
 * @brief Tests for `lh/gx/slider.h`.
 *
 * A slider over 0..10 at 5, at (2,2) 30 by 8 on a black 40 by 12 screen: the
 * knob travels 22 pixels, so at 5 its left edge is 11 from the row's. A press
 * puts the knob under the pointer, a move drags it, a release leaves it, a
 * cancel puts it back; every change by the user calls the reaction, one by the
 * program does not.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/pointer.h>
#include <lh/gx/render.h>
#include <lh/gx/slider.h>

#include <vector>

namespace
{

constexpr lh_gx_color_t black = 0xFF000000U;
constexpr lh_gx_color_t grey = 0xFF808080U;
constexpr lh_gx_color_t blue = 0xFF0000FFU;
constexpr lh_gx_color_t white = 0xFFFFFFFFU;

lh_void
remember(lh_gx_slider_t *, lh_s32_t value, lh_ptr context)
{
    static_cast<std::vector<int> *>(context)->push_back(value);
}

lh_gx_point_t
point(lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;

    lh_gx_point_init(&at, x, y);
    return at;
}

bool
place_is(const lh_gx_entity_t *entity, lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t w,
         lh_gx_scalar_t h)
{
    lh_gx_rect_t want;

    lh_gx_rect_init(&want, x, y, w, h);
    return lh_gx_rect_eq(lh_gx_entity_get_place(entity), &want) == lh_bool_true;
}

struct Screen
{
    lh_u32_t words[40 * 12] = {};
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_style_t back_style;
    lh_gx_style_t rail_style;
    lh_gx_style_t accent_style;
    lh_gx_style_t knob_style;
    lh_gx_entity_t back;
    lh_gx_slider_t slider;
    lh_gx_pointer_t pointer;
    std::vector<int> changes;

    Screen()
    {
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, 40U, 12U, lh_gx_pixmap_format_argb8888);
        lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
        lh_gx_backend_sw_canvas_init(&canvas);
        lh_gx_render_init(&render, &canvas, &sw, 40, 12);
        lh_gx_style_init(&back_style, black, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&rail_style, grey, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&accent_style, blue, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&knob_style, white, 0U, 0, 0, 0U, nullptr);
        lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 40, 12);
        lh_gx_slider_init(&slider, &rail_style, &accent_style, &knob_style, 2, 2, 30, 8, 0, 10, 5,
                          remember, &changes);
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
        lh_gx_entity_attach(&back, lh_gx_slider_get_row_mut(&slider));
        lh_gx_pointer_init(&pointer);
        draw();
    }

    lh_void
    draw()
    {
        lh_gx_damage_t drawn;

        lh_gx_render_draw(&render, &drawn);
    }

    lh_u32_t
    at(int x, int y) const
    {
        return words[y * 40 + x];
    }

    lh_void
    press(lh_gx_scalar_t x)
    {
        const lh_gx_point_t where = point(x, 6);

        lh_gx_pointer_press(&pointer, lh_gx_render_get_screen_mut(&render), &where);
    }

    lh_void
    move(lh_gx_scalar_t x)
    {
        const lh_gx_point_t where = point(x, 30);

        lh_gx_pointer_move(&pointer, &where);
    }

    lh_void
    release(lh_gx_scalar_t x)
    {
        const lh_gx_point_t where = point(x, 6);

        lh_gx_pointer_release(&pointer, &where);
    }
};

TEST(gx_slider, the_rail_the_fill_and_the_knob_are_laid_out_for_the_value)
{
    Screen screen;

    EXPECT_EQ(lh_gx_slider_get_travel(&screen.slider), 22);
    EXPECT_EQ(lh_gx_slider_get_offset(&screen.slider), 11);
    EXPECT_TRUE(place_is(lh_gx_slider_get_rail(&screen.slider), 4, 3, 22, 2))
        << "from the knob's centre at 0 to its centre at 10, a quarter of the height";
    EXPECT_TRUE(place_is(lh_gx_slider_get_fill(&screen.slider), 4, 3, 11, 2));
    EXPECT_TRUE(place_is(lh_gx_slider_get_knob(&screen.slider), 11, 0, 8, 8));
    EXPECT_EQ(screen.at(14, 4), white) << "the knob";
    EXPECT_EQ(screen.at(8, 5), blue) << "the fill, left of the knob";
    EXPECT_EQ(screen.at(25, 5), grey) << "the rail, right of the knob";
    EXPECT_EQ(screen.at(25, 3), black) << "above the rail: the row draws nothing";
}

TEST(gx_slider, the_value_at_a_point_is_the_nearest_held_in_the_range)
{
    Screen screen;
    const lh_gx_point_t centre = point(17, 6);
    const lh_gx_point_t left = point(-40, 6);
    const lh_gx_point_t right = point(100, 6);
    const lh_gx_point_t step = point(8, 6);

    /* The row's left 2, half the knob 4: a knob centre at x is (x - 6) along. */
    EXPECT_EQ(lh_gx_slider_get_value_at(&screen.slider, &centre), 5);
    EXPECT_EQ(lh_gx_slider_get_value_at(&screen.slider, &left), 0);
    EXPECT_EQ(lh_gx_slider_get_value_at(&screen.slider, &right), 10);
    EXPECT_EQ(lh_gx_slider_get_value_at(&screen.slider, &step), 1) << "2 of 22 rounds to 1 of 10";
}

TEST(gx_slider, a_drag_moves_the_knob_and_calls_the_reaction_at_each_change)
{
    Screen screen;

    screen.press(6);
    EXPECT_EQ(lh_gx_slider_get_value(&screen.slider), 0) << "the knob jumps under the pointer";
    screen.move(28);
    EXPECT_EQ(lh_gx_slider_get_value(&screen.slider), 10)
        << "the height of the pointer does not count";
    screen.move(28);
    screen.release(17);
    EXPECT_EQ(lh_gx_slider_get_value(&screen.slider), 5);
    EXPECT_EQ(screen.changes, (std::vector<int>{0, 10, 5}))
        << "a move with no change calls nothing";
    screen.draw();
    EXPECT_EQ(screen.at(14, 4), white);
}

TEST(gx_slider, a_cancel_puts_the_value_back)
{
    Screen screen;

    screen.press(28);
    EXPECT_EQ(lh_gx_slider_get_value(&screen.slider), 10);
    lh_gx_pointer_cancel(&screen.pointer);
    EXPECT_EQ(lh_gx_slider_get_value(&screen.slider), 5);
    EXPECT_EQ(screen.changes, (std::vector<int>{10, 5}));
}

TEST(gx_slider, the_program_slides_it_without_the_reaction_and_the_knob_is_redrawn)
{
    Screen screen;

    EXPECT_TRUE(lh_gx_slider_slide(&screen.slider, 99));
    EXPECT_EQ(lh_gx_slider_get_value(&screen.slider), 10) << "held in the range";
    EXPECT_FALSE(lh_gx_slider_slide(&screen.slider, 10)) << "no change";
    EXPECT_TRUE(screen.changes.empty());
    screen.draw();
    EXPECT_EQ(screen.at(14, 5), blue) << "where the knob was: the fill now";
    EXPECT_EQ(screen.at(14, 4), black) << "above the rail, where the knob was";
    EXPECT_EQ(screen.at(26, 4), white) << "the knob at the end";
}

TEST(gx_slider, an_empty_range_stays_at_its_one_value)
{
    lh_gx_slider_t slider;

    lh_gx_slider_init(&slider, nullptr, nullptr, nullptr, 0, 0, 30, 8, 3, 3, 7, nullptr, nullptr);
    EXPECT_EQ(lh_gx_slider_get_value(&slider), 3);
    EXPECT_EQ(lh_gx_slider_get_offset(&slider), 0);
}

} // namespace
