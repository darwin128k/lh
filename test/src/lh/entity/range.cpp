#include <gtest/gtest.h>

#include <lh/entity/knob.h>
#include <lh/entity/range.h>
#include <lh/entity/screen.h>
#include <lh/entity/scroll.h>
#include <lh/entity/slider.h>
#include <lh/entity/spin.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/null.h>
#include <lh/self.h>

#include <cstdlib>
#include <vector>

namespace
{

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
range_test_alloc(lh_self_ptr, lh_usize_t size)
{
    return std::malloc(static_cast<std::size_t>(size));
}

lh_void
range_test_dealloc(lh_self_ptr, lh_ptr ptr)
{
    std::free(ptr);
}

LH_COMPILER_EXTERN_C_END

class Range : public ::testing::Test
{
  protected:
    void
    SetUp() override
    {
        sized = lh_memory_allocator_initializer_with_context(range_test_alloc, range_test_dealloc,
                                                             lh_null, lh_null);
        screen = reinterpret_cast<lh_entity_screen_t *>(
            lh_entity_create_root(&lh_entity_screen_class, &sized));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(screen),
                              lh_math_vec2_make(200, 200));
        pixels.assign(200 * 200, lh_ui_color_make(0, 0, 0, 255));
        lh_ui_canvas_init(&canvas, pixels.data(), 200, 200, 200);
    }

    void
    TearDown() override
    {
        lh_entity_delete(root());
    }

    /* A canvas and a render, so that a test can start from a screen with
       nothing waiting to be redrawn. */
    void
    settle()
    {
        lh_entity_screen_render(screen, &canvas);
        ASSERT_EQ(lh_entity_screen_get_dirty_count(screen), 0u);
    }

    lh_entity_t *
    root() const
    {
        return reinterpret_cast<lh_entity_t *>(screen);
    }

    /* A bar of the given size, which is what every position mapping below
       measures itself against. */
    lh_entity_range_t *
    make_bar(lh_float_t w, lh_float_t h)
    {
        lh_entity_2d_t *box = reinterpret_cast<lh_entity_2d_t *>(
            lh_entity_create(&lh_entity_progress_class, root()));
        lh_entity_2d_set_size(box, lh_math_vec2_make(w, h));
        return reinterpret_cast<lh_entity_range_t *>(box);
    }

    lh_memory_allocator_t sized;
    lh_entity_screen_t *screen = nullptr;
    lh_ui_canvas_t canvas;
    std::vector<lh_ui_color_t> pixels;
};

TEST_F(Range, a_new_bar_runs_zero_to_hundred_at_zero)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);

    EXPECT_EQ(lh_entity_range_get_minimum(bar), 0);
    EXPECT_EQ(lh_entity_range_get_maximum(bar), 100);
    EXPECT_EQ(lh_entity_range_get_start(bar), 0);
    EXPECT_EQ(lh_entity_range_get_value(bar), 0);
    EXPECT_EQ(lh_entity_range_get_thickness(bar), LH_ENTITY_RANGE_THICKNESS);
}

TEST_F(Range, the_value_stays_inside_the_ends)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);

    lh_entity_range_set_value(bar, 250);
    EXPECT_EQ(lh_entity_range_get_value(bar), 100);
    lh_entity_range_set_value(bar, -9);
    EXPECT_EQ(lh_entity_range_get_value(bar), 0);

    lh_entity_range_set_ends(bar, 20, 80);
    EXPECT_EQ(lh_entity_range_get_value(bar), 20); // pulled back in
    lh_entity_range_set_value(bar, 50);
    lh_entity_range_set_ends(bar, 60, 70);
    EXPECT_EQ(lh_entity_range_get_value(bar), 60);
}

TEST_F(Range, an_upper_end_below_the_lower_one_is_raised)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);

    lh_entity_range_set_ends(bar, 50, 10);
    EXPECT_EQ(lh_entity_range_get_minimum(bar), 50);
    EXPECT_EQ(lh_entity_range_get_maximum(bar), 50);
}

TEST_F(Range, reset_returns_to_the_start_not_to_a_written_down_number)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);

    lh_entity_range_set_ends(bar, 0, 200);
    lh_entity_range_set_start(bar, 75);
    EXPECT_EQ(lh_entity_range_get_value(bar), 75); // setting the start moves here
    lh_entity_range_set_value(bar, 150);
    lh_entity_range_to_start(bar);
    EXPECT_EQ(lh_entity_range_get_value(bar), 75);
    EXPECT_EQ(lh_entity_range_get_value(bar), lh_entity_range_get_start(bar));
}

TEST_F(Range, a_start_outside_the_ends_is_pulled_in)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);

    lh_entity_range_set_ends(bar, 10, 20);
    lh_entity_range_set_start(bar, 999);
    EXPECT_EQ(lh_entity_range_get_start(bar), 20);
}

TEST_F(Range, percent_is_the_same_place_as_the_value)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);

    EXPECT_EQ(lh_entity_range_to_percent(bar), 0);
    lh_entity_range_set_value(bar, 100);
    EXPECT_EQ(lh_entity_range_to_percent(bar), 100);
    lh_entity_range_set_value(bar, 50);
    EXPECT_EQ(lh_entity_range_to_percent(bar), 50);

    // A quarter of the way through 0..40 is 10, and 10 is a quarter of the way.
    lh_entity_range_set_ends(bar, 0, 40);
    lh_entity_range_set_value(bar, 10);
    EXPECT_EQ(lh_entity_range_to_percent(bar), 25);

    // Ends that run backwards are a flat span, not a negative percentage.
    lh_entity_range_set_ends(bar, 10, 10);
    EXPECT_EQ(lh_entity_range_to_percent(bar), 0);
}

TEST_F(Range, percent_survives_a_span_that_does_not_divide)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);

    // Two of three is 66.7, and rounding to nearest makes that 67 rather than
    // a floor to 66 or a truncation to 66.
    lh_entity_range_set_ends(bar, 0, 3);
    lh_entity_range_set_from_percent(bar, 50);
    EXPECT_EQ(lh_entity_range_get_value(bar), 2);
    EXPECT_EQ(lh_entity_range_to_percent(bar), 67);

    // The round trip through a percentage stays inside a third of the step.
    lh_entity_range_set_from_percent(bar, 33);
    EXPECT_EQ(lh_entity_range_get_value(bar), 1);
    EXPECT_GE(lh_entity_range_to_percent(bar), 30);
    EXPECT_LE(lh_entity_range_to_percent(bar), 40);
}

TEST_F(Range, a_percent_outside_the_scale_is_the_nearer_end)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);

    lh_entity_range_set_ends(bar, 0, 200);
    lh_entity_range_set_from_percent(bar, -50);
    EXPECT_EQ(lh_entity_range_get_value(bar), 0);
    lh_entity_range_set_from_percent(bar, 500);
    EXPECT_EQ(lh_entity_range_get_value(bar), 200);
}

TEST_F(Range, the_travel_comes_from_the_own_size_of_the_bar)
{
    lh_entity_range_t *wide = make_bar(300.0f, 16.0f);
    lh_entity_range_t *tall = make_bar(16.0f, 300.0f);

    EXPECT_EQ(lh_entity_range_usable(wide), 300);
    EXPECT_EQ(lh_entity_range_usable(tall), 300); // the long side, whichever it is

    lh_entity_range_set_value(wide, 50);
    EXPECT_EQ(lh_entity_range_to_local_pos(wide), 150); // half of half
    lh_entity_range_set_value(tall, 50);
    EXPECT_EQ(lh_entity_range_to_local_pos(tall), 150);
}

TEST_F(Range, a_position_maps_back_to_the_value_it_came_from)
{
    lh_entity_range_t *bar = make_bar(200.0f, 10.0f);

    lh_entity_range_set_ends(bar, 0, 100);
    lh_entity_range_set_from_local_pos(bar, 50);
    EXPECT_EQ(lh_entity_range_get_value(bar), 25);
    EXPECT_EQ(lh_entity_range_to_local_pos(bar), 50);

    lh_entity_range_set_from_local_pos(bar, 200); // past the end
    EXPECT_EQ(lh_entity_range_get_value(bar), 100);
    lh_entity_range_set_from_local_pos(bar, -20);
    EXPECT_EQ(lh_entity_range_get_value(bar), 0);
}

TEST_F(Range, a_bar_with_no_size_yet_has_no_travel)
{
    lh_entity_range_t *bar =
        reinterpret_cast<lh_entity_range_t *>(lh_entity_create(&lh_entity_progress_class, root()));

    EXPECT_EQ(lh_entity_range_usable(bar), 0);
    EXPECT_EQ(lh_entity_range_to_local_pos(bar), 0);
    lh_entity_range_set_from_local_pos(bar, 50);
    EXPECT_EQ(lh_entity_range_get_value(bar), 0); // nothing to move along
}

TEST_F(Range, thickness_is_a_field_rather_than_a_constant_in_the_paint_path)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);

    lh_entity_range_set_thickness(bar, 9);
    EXPECT_EQ(lh_entity_range_get_thickness(bar), 9);
    lh_entity_range_set_thickness(bar, -4);
    EXPECT_EQ(lh_entity_range_get_thickness(bar), 0);
}

TEST_F(Range, the_ends_of_a_track_come_from_the_box_not_from_a_number)
{
    const lh_math_rect_t wide = lh_math_rect_make(0, 0, 200, 8);
    const lh_math_rect_t tall = lh_math_rect_make(0, 0, 8, 200);
    const lh_math_rect_t hair = lh_math_rect_make(0, 0, 1, 200);

    EXPECT_EQ(lh_entity_range_cap(&wide), 4);
    EXPECT_EQ(lh_entity_range_cap(&tall), 4);
    EXPECT_EQ(lh_entity_range_cap(&hair), 0); // too thin to round

    // A bar follows the long side and keeps its length.
    const lh_math_rect_t horizontal = lh_entity_range_track(&wide, 4);
    EXPECT_EQ(lh_math_rect_get_size_width(&horizontal), 200);
    EXPECT_EQ(lh_math_rect_get_size_height(&horizontal), 4);
    const lh_math_rect_t vertical = lh_entity_range_track(&tall, 4);
    EXPECT_EQ(lh_math_rect_get_size_width(&vertical), 4);
    EXPECT_EQ(lh_math_rect_get_size_height(&vertical), 200);

    // A thickness past the short side is kept to the short side.
    const lh_math_rect_t fat = lh_entity_range_track(&wide, 99);
    const lh_math_rect_t thin = lh_entity_range_track(&wide, -3);
    EXPECT_EQ(lh_math_rect_get_size_height(&fat), 8);
    EXPECT_EQ(lh_math_rect_get_size_height(&thin), 0);
}

TEST_F(Range, the_filled_part_is_clipped_to_the_track)
{
    const lh_math_rect_t track = lh_math_rect_make(10, 0, 100, 4);

    const lh_math_rect_t part = lh_entity_range_fill(&track, 40);
    const lh_math_rect_t over = lh_entity_range_fill(&track, 400);
    const lh_math_rect_t under = lh_entity_range_fill(&track, -5);
    EXPECT_EQ(lh_math_rect_get_size_width(&part), 40);
    EXPECT_EQ(lh_math_rect_get_size_width(&over), 100);
    EXPECT_EQ(lh_math_rect_get_size_width(&under), 0);

    // A tall track fills upward, so the fill sits at the far end.
    const lh_math_rect_t tall = lh_math_rect_make(0, 0, 4, 100);
    const lh_math_rect_t filled = lh_entity_range_fill(&tall, 30);
    EXPECT_EQ(lh_math_rect_get_y(&filled), 70);
    EXPECT_EQ(lh_math_rect_get_size_height(&filled), 30);
}

// The ladder: a trackbar, a knob, a spin box and a scrollbar are all a range,
// so they all answer the same questions through the same API. One getter per
// widget, and past that there is nothing widget-specific about a value.
TEST_F(Range, every_ranged_widget_hands_back_the_same_range)
{
    lh_entity_slider_t *slider = reinterpret_cast<lh_entity_slider_t *>(
        lh_entity_create(&lh_entity_slider_class, root()));
    lh_entity_knob_t *knob =
        reinterpret_cast<lh_entity_knob_t *>(lh_entity_create(&lh_entity_knob_class, root()));
    lh_entity_spin_t *spin =
        reinterpret_cast<lh_entity_spin_t *>(lh_entity_create(&lh_entity_spin_class, root()));
    lh_entity_scroll_t *scroll =
        reinterpret_cast<lh_entity_scroll_t *>(lh_entity_create(&lh_entity_scroll_class, root()));
    lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(slider), lh_math_vec2_make(200, 10));
    lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(knob), lh_math_vec2_make(60, 60));
    lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(spin), lh_math_vec2_make(120, 24));
    lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(scroll), lh_math_vec2_make(10, 200));

    lh_entity_range_t *ranges[4] = {lh_entity_slider_get_range(slider),
                                    lh_entity_knob_get_range(knob),
                                    lh_entity_spin_get_range(spin),
                                    lh_entity_scroll_get_range(scroll)};

    for (lh_int_t i = 0; i < 4; ++i)
    {
        lh_entity_range_t *bar = ranges[i];
        ASSERT_TRUE(bar != nullptr);
        // A brand new one is the same everywhere.
        EXPECT_EQ(lh_entity_range_get_minimum(bar), 0);
        EXPECT_EQ(lh_entity_range_get_maximum(bar), 100);
        EXPECT_EQ(lh_entity_range_get_start(bar), 0);
        EXPECT_EQ(lh_entity_range_get_value(bar), 0);
        EXPECT_EQ(lh_entity_range_get_thickness(bar), LH_ENTITY_RANGE_THICKNESS);

        // The four questions, asked the same way.
        lh_entity_range_set_ends(bar, 0, 200);
        lh_entity_range_set_start(bar, 20);
        EXPECT_EQ(lh_entity_range_get_start(bar), 20);
        lh_entity_range_set_value(bar, 50);
        EXPECT_EQ(lh_entity_range_get_value(bar), 50);
        EXPECT_EQ(lh_entity_range_to_percent(bar), 25);

        lh_entity_range_set_from_percent(bar, 75);
        EXPECT_EQ(lh_entity_range_get_value(bar), 150);
        lh_entity_range_set_thickness(bar, 6);
        EXPECT_EQ(lh_entity_range_get_thickness(bar), 6);

        lh_entity_range_to_start(bar);
        EXPECT_EQ(lh_entity_range_get_value(bar), 20);
    }
}

TEST_F(Range, the_range_a_widget_hands_back_is_the_widgets_own_record)
{
    lh_entity_slider_t *slider = reinterpret_cast<lh_entity_slider_t *>(
        lh_entity_create(&lh_entity_slider_class, root()));
    lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(slider), lh_math_vec2_make(200, 10));
    lh_entity_range_t *first = lh_entity_slider_get_range(slider);
    lh_entity_range_t *again = lh_entity_slider_get_range(slider);

    EXPECT_EQ(first, again);                 // the same record, not a copy
    EXPECT_EQ(lh_addr_of(slider->range), first); // and it is the member, by name
    lh_entity_range_set_value(first, 42);
    EXPECT_EQ(lh_entity_range_get_value(again), 42); // so a write is seen
}

TEST_F(Range, a_progress_bar_needs_no_getter_because_it_is_a_range)
{
    // The one widget that is a range outright, so the caller already holds the
    // whole API with nothing to ask for.
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);
    EXPECT_EQ(lh_addr_of(bar), lh_addr_of(bar));
    lh_entity_range_set_value(bar, 33);
    EXPECT_EQ(lh_entity_range_get_value(bar), 33);
}

TEST_F(Range, a_new_bar_leaves_the_way_it_runs_undecided)
{
    lh_entity_range_t *tall = make_bar(10.0f, 200.0f);
    lh_entity_range_t *wide = make_bar(200.0f, 10.0f);
    lh_entity_range_t *square = make_bar(40.0f, 40.0f);

    EXPECT_EQ(lh_entity_range_get_axis(tall), LH_ENTITY_RANGE_AXIS_AUTO);
    EXPECT_EQ(lh_entity_range_get_axis(wide), LH_ENTITY_RANGE_AXIS_AUTO);
    // Undecided is answered by the box, and a square box has nothing to
    // prefer, so it reads the way a progress bar does.
    EXPECT_EQ(lh_entity_range_is_vertical(tall), lh_bool_true);
    EXPECT_EQ(lh_entity_range_is_vertical(wide), lh_bool_false);
    EXPECT_EQ(lh_entity_range_is_vertical(square), lh_bool_false);
}

TEST_F(Range, an_axis_wins_over_the_shape_of_the_box)
{
    lh_entity_range_t *wide = make_bar(200.0f, 10.0f);
    lh_entity_range_t *tall = make_bar(10.0f, 200.0f);

    lh_entity_range_set_axis(wide, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_range_set_axis(tall, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    EXPECT_EQ(lh_entity_range_is_vertical(wide), lh_bool_true);
    EXPECT_EQ(lh_entity_range_is_vertical(tall), lh_bool_false);
    // And the travel is the side the axis names, not the long one.
    EXPECT_EQ(lh_entity_range_usable(wide), 10);
    EXPECT_EQ(lh_entity_range_usable(tall), 10);
}

TEST_F(Range, the_undecided_travel_is_the_long_side)
{
    lh_entity_range_t *bar = make_bar(200.0f, 10.0f);

    EXPECT_EQ(lh_entity_range_usable(bar), 200);
    lh_entity_range_set_axis(bar, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    EXPECT_EQ(lh_entity_range_usable(bar), 200);
    lh_entity_range_set_axis(bar, LH_ENTITY_RANGE_AXIS_VERTICAL);
    EXPECT_EQ(lh_entity_range_usable(bar), 10);
    lh_entity_range_set_axis(bar, LH_ENTITY_RANGE_AXIS_AUTO);
    EXPECT_EQ(lh_entity_range_usable(bar), 200);
}

TEST_F(Range, an_axis_that_is_none_of_the_three_is_the_undecided_one)
{
    lh_entity_range_t *bar = make_bar(10.0f, 200.0f);
    lh_entity_range_set_axis(bar, LH_ENTITY_RANGE_AXIS_HORIZONTAL);

    lh_entity_range_set_axis(bar, 99);
    EXPECT_EQ(lh_entity_range_get_axis(bar), LH_ENTITY_RANGE_AXIS_AUTO);
    EXPECT_EQ(lh_entity_range_is_vertical(bar), lh_bool_true); // back to the box
    lh_entity_range_set_axis(bar, -1);
    EXPECT_EQ(lh_entity_range_get_axis(bar), LH_ENTITY_RANGE_AXIS_AUTO);
}

TEST_F(Range, a_value_is_the_same_number_however_the_bar_is_turned)
{
    // The axis is where the number lives, not what it is: a bar on its side and
    // a bar upright agree about 40 out of 0 to 100, and each puts it on its own
    // track.
    lh_entity_range_t *wide = make_bar(200.0f, 10.0f);
    lh_entity_range_t *tall = make_bar(10.0f, 200.0f);
    lh_entity_range_set_axis(tall, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_range_set_ends(wide, 0, 100);
    lh_entity_range_set_ends(tall, 0, 100);
    lh_entity_range_set_value(wide, 40);
    lh_entity_range_set_value(tall, 40);

    EXPECT_EQ(lh_entity_range_to_percent(wide), lh_entity_range_to_percent(tall));
    EXPECT_EQ(lh_entity_range_to_local_pos(wide), 80);
    EXPECT_EQ(lh_entity_range_to_local_pos(tall), 80);
}

TEST_F(Range, a_value_is_a_value_is_a_value)
{
    lh_entity_range_t *first = make_bar(100.0f, 10.0f);
    lh_entity_range_t *second = make_bar(100.0f, 10.0f);
    lh_entity_range_set_value(first, 42);

    EXPECT_EQ(lh_entity_range_get_value(second), 0);
    lh_entity_range_set_value(second, 42);
    EXPECT_EQ(lh_entity_range_get_value(first), lh_entity_range_get_value(second));
}

TEST_F(Range, ends_that_do_not_move_leave_the_screen_alone)
{
    // A linked view re-states its bars' ends on every draw, so an end that did
    // not move must not mark a redraw: that is the difference between a window
    // that settles and one that repaints itself for ever. The screen starts
    // clean, so a new dirty area is a new dirty area and not a merge.
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);
    lh_entity_range_set_ends(bar, 0, 200);
    lh_entity_range_set_value(bar, 50);
    settle();

    lh_entity_range_set_ends(bar, 0, 200);
    EXPECT_EQ(lh_entity_screen_get_dirty_count(screen), 0u);

    lh_entity_range_set_ends(bar, 0, 201);
    EXPECT_EQ(lh_entity_screen_get_dirty_count(screen), 1u);
}

TEST_F(Range, the_value_a_shrunken_extent_pulls_back_counts_as_a_change)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);
    lh_entity_range_set_ends(bar, 0, 200);
    lh_entity_range_set_value(bar, 150);
    settle();

    // The ends stay put and the value cannot, so this is a change and says so.
    lh_entity_range_set_ends(bar, 0, 100);
    EXPECT_EQ(lh_entity_range_get_value(bar), 100);
    EXPECT_EQ(lh_entity_screen_get_dirty_count(screen), 1u);
}

TEST_F(Range, a_position_says_what_it_is_worth_before_it_is_written)
{
    lh_entity_range_t *bar = make_bar(100.0f, 10.0f);
    lh_entity_range_set_ends(bar, 0, 300);

    // The other half of lh_entity_range_to_pos: what a pointer's place along a
    // track would be worth, asked and not yet taken.
    EXPECT_EQ(lh_entity_range_to_value(bar, 0, 150), 0);
    EXPECT_EQ(lh_entity_range_to_value(bar, 75, 150), 150);
    EXPECT_EQ(lh_entity_range_to_value(bar, 150, 150), 300);
    EXPECT_EQ(lh_entity_range_get_value(bar), 0);

    // What it answers is what writing it gives, at every position there is.
    for (lh_int_t pos = 0; pos <= 150; ++pos)
    {
        const lh_int_t worth = lh_entity_range_to_value(bar, pos, 150);
        lh_entity_range_set_value(bar, 0);
        lh_entity_range_set_from_pos(bar, pos, 150);
        EXPECT_EQ(lh_entity_range_get_value(bar), worth);
    }

    // Past either end is that end, and a span of nothing is a value standing
    // where it stands: there is no position that would have meant anything.
    EXPECT_EQ(lh_entity_range_to_value(bar, 900, 150), 300);
    EXPECT_EQ(lh_entity_range_to_value(bar, -900, 150), 0);
    lh_entity_range_set_value(bar, 42);
    EXPECT_EQ(lh_entity_range_to_value(bar, 75, 0), 42);
    EXPECT_EQ(lh_entity_range_to_value(bar, 75, -5), 42);

    // And the minimum is not assumed to be zero: a bar that runs below it
    // answers from where it starts.
    lh_entity_range_set_ends(bar, -100, 100);
    EXPECT_EQ(lh_entity_range_to_value(bar, 0, 200), -100);
    EXPECT_EQ(lh_entity_range_to_value(bar, 100, 200), 0);
    EXPECT_EQ(lh_entity_range_to_value(bar, 200, 200), 100);
}

} // namespace
