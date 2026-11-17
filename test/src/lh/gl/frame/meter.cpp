/**
 * @file meter.cpp
 * @brief Tests for `lh/gl/frame/meter.h` — a paint that measures the paint it wraps.
 *
 * The meter is gl's composition over the window's contract: the rectangle comes
 * from the window, the request and the time from the backend's optional slots.
 * Both a backend that has them and a window with no backend at all are checked,
 * without an operating system.
 */

#include <gtest/gtest.h>

#include <lh/gl/frame/meter.h>
#include <lh/os/app.h>

namespace
{

/* A backend with a clock that moves 7 us a read and a request of 2 rectangles. */
struct Clocked
{
    lh_os_window_backend_t table;
    lh_u64_t us;
    lh_ptr target_seen;
};

lh_bool_t
clocked_open(lh_ptr context, lh_os_window_t *window, const lh_char_t *, int, int, lh_os_window_t *)
{
    lh_os_window_set_native(window, context);
    return lh_bool_true;
}

lh_void
clocked_close(lh_ptr, lh_os_window_t *window)
{
    lh_os_window_on_native_destroy(window);
}

lh_void
clocked_invalidate(lh_ptr, lh_os_window_t *)
{
}

lh_void
clocked_invalidate_rect(lh_ptr, lh_os_window_t *, int, int, int, int)
{
}

lh_tick_t
clocked_now(lh_ptr)
{
    return 0U;
}

lh_u64_t
clocked_now_us(lh_ptr context)
{
    auto *clocked = static_cast<Clocked *>(context);

    clocked->us += 7U;
    return clocked->us;
}

lh_bool_t
clocked_region(lh_ptr context, lh_ptr target, lh_u64_t *area, lh_u32_t *count)
{
    static_cast<Clocked *>(context)->target_seen = target;
    *area = 300U;
    *count = 2U;
    return lh_bool_true;
}

struct Painted
{
    int calls;
    lh_os_window_t *window;
};

lh_void
measured_paint(lh_os_window_t *window, lh_ptr context)
{
    auto *painted = static_cast<Painted *>(context);

    painted->calls++;
    painted->window = window;
}

TEST(gl_frame_meter, records_what_the_backend_says_around_the_paint)
{
    Clocked clocked{};
    lh_os_app_t app{};
    lh_os_window_t window{};
    lh_gl_frame_stats_t stats;
    lh_gl_frame_meter_t meter;
    Painted painted{};
    int target = 0;

    lh_os_window_backend_init(&clocked.table);
    lh_os_window_backend_set_open(&clocked.table, clocked_open);
    lh_os_window_backend_set_close(&clocked.table, clocked_close);
    lh_os_window_backend_set_invalidate(&clocked.table, clocked_invalidate);
    lh_os_window_backend_set_invalidate_rect(&clocked.table, clocked_invalidate_rect);
    lh_os_window_backend_set_now(&clocked.table, clocked_now);
    lh_os_window_backend_set_now_us(&clocked.table, clocked_now_us);
    lh_os_window_backend_set_get_paint_region(&clocked.table, clocked_region);
    lh_os_app_init(&app);
    lh_os_app_install_window_backend(&app, &clocked.table, &clocked);
    lh_os_window_init(&window);
    ASSERT_EQ(lh_os_window_open(&app, &window, "meter", 100, 100), lh_bool_true);

    lh_gl_frame_stats_init(&stats);
    lh_gl_frame_meter_init(&meter, &stats, measured_paint, &painted);
    lh_gl_frame_meter_attach(&meter, &window);
    lh_os_window_on_native_paint(&window, &target, 10, 20, 50, 40);

    EXPECT_EQ(painted.calls, 1) << "the measured paint ran";
    EXPECT_EQ(painted.window, &window);
    EXPECT_EQ(clocked.target_seen, &target) << "the request was read off the paint's target";
    EXPECT_EQ(lh_gl_frame_stats_get_frames(&stats), 1U);
    EXPECT_EQ(lh_gl_frame_stats_get_drawn_px(&stats), 40U * 20U);
    EXPECT_EQ(lh_gl_frame_stats_get_asked_px(&stats), 300U);
    EXPECT_EQ(lh_gl_frame_stats_get_rects(&stats), 2U);
    EXPECT_EQ(lh_gl_frame_stats_get_unknown(&stats), 0U);
    EXPECT_EQ(lh_gl_frame_stats_get_us(&stats), 7U) << "one tick of the backend's clock";

    lh_os_window_close(&window);
    lh_os_window_deinit(&window);
    lh_os_app_deinit(&app);
}

TEST(gl_frame_meter, a_window_without_a_backend_records_an_unknown_request)
{
    lh_os_window_t window{};
    lh_gl_frame_stats_t stats;
    lh_gl_frame_meter_t meter;
    Painted painted{};

    lh_os_window_init(&window);
    lh_gl_frame_stats_init(&stats);
    lh_gl_frame_meter_init(&meter, &stats, measured_paint, &painted);
    lh_gl_frame_meter_attach(&meter, &window);
    lh_os_window_on_native_paint(&window, nullptr, 0, 0, 8, 8);

    EXPECT_EQ(painted.calls, 1);
    EXPECT_EQ(lh_gl_frame_stats_get_frames(&stats), 1U);
    EXPECT_EQ(lh_gl_frame_stats_get_unknown(&stats), 1U);
    EXPECT_EQ(lh_gl_frame_stats_get_us(&stats), 0U);
    EXPECT_EQ(lh_gl_frame_meter_now_us(&window), 0U);
    lh_os_window_deinit(&window);
}

TEST(gl_frame_meter, keeps_what_it_was_given)
{
    lh_gl_frame_stats_t stats;
    lh_gl_frame_meter_t meter;
    int context = 0;

    lh_gl_frame_meter_init(&meter, &stats, measured_paint, &context);
    EXPECT_EQ(lh_gl_frame_meter_get_stats(&meter), &stats);
    EXPECT_TRUE(lh_gl_frame_meter_get_on_paint_fn(&meter) == measured_paint);
    EXPECT_EQ(lh_gl_frame_meter_get_on_paint_context(&meter), &context);
}

} // namespace
