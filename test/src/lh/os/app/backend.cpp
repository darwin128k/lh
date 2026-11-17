/**
 * @file backend.cpp
 * @brief Tests for `lh/os/app/backend.h` — the table a backend fills.
 *
 * The table is a brick of its own: every slot starts empty, each one reads
 * back what was stored, and the table is usable only when every required
 * slot is set. The system backend, where there is one, fills all of them.
 */

#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/os/app/backend.h>

#if LH_LIBRARY_OPTION_OS_WINDOW
#    include <lh/os/app/backend/system.h>
#endif

namespace
{

lh_tick_t
test_now(lh_ptr)
{
    return 0U;
}

lh_bool_t
test_wait(lh_ptr, lh_tick_t)
{
    return lh_bool_false;
}

lh_void
test_wake(lh_ptr)
{
}

TEST(os_app_backend, a_new_table_has_every_slot_empty)
{
    lh_os_app_backend_t table;

    lh_os_app_backend_init(&table);
    EXPECT_TRUE(lh_os_app_backend_get_now(&table) == nullptr);
    EXPECT_TRUE(lh_os_app_backend_get_wait(&table) == nullptr);
    EXPECT_TRUE(lh_os_app_backend_get_wake(&table) == nullptr);
    EXPECT_EQ(lh_os_app_backend_is_complete(&table), lh_bool_false);
}

TEST(os_app_backend, every_slot_reads_back_what_was_stored)
{
    lh_os_app_backend_t table;

    lh_os_app_backend_init(&table);
    lh_os_app_backend_set_now(&table, test_now);
    EXPECT_TRUE(lh_os_app_backend_get_now(&table) == test_now);
    lh_os_app_backend_set_wait(&table, test_wait);
    EXPECT_TRUE(lh_os_app_backend_get_wait(&table) == test_wait);
    lh_os_app_backend_set_wake(&table, test_wake);
    EXPECT_TRUE(lh_os_app_backend_get_wake(&table) == test_wake);
    EXPECT_EQ(lh_os_app_backend_is_complete(&table), lh_bool_true);
}

TEST(os_app_backend, the_table_is_complete_only_with_every_required_slot)
{
    {
        lh_os_app_backend_t table;

        lh_os_app_backend_init(&table);
        lh_os_app_backend_set_wait(&table, test_wait);
        lh_os_app_backend_set_wake(&table, test_wake);
        EXPECT_EQ(lh_os_app_backend_is_complete(&table), lh_bool_false) << "without `now`";
    }
    {
        lh_os_app_backend_t table;

        lh_os_app_backend_init(&table);
        lh_os_app_backend_set_now(&table, test_now);
        lh_os_app_backend_set_wake(&table, test_wake);
        EXPECT_EQ(lh_os_app_backend_is_complete(&table), lh_bool_false) << "without `wait`";
    }
    {
        lh_os_app_backend_t table;

        lh_os_app_backend_init(&table);
        lh_os_app_backend_set_now(&table, test_now);
        lh_os_app_backend_set_wait(&table, test_wait);
        EXPECT_EQ(lh_os_app_backend_is_complete(&table), lh_bool_false) << "without `wake`";
    }
}

#if LH_LIBRARY_OPTION_OS_WINDOW

TEST(os_app_backend, the_system_backend_fills_every_slot)
{
    lh_os_app_backend_t table;

    lh_os_app_backend_system_init(&table);
    EXPECT_TRUE(lh_os_app_backend_get_now(&table) != nullptr);
    EXPECT_TRUE(lh_os_app_backend_get_wait(&table) != nullptr);
    EXPECT_TRUE(lh_os_app_backend_get_wake(&table) != nullptr);
    EXPECT_EQ(lh_os_app_backend_is_complete(&table), lh_bool_true);
}

#endif

} // namespace
