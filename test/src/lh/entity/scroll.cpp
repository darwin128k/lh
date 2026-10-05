#include <gtest/gtest.h>

#include <lh/entity/event.h>
#include <lh/entity/scroll.h>
#include <lh/entity/screen.h>
#include <lh/entity/view.h>
#include <lh/math/rect.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/null.h>
#include <lh/self.h>

#include <cstdlib>

namespace
{

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
scroll_test_alloc(lh_self_ptr, lh_usize_t size)
{
    return std::malloc(static_cast<std::size_t>(size));
}

lh_void
scroll_test_dealloc(lh_self_ptr, lh_ptr ptr)
{
    std::free(ptr);
}

LH_COMPILER_EXTERN_C_END

/* A bar of the given box, standing on a screen so that it has a root and can
   be invalidated like a real one. The box is the size, so the axis and the
   geometry below agree about which way it runs. */
class Scroll : public ::testing::Test
{
  protected:
    void
    SetUp() override
    {
        sized = lh_memory_allocator_initializer_with_context(scroll_test_alloc, scroll_test_dealloc,
                                                             lh_null, lh_null);
        screen = reinterpret_cast<lh_entity_screen_t *>(
            lh_entity_create_root(&lh_entity_screen_class, &sized));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(screen),
                              lh_math_vec2_make(400, 400));
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

    lh_entity_scroll_t *
    make_bar(lh_int_t w, lh_int_t h)
    {
        lh_entity_scroll_t *bar = reinterpret_cast<lh_entity_scroll_t *>(
            lh_entity_create(&lh_entity_scroll_class, root()));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(bar), lh_math_vec2_make(w, h));
        return bar;
    }

    /* The box of a track, as a pointer the geometry takes. A bar of the size
       above and a track of this box are the same bar on screen. */
    lh_math_rect_t *
    box(lh_int_t w, lh_int_t h)
    {
        bounds = lh_math_rect_make(0, 0, w, h);
        return lh_addr_of(bounds);
    }

    lh_int_t
    thumb_width(lh_entity_scroll_t *bar, lh_int_t w, lh_int_t h)
    {
        const lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(w, h));
        return lh_math_rect_get_size_width(&thumb);
    }

    lh_int_t
    thumb_height(lh_entity_scroll_t *bar, lh_int_t w, lh_int_t h)
    {
        const lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(w, h));
        return lh_math_rect_get_size_height(&thumb);
    }

    /* The thumb of a bar over a track of that box, as a pointer a test can ask
       about. The same record the width and height above read, kept so that a
       test can take an address of it. */
    lh_math_rect_t *
    thumb_of(lh_entity_scroll_t *bar, lh_int_t w, lh_int_t h)
    {
        thumb = lh_entity_scroll_thumb(bar, box(w, h));
        return lh_addr_of(thumb);
    }

    lh_memory_allocator_t sized;
    lh_entity_screen_t *screen = nullptr;
    lh_math_rect_t bounds = lh_math_rect_make_empty();
    lh_math_rect_t thumb = lh_math_rect_make_empty();
};

TEST_F(Scroll, a_new_bar_says_nothing_about_how_it_should_look)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);

    EXPECT_EQ(lh_entity_scroll_get_mode(bar), LH_ENTITY_SCROLL_SHOW_AUTO);
    EXPECT_EQ(lh_entity_scroll_get_thumb_min(bar), LH_ENTITY_SCROLL_THUMB_MIN);
    EXPECT_EQ(lh_entity_scroll_get_thumb_inset(bar), LH_ENTITY_SCROLL_INSET);
    EXPECT_EQ(lh_entity_scroll_get_page(bar), 10);
    EXPECT_EQ(lh_entity_scroll_get_thumb(bar), nullptr); // the track's text color
    // Ends apart, so a fresh bar is a bar that is needed.
    EXPECT_EQ(lh_entity_scroll_is_needed(bar), lh_bool_true);
}

TEST_F(Scroll, the_thumb_is_the_pages_share_of_the_content)
{
    // 300 of travel, 100 on screen, so the content is 400 and the thumb is a
    // quarter of the track: 50 of 200.
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_set_ends(lh_entity_scroll_get_range(bar), 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    const lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(12, 200));
    EXPECT_EQ(lh_math_rect_get_size_width(&thumb), 8);  // 12 less the inset
    EXPECT_EQ(lh_math_rect_get_size_height(&thumb), 50); // a quarter of 200
    EXPECT_EQ(lh_math_rect_get_y(&thumb), 0);           // the value is at the start
}

TEST_F(Scroll, the_thumb_travels_the_rest_of_the_track)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    // The value is where the offset is, and the offset maps onto the 150 the
    // thumb's own origin has left to move in.
    lh_entity_range_set_value(range, 150);
    lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(12, 200));
    EXPECT_EQ(lh_math_rect_get_y(&thumb), 75); // half way along that 150
    EXPECT_EQ(lh_math_rect_get_y(&thumb) + lh_math_rect_get_size_height(&thumb), 125);

    lh_entity_range_set_value(range, 300); // the end of it
    thumb = lh_entity_scroll_thumb(bar, box(12, 200));
    EXPECT_EQ(lh_math_rect_get_y(&thumb) + lh_math_rect_get_size_height(&thumb), 200);
}

TEST_F(Scroll, a_bar_with_nothing_to_move_shows_the_whole_track)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_set_ends(lh_entity_scroll_get_range(bar), 0, 0);

    EXPECT_EQ(lh_entity_scroll_is_needed(bar), lh_bool_false);
    const lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(12, 200));
    EXPECT_EQ(lh_math_rect_get_size_height(&thumb), 200);
}

TEST_F(Scroll, the_thumb_stops_at_its_floor_and_at_the_track)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);

    // A content nine times the track gives a thumb of two pixels, which is
    // there to be seen rather than caught.
    lh_entity_range_set_ends(range, 0, 9000);
    lh_entity_scroll_set_page(bar, 100);
    EXPECT_EQ(thumb_height(bar, 12, 200),
              LH_ENTITY_SCROLL_THUMB_MIN);

    // The floor is a setting, not a constant.
    lh_entity_scroll_set_thumb_min(bar, 40);
    EXPECT_EQ(lh_entity_scroll_get_thumb_min(bar), 40);
    EXPECT_EQ(thumb_height(bar, 12, 200), 40);

    // A page that covers the travel and most of the rest gives a thumb of all
    // but the last couple of pixels, and that is the most a thumb ever is: the
    // page can never be more than the whole content.
    lh_entity_scroll_set_thumb_min(bar, 0);
    lh_entity_range_set_ends(range, 0, 10);
    lh_entity_scroll_set_page(bar, 1000);
    EXPECT_EQ(thumb_height(bar, 12, 200), 198);
    // With no travel at all it is the whole track, which is the other test.
    lh_entity_range_set_ends(range, 0, 0);
    EXPECT_EQ(thumb_height(bar, 12, 200), 200);
}

TEST_F(Scroll, the_inset_never_eats_more_than_a_quarter_of_the_track)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_set_ends(lh_entity_scroll_get_range(bar), 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    EXPECT_EQ(thumb_width(bar, 12, 200), 8);

    // A thin track keeps a thumb to show: 2 of the 6 pixels is all the inset a
    // track that thin may spend, whatever the bar is told.
    lh_entity_scroll_set_thumb_inset(bar, 9);
    EXPECT_EQ(lh_entity_scroll_get_thumb_inset(bar), 9);
    EXPECT_EQ(thumb_width(bar, 6, 200), 4);
    EXPECT_EQ(thumb_width(bar, 2, 200), 2);

    // And a negative inset is nothing, not a thumb outside its track.
    lh_entity_scroll_set_thumb_inset(bar, -4);
    EXPECT_EQ(lh_entity_scroll_get_thumb_inset(bar), 0);
    EXPECT_EQ(thumb_width(bar, 12, 200), 12);
}

TEST_F(Scroll, the_bar_runs_the_way_its_axis_says_and_not_the_way_its_box_looks)
{
    // A wide box, declared vertical. Nothing about the shape of the track gets
    // a say, so the length is the height and the thumb is a tall thin thing.
    lh_entity_scroll_t *bar = make_bar(200, 12);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_axis(range, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);
    lh_entity_scroll_set_thumb_min(bar, 0);

    lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(200, 12));
    EXPECT_EQ(lh_math_rect_get_size_width(&thumb), 196); // the thick way
    EXPECT_EQ(lh_math_rect_get_size_height(&thumb), 3);  // 100 of 400, of 12
    EXPECT_EQ(lh_math_rect_get_x(&thumb), 2);

    // The very same bar, undecided, reads the other way round.
    lh_entity_range_set_axis(range, LH_ENTITY_RANGE_AXIS_AUTO);
    thumb = lh_entity_scroll_thumb(bar, box(200, 12));
    EXPECT_EQ(lh_math_rect_get_size_height(&thumb), 8); // 12 less the inset
    EXPECT_EQ(lh_math_rect_get_size_width(&thumb), 50); // a quarter of 200
}

TEST_F(Scroll, a_horizontal_thumb_lies_along_the_width)
{
    lh_entity_scroll_t *bar = make_bar(200, 12);
    lh_entity_range_set_axis(lh_entity_scroll_get_range(bar), LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    lh_entity_range_set_ends(lh_entity_scroll_get_range(bar), 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    const lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(200, 12));
    EXPECT_EQ(lh_math_rect_get_size_width(&thumb), 50);
    EXPECT_EQ(lh_math_rect_get_size_height(&thumb), 8);
    EXPECT_EQ(lh_math_rect_get_y(&thumb), 2);
}

TEST_F(Scroll, auto_shows_only_a_bar_with_something_to_move)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_t *entity = reinterpret_cast<lh_entity_t *>(bar);

    lh_entity_scroll_set_mode(bar, LH_ENTITY_SCROLL_SHOW_AUTO);
    EXPECT_EQ(lh_entity_has_flags(entity, lh_entity_flags_hidden), lh_bool_false);

    lh_entity_range_set_ends(lh_entity_scroll_get_range(bar), 0, 0);
    lh_entity_scroll_update_mode(bar);
    EXPECT_EQ(lh_entity_has_flags(entity, lh_entity_flags_hidden), lh_bool_true);

    // Travel comes back and the bar comes back with it.
    lh_entity_range_set_ends(lh_entity_scroll_get_range(bar), 0, 300);
    lh_entity_scroll_update_mode(bar);
    EXPECT_EQ(lh_entity_has_flags(entity, lh_entity_flags_hidden), lh_bool_false);
}

TEST_F(Scroll, always_keeps_a_bar_with_nothing_to_move_and_never_takes_one_away)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_t *entity = reinterpret_cast<lh_entity_t *>(bar);
    lh_entity_range_set_ends(lh_entity_scroll_get_range(bar), 0, 0);

    lh_entity_scroll_set_mode(bar, LH_ENTITY_SCROLL_SHOW_ALWAYS);
    EXPECT_EQ(lh_entity_has_flags(entity, lh_entity_flags_hidden), lh_bool_false);
    // A bar over nothing is the whole track, not a thumb at the top of it.
    EXPECT_EQ(thumb_height(bar, 12, 200), 200);

    lh_entity_range_set_ends(lh_entity_scroll_get_range(bar), 0, 300);
    lh_entity_scroll_set_mode(bar, LH_ENTITY_SCROLL_SHOW_NEVER);
    EXPECT_EQ(lh_entity_has_flags(entity, lh_entity_flags_hidden), lh_bool_true);
    // The ends still work while it is away, so a value can be set on a bar
    // nobody can see.
    lh_entity_range_set_value(lh_entity_scroll_get_range(bar), 100);
    EXPECT_EQ(lh_entity_range_get_value(lh_entity_scroll_get_range(bar)), 100);
}

TEST_F(Scroll, a_mode_that_is_none_of_the_three_is_the_undecided_one)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_scroll_set_mode(bar, LH_ENTITY_SCROLL_SHOW_NEVER);
    EXPECT_EQ(lh_entity_scroll_get_mode(bar), LH_ENTITY_SCROLL_SHOW_NEVER);

    lh_entity_scroll_set_mode(bar, 42);
    EXPECT_EQ(lh_entity_scroll_get_mode(bar), LH_ENTITY_SCROLL_SHOW_AUTO);
}

TEST_F(Scroll, the_thumb_ride_under_the_pointer_it_was_grabbed_by)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    // A press on the middle of the thumb takes hold of it and moves nothing: the
    // thumb is already where the value says, and a grab is not a jump.
    const lh_math_vec2_t middle = lh_math_vec2_make(6, 25);
    EXPECT_EQ(lh_entity_scroll_press(bar, &middle), lh_bool_true);
    EXPECT_EQ(lh_entity_range_get_value(range), 0);

    // The pointer at 100 down a 200 track. The grabbed middle is what goes to
    // 100, not the thumb's origin: a grab does not throw the thumb to wherever
    // the value alone would put it.
    const lh_math_vec2_t point = lh_math_vec2_make(6, 100);
    lh_entity_scroll_apply(bar, &point);

    const lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(12, 200));
    EXPECT_EQ(lh_math_rect_get_y(&thumb) + lh_math_rect_get_size_height(&thumb) / 2, 100);
    EXPECT_EQ(lh_entity_range_get_value(range), 150); // half of the travel
}

TEST_F(Scroll, the_point_of_the_thumb_that_was_pressed_is_the_one_that_comes_with_the_pointer)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    // 5 down a thumb that starts at 0, so 5 from its origin and not its half.
    // Throwing the middle away for the origin would put the thumb itself at the
    // pointer, and the edge that was taken hold of would have jumped 20.
    const lh_math_vec2_t edge = lh_math_vec2_make(6, 5);
    EXPECT_EQ(lh_entity_scroll_press(bar, &edge), lh_bool_true);

    const lh_math_vec2_t point = lh_math_vec2_make(6, 100);
    lh_entity_scroll_apply(bar, &point);

    const lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(12, 200));
    EXPECT_EQ(lh_math_rect_get_y(&thumb), 95);     // 190 of the value
    EXPECT_EQ(lh_math_rect_get_y(&thumb) + 5, 100); // still 5 in, where it was taken
    EXPECT_EQ(lh_entity_range_get_value(range), 190);
}

TEST_F(Scroll, a_press_on_the_track_turns_a_page_towards_the_pointer)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    // The thumb is the first 50 of the track. Below it is forward, and a page is
    // 100 of the content, so the value goes to 100 and the thumb to the middle
    // of the room it has left to move in.
    const lh_math_vec2_t below = lh_math_vec2_make(6, 150);
    EXPECT_EQ(lh_entity_scroll_press(bar, &below), lh_bool_false);
    EXPECT_EQ(lh_entity_range_get_value(range), 100);
    EXPECT_EQ(lh_math_rect_get_y(thumb_of(bar, 12, 200)), 50);

    // Above it is back, and the thumb has moved with the value, so the same
    // point 10 down is above the thumb again.
    const lh_math_vec2_t above = lh_math_vec2_make(6, 10);
    EXPECT_EQ(lh_entity_scroll_press(bar, &above), lh_bool_false);
    EXPECT_EQ(lh_entity_range_get_value(range), 0);
    EXPECT_EQ(lh_math_rect_get_y(thumb_of(bar, 12, 200)), 0);

    // The same rule at the far end: a value at the bottom of the travel puts the
    // thumb at the bottom of the track, and a press above it pages back.
    lh_entity_range_set_value(range, 300);
    const lh_math_vec2_t from_the_end = lh_math_vec2_make(6, 10);
    EXPECT_EQ(lh_entity_scroll_press(bar, &from_the_end), lh_bool_false);
    EXPECT_EQ(lh_entity_range_get_value(range), 200);
}

TEST_F(Scroll, a_press_on_the_track_leaves_the_move_after_it_alone)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_t *const entity = reinterpret_cast<lh_entity_t *>(bar);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    // A press on the track turns a page and takes hold of nothing, so a move
    // after it is the user's and not a drag. A bar that dragged here would throw
    // the thumb under a pointer that never took hold of it.
    lh_math_vec2_t track = lh_math_vec2_make(6, 150);
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_DOWN, &track);
    EXPECT_EQ(lh_entity_range_get_value(range), 100);

    lh_math_vec2_t far = lh_math_vec2_make(6, 199);
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_MOVE, &far);
    EXPECT_EQ(lh_entity_range_get_value(range), 100);
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_UP, &far);

    // A press on the thumb is the other thing, and the move after it drags. The
    // thumb is 50 to 100 now, so 75 is on it and 25 from where it started.
    lh_math_vec2_t on_thumb = lh_math_vec2_make(6, 75);
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_DOWN, &on_thumb);
    EXPECT_EQ(lh_entity_range_get_value(range), 100);
    lh_math_vec2_t dragged = lh_math_vec2_make(6, 125);
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_MOVE, &dragged);
    EXPECT_EQ(lh_entity_range_get_value(range), 200);
}

TEST_F(Scroll, a_hold_that_was_taken_away_lets_the_thumb_go)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_t *const entity = reinterpret_cast<lh_entity_t *>(bar);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    // Take hold of the thumb, which is the first 50 down the track, and drag it
    // to where the middle of the bar would put it.
    lh_math_vec2_t on_thumb = lh_math_vec2_make(6, 25);
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_DOWN, &on_thumb);
    lh_math_vec2_t dragged = lh_math_vec2_make(6, 100);
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_MOVE, &dragged);
    ASSERT_EQ(lh_entity_range_get_value(range), 150);

    // The hold is taken away rather than let go: the thumb is let go where the
    // drag had got to, and nothing moves it after that. A bar still being
    // dragged with no pointer under it is the one thing a cancel has to stop,
    // and the value is left alone because a hold that was taken away never
    // reached anywhere else.
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_CANCEL, lh_null);
    lh_math_vec2_t after = lh_math_vec2_make(6, 180);
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_MOVE, &after);
    EXPECT_EQ(lh_entity_range_get_value(range), 150);
    // And a release after it is not a second ending of the same hold.
    lh_entity_send_event(entity, LH_ENTITY_EVENT_POINTER_UP, &after);
    EXPECT_EQ(lh_entity_range_get_value(range), 150);
}

TEST_F(Scroll, a_page_is_the_page_and_stops_at_the_ends)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    lh_entity_scroll_page_by(bar, 1);
    EXPECT_EQ(lh_entity_range_get_value(range), 100);
    lh_entity_scroll_page_by(bar, -1);
    EXPECT_EQ(lh_entity_range_get_value(range), 0);
    // None of a page is none.
    EXPECT_EQ(lh_entity_scroll_set_value(bar, lh_entity_range_get_value(range)), lh_bool_false);
    lh_entity_scroll_page_by(bar, 0);
    EXPECT_EQ(lh_entity_range_get_value(range), 0);

    // Three pages forward off a content of 300 is the end of it, not 400.
    lh_entity_scroll_page_by(bar, 3);
    EXPECT_EQ(lh_entity_range_get_value(range), 300);
    lh_entity_scroll_page_by(bar, 1);
    EXPECT_EQ(lh_entity_range_get_value(range), 300);
    // And a count of pages that would run over the end of a number lands on the
    // end anyway.
    lh_entity_scroll_page_by(bar, 1000000);
    EXPECT_EQ(lh_entity_range_get_value(range), 300);
    lh_entity_scroll_page_by(bar, -1000000);
    EXPECT_EQ(lh_entity_range_get_value(range), 0);
}

TEST_F(Scroll, a_value_set_by_hand_says_so_and_a_value_that_did_not_move_does_not)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_ends(range, 0, 300);

    EXPECT_EQ(lh_entity_scroll_set_value(bar, 120), lh_bool_true);
    EXPECT_EQ(lh_entity_range_get_value(range), 120);
    // The same number twice is one move, not two.
    EXPECT_EQ(lh_entity_scroll_set_value(bar, 120), lh_bool_false);
    // Past the end is the end, and that is a move the first time and not the
    // second.
    EXPECT_EQ(lh_entity_scroll_set_value(bar, 900), lh_bool_true);
    EXPECT_EQ(lh_entity_range_get_value(range), 300);
    EXPECT_EQ(lh_entity_scroll_set_value(bar, 900), lh_bool_false);
}

TEST_F(Scroll, a_horizontal_bar_pages_along_its_own_way)
{
    lh_entity_scroll_t *bar = make_bar(200, 12);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_axis(range, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    // The thumb is the first 50 along. 150 along is forward whatever the y is,
    // and 10 along is back.
    const lh_math_vec2_t forward = lh_math_vec2_make(150, 0);
    EXPECT_EQ(lh_entity_scroll_press(bar, &forward), lh_bool_false);
    EXPECT_EQ(lh_entity_range_get_value(range), 100);
    EXPECT_EQ(lh_math_rect_get_x(thumb_of(bar, 200, 12)), 50);

    const lh_math_vec2_t back = lh_math_vec2_make(10, 0);
    EXPECT_EQ(lh_entity_scroll_press(bar, &back), lh_bool_false);
    EXPECT_EQ(lh_entity_range_get_value(range), 0);

    // Turned on its side, the very same point is a different place: the x of 150
    // means nothing to a bar that runs top to bottom, and its own y of 0 is the
    // top of the track, so the press lands on the thumb and holds it.
    lh_entity_range_set_axis(range, LH_ENTITY_RANGE_AXIS_VERTICAL);
    const lh_math_vec2_t down = lh_math_vec2_make(150, 0);
    EXPECT_EQ(lh_entity_scroll_press(bar, &down), lh_bool_true);
    EXPECT_EQ(lh_entity_range_get_value(range), 0);

    // And the other way up the y of a point is not read at all: a press from
    // nowhere near the box still pages the bar, because the only coordinate that
    // says anything to a horizontal one is the x.
    lh_entity_range_set_axis(range, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    const lh_math_vec2_t far_off = lh_math_vec2_make(150, 9999);
    EXPECT_EQ(lh_entity_scroll_press(bar, &far_off), lh_bool_false);
    EXPECT_EQ(lh_entity_range_get_value(range), 100);
}

TEST_F(Scroll, a_drag_past_the_ends_stops_at_them)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    const lh_math_vec2_t past = lh_math_vec2_make(6, 900);
    lh_entity_scroll_apply(bar, &past);
    EXPECT_EQ(lh_entity_range_get_value(range), 300);
    const lh_math_rect_t thumb = lh_entity_scroll_thumb(bar, box(12, 200));
    EXPECT_EQ(lh_math_rect_get_y(&thumb) + lh_math_rect_get_size_height(&thumb), 200);

    const lh_math_vec2_t before = lh_math_vec2_make(6, -400);
    lh_entity_scroll_apply(bar, &before);
    EXPECT_EQ(lh_entity_range_get_value(range), 0);
    lh_math_rect_t back = lh_entity_scroll_thumb(bar, box(12, 200));
    EXPECT_EQ(lh_math_rect_get_y(&back), 0);
}

TEST_F(Scroll, a_horizontal_drag_reads_the_x_and_not_the_y)
{
    lh_entity_scroll_t *bar = make_bar(200, 12);
    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    lh_entity_range_set_axis(range, LH_ENTITY_RANGE_AXIS_VERTICAL); // and back again
    lh_entity_range_set_axis(range, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    lh_entity_range_set_ends(range, 0, 300);
    lh_entity_scroll_set_page(bar, 100);

    // A drag with no press behind it puts the thumb's own origin under the
    // pointer, which is the same rule with nothing taken hold of: 100 along a
    // 200 wide track is 100 of the 150 the thumb has left to move in, so the
    // value is 200 of the travel and the origin is where the pointer is. The y
    // of the point is 0 and is not read at all.
    const lh_math_vec2_t point = lh_math_vec2_make(100, 0);
    lh_entity_scroll_apply(bar, &point);
    EXPECT_EQ(lh_entity_range_get_value(range), 200);
    EXPECT_EQ(lh_math_rect_get_x(thumb_of(bar, 200, 12)), 100);

    // Turned on its side, the very same point is a different place: the x of
    // 100 means nothing to a bar that runs top to bottom, and its own y of 0
    // is the top of the track.
    lh_entity_range_set_axis(range, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_range_set_value(range, 0);
    lh_entity_scroll_apply(bar, &point);
    EXPECT_EQ(lh_entity_range_get_value(range), 0);
}

TEST_F(Scroll, the_range_a_bar_hands_back_is_the_bars_own_record)
{
    lh_entity_scroll_t *bar = make_bar(12, 200);
    lh_entity_range_t *first = lh_entity_scroll_get_range(bar);

    EXPECT_EQ(first, lh_addr_of(bar->range));
    EXPECT_EQ(first, lh_entity_scroll_get_range(bar));
    // So the axis, the ends and the value are all the range's own API and the
    // bar adds nothing to them.
    lh_entity_range_set_axis(first, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_range_set_ends(first, 0, 500);
    EXPECT_EQ(lh_entity_range_get_maximum(first), 500);
    EXPECT_EQ(lh_entity_range_is_vertical(first), lh_bool_true);
}

} // namespace
