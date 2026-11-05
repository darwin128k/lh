#include <gtest/gtest.h>

#include <lh/rail/base.h>
#include <lh/rail/base/initializer.h>
#include <lh/rail/base/wait/forever.h>
#include <lh/expect/death.h>

namespace
{

/* A base made of fakes: every call records what it was given. */
struct record
{
    int calls = 0;
    lh_ptr handle = nullptr;
    lh_ptr lock = nullptr;
    lh_u32_t ms = 0;
    lh_s32_t cpu = 0;
    lh_u32_t flags = 0;
    lh_rail_base_run_cb run = nullptr;
    lh_ptr user = nullptr;
};

record g_record;
int g_lock_object;
int g_signal_object;
int g_thread_object;

lh_u64_t
fake_now(void)
{
    ++g_record.calls;
    return 1234u;
}

lh_ptr
fake_lock_open(void)
{
    ++g_record.calls;
    return &g_lock_object;
}

lh_ptr
fake_signal_open(void)
{
    ++g_record.calls;
    return &g_signal_object;
}

lh_bool_t
fake_handle(lh_ptr handle)
{
    ++g_record.calls;
    g_record.handle = handle;
    return lh_bool_true;
}

lh_bool_t
fake_wait(lh_ptr signal, lh_ptr lock, lh_u32_t timeout_ms)
{
    ++g_record.calls;
    g_record.handle = signal;
    g_record.lock = lock;
    g_record.ms = timeout_ms;
    return lh_bool_true;
}

lh_ptr
fake_start(lh_rail_base_run_cb run, lh_ptr user, lh_s32_t cpu, lh_u32_t flags)
{
    ++g_record.calls;
    g_record.run = run;
    g_record.user = user;
    g_record.cpu = cpu;
    g_record.flags = flags;
    return &g_thread_object;
}

lh_bool_t
fake_sleep(lh_u32_t milliseconds)
{
    ++g_record.calls;
    g_record.ms = milliseconds;
    return lh_bool_true;
}

lh_void
fake_body(lh_ptr user)
{
    (void)user;
}

const lh_rail_base_t g_full = lh_rail_base_initializer(
    fake_now, fake_lock_open, fake_handle, fake_handle, fake_handle, fake_signal_open, fake_handle,
    fake_wait, fake_handle, fake_start, fake_handle, fake_sleep);

TEST(rail_base_empty_initializer, has_no_group)
{
    const lh_rail_base_t base = lh_rail_base_empty_initializer();
    EXPECT_FALSE(lh_rail_base_has_clock(&base));
    EXPECT_FALSE(lh_rail_base_has_lock(&base));
    EXPECT_FALSE(lh_rail_base_has_signal(&base));
    EXPECT_FALSE(lh_rail_base_has_thread(&base));
}

TEST(rail_base_initializer, places_every_call_in_field_order)
{
    EXPECT_EQ(lh_rail_base_get_now_cb(&g_full), fake_now);
    EXPECT_EQ(lh_rail_base_get_lock_open_cb(&g_full), fake_lock_open);
    EXPECT_EQ(lh_rail_base_get_lock_close_cb(&g_full), fake_handle);
    EXPECT_EQ(lh_rail_base_get_lock_cb(&g_full), fake_handle);
    EXPECT_EQ(lh_rail_base_get_unlock_cb(&g_full), fake_handle);
    EXPECT_EQ(lh_rail_base_get_signal_open_cb(&g_full), fake_signal_open);
    EXPECT_EQ(lh_rail_base_get_signal_close_cb(&g_full), fake_handle);
    EXPECT_EQ(lh_rail_base_get_signal_wait_cb(&g_full), fake_wait);
    EXPECT_EQ(lh_rail_base_get_signal_wake_cb(&g_full), fake_handle);
    EXPECT_EQ(lh_rail_base_get_thread_start_cb(&g_full), fake_start);
    EXPECT_EQ(lh_rail_base_get_thread_join_cb(&g_full), fake_handle);
    EXPECT_EQ(lh_rail_base_get_sleep_cb(&g_full), fake_sleep);
    EXPECT_TRUE(lh_rail_base_has_clock(&g_full));
    EXPECT_TRUE(lh_rail_base_has_lock(&g_full));
    EXPECT_TRUE(lh_rail_base_has_signal(&g_full));
    EXPECT_TRUE(lh_rail_base_has_thread(&g_full));
}

TEST(rail_base_has, one_missing_call_removes_its_group_only)
{
    lh_rail_base_t base = g_full;
    lh_rail_base_set_unlock_cb(&base, lh_null);
    EXPECT_FALSE(lh_rail_base_has_lock(&base));
    EXPECT_TRUE(lh_rail_base_has_signal(&base));

    base = g_full;
    lh_rail_base_set_signal_wait_cb(&base, lh_null);
    EXPECT_FALSE(lh_rail_base_has_signal(&base));
    EXPECT_TRUE(lh_rail_base_has_lock(&base));

    base = g_full;
    lh_rail_base_set_sleep_cb(&base, lh_null);
    EXPECT_FALSE(lh_rail_base_has_thread(&base));
    EXPECT_TRUE(lh_rail_base_has_clock(&base));

    base = g_full;
    lh_rail_base_set_now_cb(&base, lh_null);
    EXPECT_FALSE(lh_rail_base_has_clock(&base));
    EXPECT_TRUE(lh_rail_base_has_thread(&base));
}

TEST(rail_base_set, each_field_stands_alone)
{
    lh_rail_base_t base = lh_rail_base_empty_initializer();
    lh_rail_base_set_lock_open_cb(&base, fake_lock_open);
    lh_rail_base_set_lock_close_cb(&base, fake_handle);
    lh_rail_base_set_lock_cb(&base, fake_handle);
    lh_rail_base_set_unlock_cb(&base, fake_handle);
    EXPECT_TRUE(lh_rail_base_has_lock(&base));
    EXPECT_FALSE(lh_rail_base_has_signal(&base));
    lh_rail_base_set_signal_open_cb(&base, fake_signal_open);
    lh_rail_base_set_signal_close_cb(&base, fake_handle);
    lh_rail_base_set_signal_wait_cb(&base, fake_wait);
    lh_rail_base_set_signal_wake_cb(&base, fake_handle);
    EXPECT_TRUE(lh_rail_base_has_signal(&base));
    lh_rail_base_set_thread_start_cb(&base, fake_start);
    lh_rail_base_set_thread_join_cb(&base, fake_handle);
    EXPECT_FALSE(lh_rail_base_has_thread(&base));
    lh_rail_base_set_sleep_cb(&base, fake_sleep);
    EXPECT_TRUE(lh_rail_base_has_thread(&base));
}

TEST(rail_base_assign, copies_every_call)
{
    lh_rail_base_t base = lh_rail_base_empty_initializer();
    lh_rail_base_assign(&base, &g_full);
    EXPECT_EQ(lh_rail_base_get_now_cb(&base), fake_now);
    EXPECT_EQ(lh_rail_base_get_signal_wait_cb(&base), fake_wait);
    EXPECT_EQ(lh_rail_base_get_thread_start_cb(&base), fake_start);
    EXPECT_EQ(lh_rail_base_get_sleep_cb(&base), fake_sleep);
    const lh_rail_base_t empty = lh_rail_base_empty_initializer();
    lh_rail_base_assign(&base, &empty);
    EXPECT_FALSE(lh_rail_base_has_clock(&base));
    EXPECT_FALSE(lh_rail_base_has_thread(&base));
}

TEST(rail_base_calls, pass_their_arguments_through)
{
    g_record = record{};
    EXPECT_EQ(lh_rail_base_now_ms(&g_full), 1234u);
    EXPECT_EQ(lh_rail_base_lock_open(&g_full), &g_lock_object);
    EXPECT_EQ(lh_rail_base_signal_open(&g_full), &g_signal_object);
    EXPECT_EQ(g_record.calls, 3);

    EXPECT_TRUE(lh_rail_base_lock(&g_full, &g_lock_object));
    EXPECT_EQ(g_record.handle, &g_lock_object);
    EXPECT_TRUE(lh_rail_base_signal_wake(&g_full, &g_signal_object));
    EXPECT_EQ(g_record.handle, &g_signal_object);

    EXPECT_TRUE(lh_rail_base_signal_wait(&g_full, &g_signal_object, &g_lock_object,
                                         lh_rail_base_wait_forever));
    EXPECT_EQ(g_record.handle, &g_signal_object);
    EXPECT_EQ(g_record.lock, &g_lock_object);
    EXPECT_EQ(g_record.ms, 0xFFFFFFFFu);

    int user = 0;
    EXPECT_EQ(lh_rail_base_thread_start(&g_full, fake_body, &user, 2, 1u), &g_thread_object);
    EXPECT_EQ(g_record.run, fake_body);
    EXPECT_EQ(g_record.user, &user);
    EXPECT_EQ(g_record.cpu, 2);
    EXPECT_EQ(g_record.flags, 1u);

    EXPECT_TRUE(lh_rail_base_thread_join(&g_full, &g_thread_object));
    EXPECT_EQ(g_record.handle, &g_thread_object);
    EXPECT_TRUE(lh_rail_base_sleep(&g_full, 7u));
    EXPECT_EQ(g_record.ms, 7u);
    EXPECT_TRUE(lh_rail_base_unlock(&g_full, &g_lock_object));
    EXPECT_TRUE(lh_rail_base_signal_close(&g_full, &g_signal_object));
    EXPECT_TRUE(lh_rail_base_lock_close(&g_full, &g_lock_object));
    EXPECT_EQ(g_record.handle, &g_lock_object);
    EXPECT_EQ(g_record.calls, 12);
}

TEST(rail_base_death, rejects_null_base)
{
    LH_EXPECT_DEATH(lh_rail_base_get_now_cb(lh_null));
    LH_EXPECT_DEATH(lh_rail_base_has_thread(lh_null));
}

} // namespace
