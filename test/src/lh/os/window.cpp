#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/os/app.h>
#include <lh/os/window.h>
#include <lh/os/window/close/reason.h>
#include <lh/util/addr.h>

namespace
{

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

} // namespace
