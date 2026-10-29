#include <gtest/gtest.h>

#include <lh/os/app.h>
#include <lh/os/system/window.h>
#include <lh/os/tick.h>
#include <lh/os/window.h>

#if !defined(_WIN32)
#    include <cstdlib>
#endif

/* A test that opens a real window needs a screen to open it on. Windows always has
   one; Linux has one when $DISPLAY names an X server, and a CI machine has none --
   there the test is skipped, not failed: no window is what a headless machine is. */
static bool
lh_test_has_display()
{
#if defined(_WIN32)
    return true;
#else
    const char *display = std::getenv("DISPLAY");

    return display != nullptr && lh_ptr_deref(display) != '\0';
#endif
}

#define LH_TEST_NEEDS_DISPLAY()                                                                    \
    if (!lh_test_has_display())                                                                    \
    GTEST_SKIP() << "no display to open a window on"

namespace
{

struct ZoneLog
{
    int count;
    int x;
    int y;
    lh_os_window_zone_t last;
    int top;
};

/* A frame of the caller's own: the top ::top rows are the caption, and a box in the
   middle of it is the caller's own button, which must stay its own. */
lh_os_window_zone_t
on_zone(lh_os_window_t * /*self*/, int x, int y, lh_ptr context)
{
    auto *log = static_cast<ZoneLog *>(context);

    log->count++;
    log->x = x;
    log->y = y;
    if (y < log->top && !(x >= 100 && x < 130 && y >= 4 && y < 28))
    {
        log->last = lh_os_window_zone_caption;
    }
    else
    {
        log->last = lh_os_window_zone_client;
    }
    return log->last;
}

struct CloseLog
{
    int count;
    lh_os_window_close_reason_t reason;
};

lh_void
on_close(lh_os_window_t * /*self*/, lh_os_window_close_reason_t reason, lh_ptr context)
{
    auto *log = static_cast<CloseLog *>(context);

    if (log == nullptr)
    {
        ADD_FAILURE() << "null close log";
        return;
    }
    log->count++;
    log->reason = reason;
}

struct InputLog
{
    int count;
    lh_os_input_kind_t kind;
    lh_os_input_button_t button;
    lh_os_input_coord_t x;
    lh_os_input_coord_t y;
    lh_u32_t buttons_seen_by_window;
};

/* The window is the context too, so the sink can read what the window already
   believes at the moment an event reaches it. */
struct InputSink
{
    InputLog log;
    lh_os_window_t *window;
};

lh_void
on_input(lh_self_ptr self, const lh_os_input_event_ptr event)
{
    auto *sink = static_cast<InputSink *>(self);

    if (sink == nullptr)
    {
        ADD_FAILURE() << "null input sink";
        return;
    }
    sink->log.count++;
    sink->log.kind = lh_os_input_event_get_kind(event);
    sink->log.button = lh_os_input_event_get_button(event);
    sink->log.x = lh_os_input_event_get_x(event);
    sink->log.y = lh_os_input_event_get_y(event);
    sink->log.buttons_seen_by_window = lh_os_window_get_pressed_buttons(sink->window);
}

/* One mouse event of @p kind about @p button, leaving @p buttons held, at (x, y). */
lh_os_input_event_t
mouse_event(lh_os_input_kind_t kind, lh_os_input_button_t button, lh_u32_t buttons, int x, int y)
{
    lh_os_input_event_t event{};

    lh_os_input_event_init_pointer(lh_addr_of(event), kind, lh_os_input_device_kind_mouse, 0U,
                                   button, lh_os_input_coord_from_int(x),
                                   lh_os_input_coord_from_int(y), 0U);
    lh_os_input_event_set_buttons(lh_addr_of(event), buttons);
    return event;
}

struct ResizeLog
{
    int count;
    int width;
    int height;
};

lh_void
on_resize(lh_os_window_t * /*self*/, int width, int height, lh_ptr context)
{
    auto *log = static_cast<ResizeLog *>(context);

    if (log == nullptr)
    {
        ADD_FAILURE() << "null resize log";
        return;
    }
    log->count++;
    log->width = width;
    log->height = height;
}

TEST(os_window, top_level_open_close_api)
{
    LH_TEST_NEEDS_DISPLAY();
    lh_os_app_t app{};
    lh_os_window_t main_win{};
    CloseLog log{};

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(main_win));
    lh_os_window_set_on_close(lh_addr_of(main_win), on_close, lh_addr_of(log));

    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(main_win), "lh-test-main", 320, 200),
              lh_bool_true);
    EXPECT_EQ(lh_os_window_is_open(lh_addr_of(main_win)), lh_bool_true);
    EXPECT_EQ(lh_os_app_get_window(lh_addr_of(app)), lh_addr_of(main_win));
    EXPECT_EQ(lh_os_window_get_app(lh_addr_of(main_win)), lh_addr_of(app));
    EXPECT_EQ(lh_os_window_get_parent(lh_addr_of(main_win)), lh_null);

    lh_os_window_close(lh_addr_of(main_win));
    EXPECT_EQ(lh_os_window_is_open(lh_addr_of(main_win)), lh_bool_false);
    EXPECT_EQ(log.count, 1);
    EXPECT_EQ(log.reason, lh_os_window_close_reason_api);
    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_true);
    EXPECT_EQ(lh_os_app_get_window(lh_addr_of(app)), lh_null);

    lh_os_window_deinit(lh_addr_of(main_win));
    lh_os_app_deinit(lh_addr_of(app));
}

TEST(os_window, second_top_level_is_not_main)
{
    LH_TEST_NEEDS_DISPLAY();
    lh_os_app_t app{};
    lh_os_window_t main_win{};
    lh_os_window_t other{};

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(main_win));
    lh_os_window_init(lh_addr_of(other));

    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(main_win), "lh-test-main", 200, 100),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(other), "lh-test-other", 200, 100),
              lh_bool_true);

    EXPECT_EQ(lh_os_app_get_first_window(lh_addr_of(app)), lh_addr_of(main_win));
    EXPECT_EQ(lh_os_app_get_next_window(lh_addr_of(app), lh_addr_of(main_win)), lh_addr_of(other));
    EXPECT_EQ(lh_os_app_get_next_window(lh_addr_of(app), lh_addr_of(other)), lh_null);

    lh_os_window_close(lh_addr_of(other));
    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_false);
    EXPECT_EQ(lh_os_app_get_window(lh_addr_of(app)), lh_addr_of(main_win));

    lh_os_window_close(lh_addr_of(main_win));
    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_true);

    lh_os_window_deinit(lh_addr_of(other));
    lh_os_window_deinit(lh_addr_of(main_win));
    lh_os_app_deinit(lh_addr_of(app));
}

TEST(os_window, modal_child_links_and_closes)
{
    LH_TEST_NEEDS_DISPLAY();
    lh_os_app_t app{};
    lh_os_window_t parent{};
    lh_os_window_t child{};
    CloseLog child_log{};

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(parent));
    lh_os_window_init(lh_addr_of(child));
    lh_os_window_set_on_close(lh_addr_of(child), on_close, lh_addr_of(child_log));

    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(parent), "lh-test-parent", 240, 160),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_open_modal(lh_addr_of(parent), lh_addr_of(child), "lh-test-modal", 180,
                                      120),
              lh_bool_true);

    EXPECT_EQ(lh_os_window_is_open(lh_addr_of(child)), lh_bool_true);
    EXPECT_EQ(lh_os_window_get_parent(lh_addr_of(child)), lh_addr_of(parent));
    EXPECT_EQ(lh_os_window_get_app(lh_addr_of(child)), lh_null);
    EXPECT_EQ(lh_os_window_get_first_child(lh_addr_of(parent)), lh_addr_of(child));
    EXPECT_EQ(lh_os_window_get_next_child(lh_addr_of(parent), lh_addr_of(child)), lh_null);

    lh_os_window_close(lh_addr_of(child));
    EXPECT_EQ(child_log.count, 1);
    EXPECT_EQ(child_log.reason, lh_os_window_close_reason_api);
    EXPECT_EQ(lh_os_window_get_first_child(lh_addr_of(parent)), lh_null);
    EXPECT_EQ(lh_os_window_is_open(lh_addr_of(parent)), lh_bool_true);
    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_false);

    lh_os_window_close(lh_addr_of(parent));
    lh_os_window_deinit(lh_addr_of(child));
    lh_os_window_deinit(lh_addr_of(parent));
    lh_os_app_deinit(lh_addr_of(app));
}

TEST(os_window, close_parent_closes_modal_child)
{
    LH_TEST_NEEDS_DISPLAY();
    lh_os_app_t app{};
    lh_os_window_t parent{};
    lh_os_window_t child{};
    CloseLog child_log{};

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(parent));
    lh_os_window_init(lh_addr_of(child));
    lh_os_window_set_on_close(lh_addr_of(child), on_close, lh_addr_of(child_log));

    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(parent), "lh-test-parent2", 240, 160),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_open_modal(lh_addr_of(parent), lh_addr_of(child), "lh-test-modal2", 180,
                                      120),
              lh_bool_true);

    lh_os_window_close(lh_addr_of(parent));
    EXPECT_EQ(lh_os_window_is_open(lh_addr_of(child)), lh_bool_false);
    EXPECT_EQ(lh_os_window_is_open(lh_addr_of(parent)), lh_bool_false);
    EXPECT_GE(child_log.count, 1);
    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_true);

    lh_os_window_deinit(lh_addr_of(child));
    lh_os_window_deinit(lh_addr_of(parent));
    lh_os_app_deinit(lh_addr_of(app));
}

TEST(os_window, on_native_input_reaches_the_sink_as_it_came)
{
    lh_os_window_t window{};
    InputSink sink{};
    const lh_u32_t primary = lh_os_input_button_to_mask(lh_os_input_button_primary);
    lh_os_input_event_t down =
        mouse_event(lh_os_input_kind_pointer_down, lh_os_input_button_primary, primary, 12, 34);

    lh_os_window_init(lh_addr_of(window));
    sink.window = lh_addr_of(window);
    lh_os_window_set_on_input(lh_addr_of(window), on_input, lh_addr_of(sink));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(down));
    EXPECT_EQ(sink.log.count, 1);
    EXPECT_EQ(sink.log.kind, lh_os_input_kind_pointer_down);
    EXPECT_EQ(sink.log.button, lh_os_input_button_primary);
    EXPECT_EQ(sink.log.x, lh_os_input_coord_from_int(12));
    EXPECT_EQ(sink.log.y, lh_os_input_coord_from_int(34));
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, an_event_with_no_sink_is_dropped)
{
    lh_os_window_t window{};
    lh_os_input_event_t key{};

    lh_os_window_init(lh_addr_of(window));
    lh_os_input_event_init_key(lh_addr_of(key), lh_os_key_tab, lh_bool_true, lh_bool_false, 0U,
                               0U);
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(key));
    EXPECT_EQ(lh_os_window_get_pressed_buttons(lh_addr_of(window)), 0U);
    lh_os_window_deinit(lh_addr_of(window));
}

/* The window keeps what the last pointer event left held, and has it already by the
   time the sink runs: a sink asking "is a button down" gets the answer the event gives. */
TEST(os_window, pressed_buttons_follow_the_pointer_events)
{
    lh_os_window_t window{};
    InputSink sink{};
    const lh_u32_t primary = lh_os_input_button_to_mask(lh_os_input_button_primary);
    const lh_u32_t secondary = lh_os_input_button_to_mask(lh_os_input_button_secondary);
    lh_os_input_event_t first =
        mouse_event(lh_os_input_kind_pointer_down, lh_os_input_button_primary, primary, 1, 2);
    lh_os_input_event_t second = mouse_event(
        lh_os_input_kind_pointer_down, lh_os_input_button_secondary, primary | secondary, 1, 2);
    lh_os_input_event_t move = mouse_event(lh_os_input_kind_pointer_move, lh_os_input_button_none,
                                           primary | secondary, 3, 4);
    lh_os_input_event_t up =
        mouse_event(lh_os_input_kind_pointer_up, lh_os_input_button_primary, secondary, 3, 4);

    lh_os_window_init(lh_addr_of(window));
    sink.window = lh_addr_of(window);
    lh_os_window_set_on_input(lh_addr_of(window), on_input, lh_addr_of(sink));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(first));
    EXPECT_EQ(sink.log.buttons_seen_by_window, primary);
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(second));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(move));
    EXPECT_EQ(lh_os_window_get_pressed_buttons(lh_addr_of(window)), primary | secondary);
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(up));
    EXPECT_EQ(lh_os_window_get_pressed_buttons(lh_addr_of(window)), secondary);
    EXPECT_EQ(sink.log.count, 4);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_cancel_or_a_focus_loss_lets_every_button_go)
{
    lh_os_window_t window{};
    const lh_u32_t primary = lh_os_input_button_to_mask(lh_os_input_button_primary);
    lh_os_input_event_t down =
        mouse_event(lh_os_input_kind_pointer_down, lh_os_input_button_primary, primary, 1, 2);
    lh_os_input_event_t cancel =
        mouse_event(lh_os_input_kind_pointer_cancel, lh_os_input_button_none, primary, 0, 0);
    lh_os_input_event_t focus_out{};

    lh_os_input_event_init(lh_addr_of(focus_out), lh_os_input_kind_focus_out,
                           lh_os_input_device_kind_keyboard, 0U);
    lh_os_window_init(lh_addr_of(window));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(down));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(cancel));
    EXPECT_EQ(lh_os_window_get_pressed_buttons(lh_addr_of(window)), 0U)
        << "a cancel says nothing is held, whatever its mask";
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(down));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(focus_out));
    EXPECT_EQ(lh_os_window_get_pressed_buttons(lh_addr_of(window)), 0U);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_key_event_leaves_the_buttons_alone)
{
    lh_os_window_t window{};
    const lh_u32_t primary = lh_os_input_button_to_mask(lh_os_input_button_primary);
    lh_os_input_event_t down =
        mouse_event(lh_os_input_kind_pointer_down, lh_os_input_button_primary, primary, 1, 2);
    lh_os_input_event_t key{};

    lh_os_input_event_init_key(lh_addr_of(key), lh_os_key_tab, lh_bool_true, lh_bool_false, 0U,
                               0U);
    lh_os_window_init(lh_addr_of(window));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(down));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(key));
    EXPECT_EQ(lh_os_window_get_pressed_buttons(lh_addr_of(window)), primary);
    lh_os_window_deinit(lh_addr_of(window));
}

/* What a window's controller was handed, and what the window's sink had seen by then. */
struct ControllerLog
{
    InputSink sink;
    int handled;
    int raw;
    int long_presses;
    int sink_count_at_first;
    int updates;
    int cancels;
    lh_os_input_kind_t last;
};

lh_bool_t
controller_handle(lh_self_ptr self, const lh_os_input_event_ptr event)
{
    auto *log = static_cast<ControllerLog *>(self);

    if (log->handled == 0)
    {
        log->sink_count_at_first = log->sink.log.count;
    }
    log->handled++;
    log->raw += lh_os_input_event_is_synthesized(event) ? 0 : 1;
    log->long_presses += lh_os_input_event_get_kind(event) == lh_os_input_kind_long_press ? 1 : 0;
    log->last = lh_os_input_event_get_kind(event);
    return lh_bool_true;
}

lh_void
controller_update(lh_self_ptr self, lh_tick_t /*now*/)
{
    static_cast<ControllerLog *>(self)->updates++;
}

lh_void
controller_cancel(lh_self_ptr self)
{
    static_cast<ControllerLog *>(self)->cancels++;
}

/* The sink keeps seeing every raw event, and first; the controller sees each one after
   it, with the gestures it makes of them. */
TEST(os_window, the_controller_gets_every_event_after_the_sink)
{
    lh_os_window_t window{};
    lh_os_controller_t controller{};
    ControllerLog log{};
    const lh_u32_t primary = lh_os_input_button_to_mask(lh_os_input_button_primary);
    lh_os_input_event_t down =
        mouse_event(lh_os_input_kind_pointer_down, lh_os_input_button_primary, primary, 5, 6);
    lh_os_input_event_t up =
        mouse_event(lh_os_input_kind_pointer_up, lh_os_input_button_primary, 0U, 5, 6);

    lh_os_window_init(lh_addr_of(window));
    lh_os_controller_init(lh_addr_of(controller), controller_handle, lh_addr_of(log), lh_null);
    log.sink.window = lh_addr_of(window);
    lh_os_window_set_on_input(lh_addr_of(window), on_input, lh_addr_of(log.sink));
    EXPECT_EQ(lh_os_window_get_controller(lh_addr_of(window)), nullptr);
    lh_os_window_set_controller(lh_addr_of(window), lh_addr_of(controller));
    EXPECT_EQ(lh_os_window_get_controller(lh_addr_of(window)), lh_addr_of(controller));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(down));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(up));
    EXPECT_EQ(log.sink.log.count, 2);
    EXPECT_EQ(log.sink_count_at_first, 1) << "the sink had the press before the controller";
    EXPECT_EQ(log.raw, 2);
    EXPECT_EQ(log.last, lh_os_input_kind_click) << "the recognizer's click follows the release";
    lh_os_window_set_controller(lh_addr_of(window), lh_null);
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(down));
    EXPECT_EQ(log.raw, 2) << "a cleared controller hears nothing";
    EXPECT_EQ(log.sink.log.count, 3);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_focus_loss_cancels_the_controller)
{
    lh_os_window_t window{};
    lh_os_controller_t controller{};
    ControllerLog log{};
    lh_os_input_event_t focus_out{};

    lh_os_input_event_init(lh_addr_of(focus_out), lh_os_input_kind_focus_out,
                           lh_os_input_device_kind_keyboard, 0U);
    lh_os_window_init(lh_addr_of(window));
    lh_os_controller_init(lh_addr_of(controller), controller_handle, lh_addr_of(log), lh_null);
    lh_os_controller_set_cancel_cb(lh_addr_of(controller), controller_cancel);
    lh_os_window_set_controller(lh_addr_of(window), lh_addr_of(controller));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(focus_out));
    EXPECT_EQ(log.raw, 1) << "the handler hears the focus loss itself";
    EXPECT_EQ(log.cancels, 1);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_tick_updates_the_controller_before_on_tick)
{
    lh_os_window_t window{};
    lh_os_controller_t controller{};
    ControllerLog log{};

    lh_os_window_init(lh_addr_of(window));
    lh_os_controller_init(lh_addr_of(controller), controller_handle, lh_addr_of(log), lh_null);
    lh_os_controller_set_update_cb(lh_addr_of(controller), controller_update);
    lh_os_window_on_native_tick(lh_addr_of(window));
    EXPECT_EQ(log.updates, 0);
    lh_os_window_set_controller(lh_addr_of(window), lh_addr_of(controller));
    lh_os_window_on_native_tick(lh_addr_of(window));
    lh_os_window_on_native_tick(lh_addr_of(window));
    EXPECT_EQ(log.updates, 2);
    lh_os_window_deinit(lh_addr_of(window));
}

/* A touch contact is not the mouse: the mask the window keeps for a lost capture is
   pointer 0's alone. */
TEST(os_window, a_touch_contact_leaves_the_mouse_buttons_alone)
{
    lh_os_window_t window{};
    const lh_u32_t primary = lh_os_input_button_to_mask(lh_os_input_button_primary);
    lh_os_input_event_t touch{};
    lh_os_input_event_t cancel{};

    lh_os_input_event_init_pointer(lh_addr_of(touch), lh_os_input_kind_pointer_down,
                                   lh_os_input_device_kind_touch, 1U, lh_os_input_button_primary,
                                   lh_os_input_coord_from_int(1), lh_os_input_coord_from_int(2),
                                   0U);
    lh_os_input_event_set_buttons(lh_addr_of(touch), primary);
    lh_os_input_event_init_pointer(
        lh_addr_of(cancel), lh_os_input_kind_pointer_cancel, lh_os_input_device_kind_touch, 1U,
        lh_os_input_button_none, lh_os_input_coord_from_int(1), lh_os_input_coord_from_int(2), 0U);
    lh_os_window_init(lh_addr_of(window));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(touch));
    EXPECT_EQ(lh_os_window_get_pressed_buttons(lh_addr_of(window)), 0U);
    lh_os_input_event_t mouse =
        mouse_event(lh_os_input_kind_pointer_down, lh_os_input_button_primary, primary, 1, 2);
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(mouse));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(cancel));
    EXPECT_EQ(lh_os_window_get_pressed_buttons(lh_addr_of(window)), primary)
        << "the cancel of a contact does not let go of the mouse";
    lh_os_window_deinit(lh_addr_of(window));
}

/* No on_tick: the window arms its timer for the recognizer's deadline by itself, and the
   long press arrives through the message pump alone. */
TEST(os_window, a_long_press_comes_due_on_the_window_timer_alone)
{
    LH_TEST_NEEDS_DISPLAY();
    lh_os_app_t app{};
    lh_os_window_t window{};
    lh_os_controller_t controller{};
    ControllerLog log{};
    lh_os_input_event_t down{};
    lh_tick_t started;

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(window));
    lh_os_controller_init(lh_addr_of(controller), controller_handle, lh_addr_of(log), lh_null);
    lh_os_window_set_controller(lh_addr_of(window), lh_addr_of(controller));
    ASSERT_EQ(
        lh_os_window_open(lh_addr_of(app), lh_addr_of(window), "lh_test long press", 160, 120),
        lh_bool_true);
    lh_os_input_event_init_pointer(lh_addr_of(down), lh_os_input_kind_pointer_down,
                                   lh_os_input_device_kind_touch, 1U, lh_os_input_button_primary,
                                   lh_os_input_coord_from_int(10), lh_os_input_coord_from_int(10),
                                   lh_os_tick_ms());
    lh_os_input_event_set_buttons(lh_addr_of(down),
                                  lh_os_input_button_to_mask(lh_os_input_button_primary));
    lh_os_window_on_native_input(lh_addr_of(window), lh_addr_of(down));
    started = lh_os_tick_ms();
    while (log.long_presses == 0 && lh_tick_elapsed(started, lh_os_tick_ms()) < 3000U)
    {
        (void)lh_os_system_window_pump_wait(50U);
    }
    EXPECT_EQ(log.long_presses, 1);
    lh_os_window_close(lh_addr_of(window));
    lh_os_app_deinit(lh_addr_of(app));
}

TEST(os_window, zone_is_client_until_the_caller_says_otherwise)
{
    lh_os_window_t window{};

    lh_os_window_init(lh_addr_of(window));
    /* No callback is the ordinary window: every point is the app's, which is what
       keeps a press reaching a listener instead of being swallowed as a caption. */
    EXPECT_EQ(lh_os_window_zone_at(lh_addr_of(window), 0, 0), lh_os_window_zone_client);
    EXPECT_EQ(lh_os_window_zone_at(lh_addr_of(window), 400, 300), lh_os_window_zone_client);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, zone_answers_with_the_callers_own_layout)
{
    lh_os_window_t window{};
    ZoneLog log{0, -1, -1, lh_os_window_zone_client, 36};

    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_on_zone(lh_addr_of(window), on_zone, lh_addr_of(log));

    EXPECT_EQ(lh_os_window_zone_at(lh_addr_of(window), 300, 18), lh_os_window_zone_caption);
    EXPECT_EQ(log.count, 1);
    EXPECT_EQ(log.x, 300);
    EXPECT_EQ(log.y, 18);

    /* The button the caller drew on its own title bar: still its own, so a press
       there clicks it instead of moving the window. */
    EXPECT_EQ(lh_os_window_zone_at(lh_addr_of(window), 110, 10), lh_os_window_zone_client);
    EXPECT_EQ(lh_os_window_zone_at(lh_addr_of(window), 110, 40), lh_os_window_zone_client);
    EXPECT_EQ(lh_os_window_zone_at(lh_addr_of(window), 400, 300), lh_os_window_zone_client);
    EXPECT_EQ(log.count, 4);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_window_starts_with_the_system_frame_and_square_corners)
{
    lh_os_window_t window{};

    lh_os_window_init(lh_addr_of(window));
    /* Both default to what a window that says nothing wants: the OS frame, and no
       cut on its corners. */
    EXPECT_EQ(window.frame, lh_os_window_frame_system);
    EXPECT_EQ(window.corner, 0);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, corner_radius_is_kept_and_clamped_to_square)
{
    lh_os_window_t window{};

    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_corner_radius(lh_addr_of(window), 14);
    EXPECT_EQ(window.corner, 14);
    /* A negative radius is not an error, it is square: the caller asking for nothing
       should not have to special-case the ask. */
    lh_os_window_set_corner_radius(lh_addr_of(window), -3);
    EXPECT_EQ(window.corner, 0);
    lh_os_window_set_corner_radius(lh_addr_of(window), 0);
    EXPECT_EQ(window.corner, 0);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, frame_and_zone_do_not_interfere)
{
    lh_os_window_t window{};
    ZoneLog log{0, -1, -1, lh_os_window_zone_client, 36};

    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_frame_v(lh_addr_of(window), lh_os_window_frame_own);
    /* Naming zones is what the caller's own frame has instead of an OS one: with a
       system frame the OS already moves and resizes the window, and the zones say
       nothing about that. */
    lh_os_window_set_on_zone(lh_addr_of(window), on_zone, lh_addr_of(log));

    EXPECT_EQ(window.frame, lh_os_window_frame_own);
    EXPECT_EQ(lh_os_window_zone_at(lh_addr_of(window), 300, 18), lh_os_window_zone_caption);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_resize_reports_the_client_and_the_maximized_state)
{
    lh_os_window_t window{};
    ResizeLog log{0, -1, -1};

    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_on_resize(lh_addr_of(window), on_resize, lh_addr_of(log));

    lh_os_window_on_native_resize(lh_addr_of(window), 2560, 1440, lh_bool_true);
    EXPECT_EQ(log.count, 1);
    EXPECT_EQ(log.width, 2560);
    EXPECT_EQ(log.height, 1440);
    EXPECT_EQ(lh_os_window_is_maximized(lh_addr_of(window)), lh_bool_true);

    lh_os_window_on_native_resize(lh_addr_of(window), 800, 600, lh_bool_false);
    EXPECT_EQ(log.count, 2);
    EXPECT_EQ(lh_os_window_is_maximized(lh_addr_of(window)), lh_bool_false);
    lh_os_window_deinit(lh_addr_of(window));
}

/* The message that makes a window appear carries the size before creation has
   applied the one it was created with, and a minimized window really does have no
   client at all. Neither is a size to lay out for: an app that laid out for one puts
   every entity at the origin or off it — a title bar 0 wide is invisible. The
   maximized flag is a state, not a size, so it is still recorded. */
TEST(os_window, an_empty_client_is_not_a_size_to_lay_out_for)
{
    lh_os_window_t window{};
    ResizeLog log{0, -1, -1};

    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_on_resize(lh_addr_of(window), on_resize, lh_addr_of(log));

    lh_os_window_on_native_resize(lh_addr_of(window), 0, 0, lh_bool_false);
    EXPECT_EQ(log.count, 0);
    lh_os_window_on_native_resize(lh_addr_of(window), 800, 0, lh_bool_false);
    EXPECT_EQ(log.count, 0);
    lh_os_window_on_native_resize(lh_addr_of(window), 0, 600, lh_bool_false);
    EXPECT_EQ(log.count, 0);
    lh_os_window_on_native_resize(lh_addr_of(window), -800, -600, lh_bool_false);
    EXPECT_EQ(log.count, 0);

    /* Minimizing a maximized window is the same message with nothing to draw into,
       and the state still has to change: the app asks it to draw the right glyph. */
    lh_os_window_on_native_resize(lh_addr_of(window), 2560, 1440, lh_bool_true);
    lh_os_window_on_native_resize(lh_addr_of(window), 0, 0, lh_bool_false);
    EXPECT_EQ(lh_os_window_is_maximized(lh_addr_of(window)), lh_bool_false);
    EXPECT_EQ(log.count, 1);

    /* The next real size is the one the app lays out for. */
    lh_os_window_on_native_resize(lh_addr_of(window), 800, 600, lh_bool_false);
    EXPECT_EQ(log.count, 2);
    EXPECT_EQ(log.width, 800);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_window_without_a_resize_callback_still_keeps_its_state)
{
    lh_os_window_t window{};

    lh_os_window_init(lh_addr_of(window));
    lh_os_window_on_native_resize(lh_addr_of(window), 0, 0, lh_bool_true);
    EXPECT_EQ(lh_os_window_is_maximized(lh_addr_of(window)), lh_bool_true);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_window_opens_where_the_window_system_wants_by_default)
{
    lh_os_window_t window{};

    lh_os_window_init(lh_addr_of(window));
    /* Saying nothing is a choice too: the window system's own placement, which is
       not the middle of the screen. */
    EXPECT_EQ(lh_os_window_get_placement(lh_addr_of(window)), lh_os_window_placement_default);
    lh_os_window_set_placement_v(lh_addr_of(window), lh_os_window_placement_center);
    EXPECT_EQ(lh_os_window_get_placement(lh_addr_of(window)), lh_os_window_placement_center);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_centred_window_centres_on_what_the_system_left_usable)
{
    LH_TEST_NEEDS_DISPLAY();
    lh_os_app_t app{};
    lh_os_window_t plain{};
    lh_os_window_t small{};
    lh_os_window_t large{};
    int plain_x = 0;
    int plain_y = 0;
    int small_x = 0;
    int small_y = 0;
    int small_width = 0;
    int small_height = 0;
    int large_x = 0;
    int large_y = 0;
    int large_width = 0;
    int large_height = 0;

    lh_os_app_init(lh_addr_of(app));

    /* The same window three ways. The work area — the monitor without the taskbar
       and any docked app bars — is what "centred" is measured against, and there
       is no call that asks what it is, so this checks the two things that hold
       whatever that area turns out to be on this machine. */
    lh_os_window_init(lh_addr_of(plain));
    lh_os_window_set_frame_v(lh_addr_of(plain), lh_os_window_frame_own);
    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(plain), "lh-test-plain", 320, 200),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_get_position(lh_addr_of(plain), lh_addr_of(plain_x), lh_addr_of(plain_y)),
              lh_bool_true);
    lh_os_window_close(lh_addr_of(plain));

    lh_os_window_init(lh_addr_of(small));
    lh_os_window_set_frame_v(lh_addr_of(small), lh_os_window_frame_own);
    lh_os_window_set_placement_v(lh_addr_of(small), lh_os_window_placement_center);
    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(small), "lh-test-centred", 320, 200),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_get_position(lh_addr_of(small), lh_addr_of(small_x), lh_addr_of(small_y)),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_get_client_size(lh_addr_of(small), lh_addr_of(small_width),
                                           lh_addr_of(small_height)),
              lh_bool_true);
    lh_os_window_close(lh_addr_of(small));

    lh_os_window_init(lh_addr_of(large));
    lh_os_window_set_frame_v(lh_addr_of(large), lh_os_window_frame_own);
    lh_os_window_set_placement_v(lh_addr_of(large), lh_os_window_placement_center);
    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(large), "lh-test-centred2", 500, 300),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_get_position(lh_addr_of(large), lh_addr_of(large_x), lh_addr_of(large_y)),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_get_client_size(lh_addr_of(large), lh_addr_of(large_width),
                                           lh_addr_of(large_height)),
              lh_bool_true);

    /* The window system's own answer is a cascade from the corner, which is not
       the middle of anything, so moving off it is what says the placement was read
       at creation rather than ignored. */
    EXPECT_TRUE(small_x != plain_x || small_y != plain_y);

    /* Two centred windows of different sizes share one centre — left plus right
       edges twice, so no rounding hides a pixel — and that holds whichever area was
       centred on, which is why this does not need to know what it was. */
    EXPECT_EQ(small_x * 2 + small_width, large_x * 2 + large_width);
    EXPECT_EQ(small_y * 2 + small_height, large_y * 2 + large_height);

    lh_os_window_close(lh_addr_of(large));
    lh_os_app_deinit(lh_addr_of(app));
}

TEST(os_window, a_window_bigger_than_the_screen_keeps_its_title_reachable)
{
    LH_TEST_NEEDS_DISPLAY();
    lh_os_app_t app{};
    lh_os_window_t window{};
    int x = 0;
    int y = 0;

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_frame_v(lh_addr_of(window), lh_os_window_frame_own);
    lh_os_window_set_placement_v(lh_addr_of(window), lh_os_window_placement_center);
    /* Far wider and taller than any desktop, so the centred corner would be well
       off the screen and the title bar with it. */
    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(window), "lh-test-huge", 20000, 20000),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_get_position(lh_addr_of(window), lh_addr_of(x), lh_addr_of(y)), lh_bool_true);

    /* Not resized — the size asked for is the size asked for — but the corner lands
       on the work area, so the window is still a window the user can move. */
    EXPECT_GE(x, 0);
    EXPECT_GE(y, 0);
    EXPECT_LT(x, 20000);
    EXPECT_LT(y, 20000);

    lh_os_window_close(lh_addr_of(window));
    lh_os_app_deinit(lh_addr_of(app));
}

} // namespace
