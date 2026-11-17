/**
 * @file backend.cpp
 * @brief Tests for `lh/os/window.h` — the table a backend fills.
 *
 * The table is a brick of its own: every slot starts empty, each one reads
 * back what was stored, and the table is usable only when every required
 * slot is set. The system backend, where there is one, fills all of them.
 */

#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/os/window.h>

#if LH_LIBRARY_OPTION_OS_WINDOW
#    include <lh/os/window/backend/system.h>
#endif

namespace
{

lh_bool_t
test_open(lh_ptr, lh_os_window_t *, const lh_char_t *, int, int, lh_os_window_t *)
{
    return lh_bool_false;
}

lh_void
test_close(lh_ptr, lh_os_window_t *)
{
}

lh_void
test_invalidate(lh_ptr, lh_os_window_t *)
{
}

lh_void
test_invalidate_rect(lh_ptr, lh_os_window_t *, int, int, int, int)
{
}

lh_tick_t
test_now(lh_ptr)
{
    return 0U;
}

lh_void
test_set_timer(lh_ptr, lh_os_window_t *, lh_u32_t)
{
}

lh_void
test_set_enabled(lh_ptr, lh_os_window_t *, lh_bool_t)
{
}

lh_void
test_minimize(lh_ptr, lh_os_window_t *)
{
}

lh_void
test_set_maximized(lh_ptr, lh_os_window_t *, lh_bool_t)
{
}

lh_void
test_set_corner_radius(lh_ptr, lh_os_window_t *, int)
{
}

lh_bool_t
test_get_client_size(lh_ptr, const lh_os_window_t *, int *, int *)
{
    return lh_bool_false;
}

lh_bool_t
test_get_position(lh_ptr, const lh_os_window_t *, int *, int *)
{
    return lh_bool_false;
}

lh_bool_t
test_get_paint_region(lh_ptr, lh_ptr, lh_u64_t *, lh_u32_t *)
{
    return lh_bool_false;
}

lh_u64_t
test_now_us(lh_ptr)
{
    return 0U;
}

TEST(os_window_backend, a_new_table_has_every_slot_empty)
{
    lh_os_window_backend_t table;

    lh_os_window_backend_init(&table);
    EXPECT_TRUE(lh_os_window_backend_get_open(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_close(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_invalidate(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_invalidate_rect(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_now(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_set_timer(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_set_enabled(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_minimize(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_set_maximized(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_set_corner_radius(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_get_client_size(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_get_position(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_get_paint_region(&table) == nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_now_us(&table) == nullptr);
    EXPECT_EQ(lh_os_window_backend_is_complete(&table), lh_bool_false);
}

TEST(os_window_backend, every_slot_reads_back_what_was_stored)
{
    lh_os_window_backend_t table;

    lh_os_window_backend_init(&table);
    lh_os_window_backend_set_open(&table, test_open);
    EXPECT_TRUE(lh_os_window_backend_get_open(&table) == test_open);
    lh_os_window_backend_set_close(&table, test_close);
    EXPECT_TRUE(lh_os_window_backend_get_close(&table) == test_close);
    lh_os_window_backend_set_invalidate(&table, test_invalidate);
    EXPECT_TRUE(lh_os_window_backend_get_invalidate(&table) == test_invalidate);
    lh_os_window_backend_set_invalidate_rect(&table, test_invalidate_rect);
    EXPECT_TRUE(lh_os_window_backend_get_invalidate_rect(&table) == test_invalidate_rect);
    lh_os_window_backend_set_now(&table, test_now);
    EXPECT_TRUE(lh_os_window_backend_get_now(&table) == test_now);
    lh_os_window_backend_set_set_timer(&table, test_set_timer);
    EXPECT_TRUE(lh_os_window_backend_get_set_timer(&table) == test_set_timer);
    lh_os_window_backend_set_set_enabled(&table, test_set_enabled);
    EXPECT_TRUE(lh_os_window_backend_get_set_enabled(&table) == test_set_enabled);
    lh_os_window_backend_set_minimize(&table, test_minimize);
    EXPECT_TRUE(lh_os_window_backend_get_minimize(&table) == test_minimize);
    lh_os_window_backend_set_set_maximized(&table, test_set_maximized);
    EXPECT_TRUE(lh_os_window_backend_get_set_maximized(&table) == test_set_maximized);
    lh_os_window_backend_set_set_corner_radius(&table, test_set_corner_radius);
    EXPECT_TRUE(lh_os_window_backend_get_set_corner_radius(&table) == test_set_corner_radius);
    lh_os_window_backend_set_get_client_size(&table, test_get_client_size);
    EXPECT_TRUE(lh_os_window_backend_get_get_client_size(&table) == test_get_client_size);
    lh_os_window_backend_set_get_position(&table, test_get_position);
    EXPECT_TRUE(lh_os_window_backend_get_get_position(&table) == test_get_position);
    lh_os_window_backend_set_get_paint_region(&table, test_get_paint_region);
    EXPECT_TRUE(lh_os_window_backend_get_get_paint_region(&table) == test_get_paint_region);
    lh_os_window_backend_set_now_us(&table, test_now_us);
    EXPECT_TRUE(lh_os_window_backend_get_now_us(&table) == test_now_us);
    EXPECT_EQ(lh_os_window_backend_is_complete(&table), lh_bool_true);
}

TEST(os_window_backend, the_table_is_complete_only_with_every_required_slot)
{
    {
        lh_os_window_backend_t table;

        lh_os_window_backend_init(&table);
        lh_os_window_backend_set_close(&table, test_close);
        lh_os_window_backend_set_invalidate(&table, test_invalidate);
        lh_os_window_backend_set_invalidate_rect(&table, test_invalidate_rect);
        lh_os_window_backend_set_now(&table, test_now);
        EXPECT_EQ(lh_os_window_backend_is_complete(&table), lh_bool_false) << "without `open`";
    }
    {
        lh_os_window_backend_t table;

        lh_os_window_backend_init(&table);
        lh_os_window_backend_set_open(&table, test_open);
        lh_os_window_backend_set_invalidate(&table, test_invalidate);
        lh_os_window_backend_set_invalidate_rect(&table, test_invalidate_rect);
        lh_os_window_backend_set_now(&table, test_now);
        EXPECT_EQ(lh_os_window_backend_is_complete(&table), lh_bool_false) << "without `close`";
    }
    {
        lh_os_window_backend_t table;

        lh_os_window_backend_init(&table);
        lh_os_window_backend_set_open(&table, test_open);
        lh_os_window_backend_set_close(&table, test_close);
        lh_os_window_backend_set_invalidate_rect(&table, test_invalidate_rect);
        lh_os_window_backend_set_now(&table, test_now);
        EXPECT_EQ(lh_os_window_backend_is_complete(&table), lh_bool_false)
            << "without `invalidate`";
    }
    {
        lh_os_window_backend_t table;

        lh_os_window_backend_init(&table);
        lh_os_window_backend_set_open(&table, test_open);
        lh_os_window_backend_set_close(&table, test_close);
        lh_os_window_backend_set_invalidate(&table, test_invalidate);
        lh_os_window_backend_set_now(&table, test_now);
        EXPECT_EQ(lh_os_window_backend_is_complete(&table), lh_bool_false)
            << "without `invalidate_rect`";
    }
    {
        lh_os_window_backend_t table;

        lh_os_window_backend_init(&table);
        lh_os_window_backend_set_open(&table, test_open);
        lh_os_window_backend_set_close(&table, test_close);
        lh_os_window_backend_set_invalidate(&table, test_invalidate);
        lh_os_window_backend_set_invalidate_rect(&table, test_invalidate_rect);
        EXPECT_EQ(lh_os_window_backend_is_complete(&table), lh_bool_false) << "without `now`";
    }
}

#if LH_LIBRARY_OPTION_OS_WINDOW

TEST(os_window_backend, the_system_backend_fills_every_slot)
{
    lh_os_window_backend_t table;

    lh_os_window_backend_system_init(&table);
    EXPECT_TRUE(lh_os_window_backend_get_open(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_close(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_invalidate(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_invalidate_rect(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_now(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_set_timer(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_set_enabled(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_minimize(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_set_maximized(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_set_corner_radius(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_get_client_size(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_get_position(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_get_paint_region(&table) != nullptr);
    EXPECT_TRUE(lh_os_window_backend_get_now_us(&table) != nullptr);
    EXPECT_EQ(lh_os_window_backend_is_complete(&table), lh_bool_true);
}

#endif

} // namespace
