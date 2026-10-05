#include <gtest/gtest.h>

#include <lh/entity/2d.h>
#include <lh/entity/screen.h>
#include <lh/entity/scroll.h>
#include <lh/entity/view.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/null.h>
#include <lh/self.h>

#include <cstdlib>

namespace
{

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
view_test_alloc(lh_self_ptr, lh_usize_t size)
{
    return std::malloc(static_cast<std::size_t>(size));
}

lh_void
view_test_dealloc(lh_self_ptr, lh_ptr ptr)
{
    std::free(ptr);
}

LH_COMPILER_EXTERN_C_END

/* A window onto a page, both on a real screen, so that the bars can be linked
   and asked what they think the travel is. */
class View : public ::testing::Test
{
  protected:
    void
    SetUp() override
    {
        sized = lh_memory_allocator_initializer_with_context(view_test_alloc, view_test_dealloc,
                                                             lh_null, lh_null);
        screen = reinterpret_cast<lh_entity_screen_t *>(
            lh_entity_create_root(&lh_entity_screen_class, &sized));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(screen),
                              lh_math_vec2_make(800, 600));
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

    lh_entity_view_t *
    make_view(lh_int_t w, lh_int_t h)
    {
        lh_entity_view_t *view = reinterpret_cast<lh_entity_view_t *>(
            lh_entity_create(&lh_entity_view_class, root()));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(view),
                              lh_math_vec2_make(w, h));
        return view;
    }

    lh_entity_2d_t *
    make_page(lh_entity_view_t *view, lh_int_t w, lh_int_t h)
    {
        lh_entity_2d_t *page = reinterpret_cast<lh_entity_2d_t *>(
            lh_entity_create(lh_addr_of(lh_entity_2d_class),
                             reinterpret_cast<lh_entity_t *>(view)));
        lh_entity_2d_set_size(page, lh_math_vec2_make(w, h));
        lh_entity_view_set_content(view, reinterpret_cast<lh_entity_t *>(page));
        return page;
    }

    lh_entity_scroll_t *
    make_bar(lh_int_t w, lh_int_t h, lh_int_t axis)
    {
        lh_entity_scroll_t *bar = reinterpret_cast<lh_entity_scroll_t *>(
            lh_entity_create(&lh_entity_scroll_class, root()));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(bar), lh_math_vec2_make(w, h));
        lh_entity_range_set_axis(lh_entity_scroll_get_range(bar), axis);
        return bar;
    }

    lh_math_vec2_t
    page_position(lh_entity_view_t *view) const
    {
        return lh_entity_2d_get_position(reinterpret_cast<lh_entity_2d_t *>(
            reinterpret_cast<lh_entity_view_t *>(view)->content));
    }

    /* A wheel over the screen at a point, by so many pixels on each side.
       Returns whatever took it, so a test can tell "this view scrolled" from
       "nobody wanted it". */
    lh_entity_t *
    wheel(lh_float_t x, lh_float_t y, lh_float_t dx, lh_float_t dy)
    {
        return lh_entity_screen_send_wheel(screen, lh_math_vec2_make(x, y),
                                           lh_math_vec2_make(dx, dy));
    }

    lh_memory_allocator_t sized;
    lh_entity_screen_t *screen = nullptr;
    lh_math_rect_t bounds = lh_math_rect_make_empty();
};

TEST_F(View, the_travel_is_what_the_content_leaves_over_the_window)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);

    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 320);
    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL), 0);
}

TEST_F(View, a_window_with_nothing_in_it_has_nowhere_to_go)
{
    lh_entity_view_t *view = make_view(340, 400);

    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL), 0);
    // And the offset cannot be anywhere but the top left.
    lh_entity_view_set_offset(view, 50, 50);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL), 0);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
}

TEST_F(View, a_content_that_fits_is_not_travel)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 300, 380);

    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL), 0);
    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
}

TEST_F(View, an_axis_asked_by_no_name_answers_with_the_long_side)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 400, 820);

    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 420);
    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL), 60);
    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_AUTO), 420);

    lh_entity_view_set_offset(view, 30, 200);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL), 30);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 200);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_AUTO), 200);
}

TEST_F(View, the_content_sits_at_the_negative_offset)
{
    lh_entity_view_t *view = make_view(340, 400);
    lh_entity_2d_t *page = make_page(view, 340, 720);

    lh_entity_view_set_offset(view, 0, 120);
    const lh_math_vec2_t at = lh_entity_2d_get_position(page);
    EXPECT_FLOAT_EQ(at.x, 0.0f);
    EXPECT_FLOAT_EQ(at.y, -120.0f);
}

TEST_F(View, an_offset_is_pulled_in_at_both_ends_and_not_only_the_low_one)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);

    // Before there was a travel, an offset past the top was the low bound and
    // an offset past the end was simply taken. Both are the same rule now.
    lh_entity_view_set_offset(view, 900, 900);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 320);
    lh_entity_view_set_offset(view, -900, -900);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL), 0);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
}

TEST_F(View, the_link_feeds_the_bar_the_travel_and_the_screenful)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);
    lh_entity_scroll_t *bar =
        make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);

    lh_entity_view_sync(view);

    lh_entity_range_t *range = lh_entity_scroll_get_range(bar);
    EXPECT_EQ(lh_entity_range_get_minimum(range), 0);
    EXPECT_EQ(lh_entity_range_get_maximum(range), 320); // the travel
    EXPECT_EQ(lh_entity_scroll_get_page(bar), 400);     // the window's own height
    EXPECT_EQ(lh_entity_scroll_is_needed(bar), lh_bool_true);
}

TEST_F(View, a_bar_on_the_other_side_is_fed_from_the_other_side)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 420, 720);
    lh_entity_scroll_t *bar = make_bar(340, 12, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    lh_entity_view_set_scrollbar(view, bar);

    lh_entity_view_sync(view);

    EXPECT_EQ(lh_entity_range_get_maximum(lh_entity_scroll_get_range(bar)), 80);
    EXPECT_EQ(lh_entity_scroll_get_page(bar), 340);
}

TEST_F(View, one_window_carries_a_bar_per_side)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);
    lh_entity_scroll_t *down = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_scroll_t *along = make_bar(340, 12, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    lh_entity_view_set_scrollbar(view, down);
    lh_entity_view_set_scrollbar(view, along);
    lh_entity_view_sync(view);

    // One goes up from 0 to 320 and the other has nothing to do at all.
    EXPECT_EQ(lh_entity_range_get_maximum(lh_entity_scroll_get_range(down)), 320);
    EXPECT_EQ(lh_entity_range_get_maximum(lh_entity_scroll_get_range(along)), 0);
    EXPECT_EQ(lh_entity_scroll_is_needed(down), lh_bool_true);
    EXPECT_EQ(lh_entity_scroll_is_needed(along), lh_bool_false);
}

TEST_F(View, a_bar_that_is_not_needed_hides_itself_and_comes_back)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);
    lh_entity_scroll_t *bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);

    lh_entity_view_sync(view);
    EXPECT_EQ(lh_entity_has_flags(reinterpret_cast<lh_entity_t *>(bar), lh_entity_flags_hidden),
              lh_bool_false);

    // The page grows tall enough to have no travel any more.
    lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(view->content),
                          lh_math_vec2_make(340, 400));
    lh_entity_view_sync(view);
    EXPECT_EQ(lh_entity_has_flags(reinterpret_cast<lh_entity_t *>(bar), lh_entity_flags_hidden),
              lh_bool_true);

    lh_entity_scroll_set_mode(bar, LH_ENTITY_SCROLL_SHOW_ALWAYS);
    EXPECT_EQ(lh_entity_has_flags(reinterpret_cast<lh_entity_t *>(bar), lh_entity_flags_hidden),
              lh_bool_false);
    lh_entity_scroll_set_mode(bar, LH_ENTITY_SCROLL_SHOW_NEVER);
    EXPECT_EQ(lh_entity_has_flags(reinterpret_cast<lh_entity_t *>(bar), lh_entity_flags_hidden),
              lh_bool_true);
}

TEST_F(View, the_offset_drives_the_bar_and_the_bar_drives_the_offset)
{
    lh_entity_view_t *view = make_view(340, 400);
    lh_entity_2d_t *page = make_page(view, 340, 720);
    lh_entity_scroll_t *bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);
    lh_entity_view_sync(view);

    // A caller that scrolls the window moves the bar with it.
    lh_entity_view_set_offset(view, 0, 200);
    EXPECT_EQ(lh_entity_range_get_value(lh_entity_scroll_get_range(bar)), 200);
    EXPECT_FLOAT_EQ(lh_entity_2d_get_position(page).y, -200.0f);

    // And a bar that moved on its own is the window's offset.
    lh_entity_range_set_value(lh_entity_scroll_get_range(bar), 60);
    lh_entity_view_sync(view);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 60);
    EXPECT_FLOAT_EQ(lh_entity_2d_get_position(page).y, -60.0f);
}

TEST_F(View, a_content_that_shrinks_pulls_the_offset_back_with_it)
{
    lh_entity_view_t *view = make_view(340, 400);
    lh_entity_2d_t *page = make_page(view, 340, 720);
    lh_entity_scroll_t *bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);
    lh_entity_view_sync(view);
    lh_entity_view_set_offset(view, 0, 300);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 300);

    // The page is edited under the window's feet.
    lh_entity_2d_set_size(page, lh_math_vec2_make(340, 450));
    lh_entity_view_sync(view);

    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 50);
    EXPECT_EQ(lh_entity_range_get_maximum(lh_entity_scroll_get_range(bar)), 50);
    EXPECT_EQ(lh_entity_range_get_value(lh_entity_scroll_get_range(bar)), 50);
    EXPECT_FLOAT_EQ(lh_entity_2d_get_position(page).y, -50.0f);
}

TEST_F(View, a_bar_in_the_slot_that_is_taken_replaces_the_one_that_was_there)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);
    lh_entity_scroll_t *first = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_scroll_t *second = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, first);
    lh_entity_view_set_scrollbar(view, second);
    lh_entity_view_sync(view);

    // A value on the bar the window has let go moves no page: the second one,
    // on the bar that is still linked, is the one that counts.
    lh_entity_range_set_value(lh_entity_scroll_get_range(first), 100);
    lh_entity_view_sync(view);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);

    lh_entity_range_set_value(lh_entity_scroll_get_range(second), 100);
    lh_entity_view_sync(view);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 100);
}

TEST_F(View, a_bar_turned_after_it_was_bound_still_reads_its_own_side)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 420, 720);
    lh_entity_scroll_t *bar = make_bar(340, 12, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    lh_entity_view_set_scrollbar(view, bar);
    // The slot says vertical, because that is how it was bound. The axis does
    // not, and the axis is what the sync asks.
    lh_entity_range_set_axis(lh_entity_scroll_get_range(bar), LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_sync(view);

    EXPECT_EQ(lh_entity_range_get_maximum(lh_entity_scroll_get_range(bar)), 320);
    EXPECT_EQ(lh_entity_scroll_get_page(bar), 400);
}

TEST_F(View, dropping_the_bars_tells_both_of_them)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 420, 720);
    lh_entity_scroll_t *down = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_scroll_t *along = make_bar(340, 12, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    lh_entity_view_set_scrollbar(view, down);
    lh_entity_view_set_scrollbar(view, along);
    lh_entity_view_sync(view);
    ASSERT_EQ(lh_entity_range_get_maximum(lh_entity_scroll_get_range(down)), 320);

    lh_entity_view_set_scrollbar(view, nullptr);
    lh_entity_view_sync(view); // nothing left to feed, and nothing to break

    // A bar still knows the numbers it was last given, so it is a bar of no
    // use rather than a broken one, and its drag moves no page.
    EXPECT_EQ(lh_entity_range_get_maximum(lh_entity_scroll_get_range(down)), 320);
    lh_math_vec2_t point = lh_math_vec2_make(6, 150);
    lh_entity_scroll_apply(down, &point);
    lh_entity_view_sync(view);
    EXPECT_NE(lh_entity_range_get_value(lh_entity_scroll_get_range(down)), 0);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
}

TEST_F(View, a_view_starts_with_no_offset_and_no_bars)
{
    lh_entity_view_t *view = make_view(340, 400);

    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
    EXPECT_EQ(lh_entity_view_get_travel(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
    // A sync with nothing attached is a no-op, not a crash.
    lh_entity_view_sync(view);
    lh_entity_view_set_scrollbar(view, nullptr);
}

TEST_F(View, a_wheel_over_a_view_scrolls_it_and_its_bar)
{
    lh_entity_view_t *view = make_view(340, 400);
    lh_entity_2d_t *page = make_page(view, 340, 720);
    lh_entity_scroll_t *bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);
    lh_entity_view_sync(view);

    // 40 pixels on, which is one notch of an ordinary wheel.
    EXPECT_EQ(wheel(100, 100, 0, 40), reinterpret_cast<lh_entity_t *>(view));
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 40);
    EXPECT_EQ(lh_entity_range_get_value(lh_entity_scroll_get_range(bar)), 40);
    EXPECT_FLOAT_EQ(lh_entity_2d_get_position(page).y, -40.0f);

    // And back again, so the same number is read back the other way.
    wheel(100, 100, 0, -40);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
}

TEST_F(View, a_wheel_comes_through_a_control_that_is_under_the_pointer)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);
    lh_entity_scroll_t *bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);
    lh_entity_view_sync(view);

    // A button over the page, with no bubble flag and no idea a wheel exists.
    lh_entity_2d_t *control = reinterpret_cast<lh_entity_2d_t *>(
        lh_entity_create(lh_addr_of(lh_entity_2d_class),
                         reinterpret_cast<lh_entity_t *>(view->content)));
    lh_entity_2d_set_size(control, lh_math_vec2_make(100, 40));

    // The wheel belongs to the place, not to a target, so the control does not
    // swallow it and neither does the page.
    EXPECT_EQ(wheel(50, 20, 0, 40), reinterpret_cast<lh_entity_t *>(view));
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 40);
}

TEST_F(View, a_wheel_a_view_cannot_use_goes_to_the_one_that_can)
{
    lh_entity_view_t *outer = make_view(600, 400);
    lh_entity_2d_t *outer_page = make_page(outer, 600, 900);
    lh_entity_scroll_t *outer_bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(outer, outer_bar);
    lh_entity_view_sync(outer);

    // A smaller window inside it, scrolled to the top of its own content.
    lh_entity_view_t *inner =
        reinterpret_cast<lh_entity_view_t *>(lh_entity_create(&lh_entity_view_class,
                                                              reinterpret_cast<lh_entity_t *>(
                                                                  outer_page)));
    lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(inner), lh_math_vec2_make(200, 200));
    make_page(inner, 200, 300);
    lh_entity_scroll_t *inner_bar = make_bar(12, 200, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(inner, inner_bar);
    lh_entity_view_sync(inner);
    ASSERT_EQ(lh_entity_view_get_offset(inner, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
    // The outer window has already been scrolled on, so it has room back.
    lh_entity_view_set_offset(outer, 0, 100);

    // A wheel back is one the inner window cannot use, being at the top of its
    // own content, and the outer one has room, so the outer one takes it.
    EXPECT_EQ(wheel(50, 50, 0, -40), reinterpret_cast<lh_entity_t *>(outer));
    EXPECT_EQ(lh_entity_view_get_offset(outer, LH_ENTITY_RANGE_AXIS_VERTICAL), 60);
    EXPECT_EQ(lh_entity_view_get_offset(inner, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
    EXPECT_FLOAT_EQ(lh_entity_2d_get_position(outer_page).y, -60.0f);
}

TEST_F(View, a_wheel_with_nowhere_to_go_is_taken_by_nobody)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);
    lh_entity_scroll_t *bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);
    lh_entity_view_sync(view);

    // At the top already, so a wheel back is not this view's to take and the
    // search goes on up and finds nothing.
    EXPECT_EQ(wheel(100, 100, 0, -40), nullptr);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
    // Forward is fine, and then back again is at the top.
    wheel(100, 100, 0, 40);
    EXPECT_EQ(wheel(100, 100, 0, -40), reinterpret_cast<lh_entity_t *>(view));
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
}

TEST_F(View, a_side_with_no_bar_is_not_a_side_the_wheel_scrolls)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 420, 720); // the content is wider than the window
    lh_entity_scroll_t *bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);
    lh_entity_view_sync(view);

    // There is room sideways, and no bar to say so, so sideways is not a side
    // this view scrolls: the content is wider than the window and the view
    // says so, rather than the wheel finding a number nobody looked for.
    EXPECT_EQ(wheel(100, 100, 40, 0), nullptr);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL), 0);
    // The side it does have a bar for is a side it scrolls.
    EXPECT_EQ(wheel(100, 100, 0, 40), reinterpret_cast<lh_entity_t *>(view));
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 40);
}

TEST_F(View, a_view_with_no_bar_at_all_ignores_the_wheel)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);

    EXPECT_EQ(wheel(100, 100, 0, -40), nullptr);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
}

TEST_F(View, a_wheel_moves_in_whole_pixels_and_keeps_the_rest)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 340, 720);
    lh_entity_scroll_t *bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);
    lh_entity_view_sync(view);

    // A touchpad sends less than a pixel at a time. Rounding each one away
    // would be a drag that never moves, so the fractions wait for each other,
    // and a wheel that has not made a whole pixel yet is taken by nobody.
    EXPECT_EQ(wheel(100, 100, 0, 0.5f), nullptr);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
    EXPECT_EQ(wheel(100, 100, 0, 0.5f), reinterpret_cast<lh_entity_t *>(view));
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 1);
    wheel(100, 100, 0, 0.5f);
    wheel(100, 100, 0, 0.5f);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 2);
    // The bar is told the whole pixels, and the view keeps the half.
    EXPECT_EQ(lh_entity_range_get_value(lh_entity_scroll_get_range(bar)), 2);
}

TEST_F(View, a_wheel_on_both_sides_moves_both)
{
    lh_entity_view_t *view = make_view(340, 400);
    make_page(view, 420, 720);
    lh_entity_scroll_t *down = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_scroll_t *along = make_bar(340, 12, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    lh_entity_view_set_scrollbar(view, down);
    lh_entity_view_set_scrollbar(view, along);
    lh_entity_view_sync(view);

    // A corner of a trackpad can turn both ways at once, and one number must
    // not be spent on the other axis.
    wheel(100, 100, 30, 40);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL), 30);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 40);
    EXPECT_EQ(lh_entity_range_get_value(lh_entity_scroll_get_range(down)), 40);
    EXPECT_EQ(lh_entity_range_get_value(lh_entity_scroll_get_range(along)), 30);
}

TEST_F(View, a_value_written_on_a_bar_reaches_the_view_through_the_bar_own_door)
{
    lh_entity_view_t *view = make_view(340, 400);
    lh_entity_2d_t *page = make_page(view, 340, 720);
    lh_entity_scroll_t *bar = make_bar(12, 400, LH_ENTITY_RANGE_AXIS_VERTICAL);
    lh_entity_view_set_scrollbar(view, bar);
    lh_entity_view_sync(view);

    // The quiet way in moves the number and tells nobody, so a caller writing
    // the range by hand is talking to itself.
    lh_entity_range_set_value(lh_entity_scroll_get_range(bar), 100);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);

    // The bar's own door is the one a drag and a page and a caller all come
    // through, and it takes the view along.
    EXPECT_EQ(lh_entity_scroll_set_value(bar, 40), lh_bool_true);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 40);
    EXPECT_FLOAT_EQ(lh_entity_2d_get_position(page).y, -40.0f);

    // A page is the view's own screenful, which is the one size a bar cannot
    // know by itself, so a page back off 40 is the start of the content.
    EXPECT_EQ(lh_entity_scroll_get_page(bar), 400);
    lh_entity_scroll_page_by(bar, -1);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 0);
    // And a page forward from there is the whole window, held at the end of the
    // content rather than run past it.
    lh_entity_scroll_page_by(bar, 1);
    EXPECT_EQ(lh_entity_view_get_offset(view, LH_ENTITY_RANGE_AXIS_VERTICAL), 320);
}

} // namespace
