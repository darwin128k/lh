#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/os/app.h>
#include <lh/os/window.h>
#include <lh/os/window/close/reason.h>
#include <lh/util/addr.h>

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

struct ClickLog
{
    int count;
    int x;
    int y;
};

lh_void
on_click(lh_os_window_t * /*self*/, int x, int y, lh_ptr context)
{
    auto *log = static_cast<ClickLog *>(context);

    if (log == nullptr)
    {
        ADD_FAILURE() << "null click log";
        return;
    }
    log->count++;
    log->x = x;
    log->y = y;
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

TEST(os_window, on_native_click_fires_callback)
{
    lh_os_window_t window{};
    ClickLog log{};

    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_on_click(lh_addr_of(window), on_click, lh_addr_of(log));
    lh_os_window_on_native_click(lh_addr_of(window), 12, 34);
    EXPECT_EQ(log.count, 1);
    EXPECT_EQ(log.x, 12);
    EXPECT_EQ(log.y, 34);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, on_native_press_move_release_and_wheel_fire_callbacks)
{
    lh_os_window_t window{};
    ClickLog press{};
    ClickLog move{};
    ClickLog release{};
    int wheel[2] = {0, 0};

    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_on_press(lh_addr_of(window), on_click, lh_addr_of(press));
    lh_os_window_set_on_move(lh_addr_of(window), on_click, lh_addr_of(move));
    lh_os_window_set_on_release(lh_addr_of(window), on_click, lh_addr_of(release));
    lh_os_window_set_on_wheel(
        lh_addr_of(window),
        [](lh_os_window_t *, int /*x*/, int /*y*/, int dx, int dy, lh_ptr context) {
            static_cast<int *>(context)[0] = dx;
            static_cast<int *>(context)[1] = dy;
        },
        wheel);

    lh_os_window_on_native_press(lh_addr_of(window), 1, 2);
    lh_os_window_on_native_move(lh_addr_of(window), 3, 4);
    lh_os_window_on_native_release(lh_addr_of(window), 5, 6);
    lh_os_window_on_native_wheel(lh_addr_of(window), 7, 8, 1, -2);

    EXPECT_EQ(press.count, 1);
    EXPECT_EQ(press.x, 1);
    EXPECT_EQ(press.y, 2);
    EXPECT_EQ(move.count, 1);
    EXPECT_EQ(move.x, 3);
    EXPECT_EQ(move.y, 4);
    EXPECT_EQ(release.count, 1);
    EXPECT_EQ(release.x, 5);
    EXPECT_EQ(release.y, 6);
    EXPECT_EQ(wheel[0], 1);
    EXPECT_EQ(wheel[1], -2);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, on_native_key_and_text_fire_callbacks)
{
    lh_os_window_t window{};
    int key_seen[2] = {-1, -1};
    lh_u32_t text_seen = 0;

    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_on_key(
        lh_addr_of(window),
        [](lh_os_window_t *, lh_key_t key, lh_bool_t pressed, lh_ptr context) {
            static_cast<int *>(context)[0] = static_cast<int>(key);
            static_cast<int *>(context)[1] = pressed ? 1 : 0;
        },
        key_seen);
    lh_os_window_set_on_text(
        lh_addr_of(window),
        [](lh_os_window_t *, lh_u32_t code, lh_ptr context) { *static_cast<lh_u32_t *>(context) = code; },
        lh_addr_of(text_seen));

    lh_os_window_on_native_key(lh_addr_of(window), lh_key_tab, lh_bool_true);
    lh_os_window_on_native_text(lh_addr_of(window), 0x0416U);

    EXPECT_EQ(key_seen[0], static_cast<int>(lh_key_tab));
    EXPECT_EQ(key_seen[1], 1);
    EXPECT_EQ(text_seen, 0x0416U);
    lh_os_window_deinit(lh_addr_of(window));
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
    lh_os_window_set_frame(lh_addr_of(window), lh_os_window_frame_own);
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
    lh_os_window_set_placement(lh_addr_of(window), lh_os_window_placement_center);
    EXPECT_EQ(lh_os_window_get_placement(lh_addr_of(window)), lh_os_window_placement_center);
    lh_os_window_deinit(lh_addr_of(window));
}

TEST(os_window, a_centred_window_centres_on_what_the_system_left_usable)
{
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
    lh_os_window_set_frame(lh_addr_of(plain), lh_os_window_frame_own);
    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(plain), "lh-test-plain", 320, 200),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_get_position(lh_addr_of(plain), lh_addr_of(plain_x), lh_addr_of(plain_y)),
              lh_bool_true);
    lh_os_window_close(lh_addr_of(plain));

    lh_os_window_init(lh_addr_of(small));
    lh_os_window_set_frame(lh_addr_of(small), lh_os_window_frame_own);
    lh_os_window_set_placement(lh_addr_of(small), lh_os_window_placement_center);
    ASSERT_EQ(lh_os_window_open(lh_addr_of(app), lh_addr_of(small), "lh-test-centred", 320, 200),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_get_position(lh_addr_of(small), lh_addr_of(small_x), lh_addr_of(small_y)),
              lh_bool_true);
    ASSERT_EQ(lh_os_window_get_client_size(lh_addr_of(small), lh_addr_of(small_width),
                                           lh_addr_of(small_height)),
              lh_bool_true);
    lh_os_window_close(lh_addr_of(small));

    lh_os_window_init(lh_addr_of(large));
    lh_os_window_set_frame(lh_addr_of(large), lh_os_window_frame_own);
    lh_os_window_set_placement(lh_addr_of(large), lh_os_window_placement_center);
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
    lh_os_app_t app{};
    lh_os_window_t window{};
    int x = 0;
    int y = 0;

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(window));
    lh_os_window_set_frame(lh_addr_of(window), lh_os_window_frame_own);
    lh_os_window_set_placement(lh_addr_of(window), lh_os_window_placement_center);
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
