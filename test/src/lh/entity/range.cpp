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
    }

    void
    TearDown() override
    {
        lh_entity_delete(root());
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

// The ladder: a trackbar, a knob, a spin box and a scrollbar are all a range.
// These four are the same question asked four times, and they are answered the
// same way, so a caller holding any of them writes the same line.
TEST_F(Range, every_ranged_widget_answers_the_same_questions)
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

    lh_entity_slider_set_ends(slider, 0, 200);
    lh_entity_knob_set_ends(knob, 0, 200);
    lh_entity_spin_set_ends(spin, 0, 200);
    lh_entity_scroll_set_ends(scroll, 0, 200);

    EXPECT_EQ(lh_entity_slider_get_minimum(slider), 0);
    EXPECT_EQ(lh_entity_knob_get_minimum(knob), 0);
    EXPECT_EQ(lh_entity_spin_get_minimum(spin), 0);
    EXPECT_EQ(lh_entity_scroll_get_minimum(scroll), 0);
    EXPECT_EQ(lh_entity_slider_get_maximum(slider), 200);
    EXPECT_EQ(lh_entity_knob_get_maximum(knob), 200);
    EXPECT_EQ(lh_entity_spin_get_maximum(spin), 200);
    EXPECT_EQ(lh_entity_scroll_get_maximum(scroll), 200);

    lh_entity_slider_set_value(slider, 50);
    lh_entity_knob_set_value(knob, 50);
    lh_entity_spin_set_value(spin, 50);
    lh_entity_scroll_set_value(scroll, 50);

    EXPECT_EQ(lh_entity_slider_get_value(slider), 50);
    EXPECT_EQ(lh_entity_knob_get_value(knob), 50);
    EXPECT_EQ(lh_entity_spin_get_value(spin), 50);
    EXPECT_EQ(lh_entity_scroll_get_value(scroll), 50);

    // The same four, asked as a percentage: this is the question a caller
    // mirrors one widget with another.
    EXPECT_EQ(lh_entity_slider_to_percent(slider), 25);
    EXPECT_EQ(lh_entity_knob_to_percent(knob), 25);
    EXPECT_EQ(lh_entity_spin_to_percent(spin), 25);
    EXPECT_EQ(lh_entity_scroll_to_percent(scroll), 25);

    lh_entity_slider_set_from_percent(slider, 75);
    EXPECT_EQ(lh_entity_slider_get_value(slider), 150);

    lh_entity_slider_set_start(slider, 10);
    EXPECT_EQ(lh_entity_slider_get_start(slider), 10);
    EXPECT_EQ(lh_entity_slider_get_value(slider), 10); // setting the start moves here
    lh_entity_slider_set_value(slider, 90);
    lh_entity_slider_reset(slider);
    EXPECT_EQ(lh_entity_slider_get_value(slider), 10);

    lh_entity_scroll_set_thickness(scroll, 6);
    EXPECT_EQ(lh_entity_scroll_get_thickness(scroll), 6);
    EXPECT_EQ(lh_entity_slider_get_thickness(slider), LH_ENTITY_RANGE_THICKNESS);
}

} // namespace
