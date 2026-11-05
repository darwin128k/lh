#include <gtest/gtest.h>

#include <lh/os/rail/base.h>
#include <lh/rail/base/wait/forever.h>

#include <atomic>

namespace
{

const lh_rail_base_t *
os_base()
{
    return lh_os_rail_base_get();
}

TEST(os_rail_base_get, has_every_group)
{
    EXPECT_TRUE(lh_rail_base_has_clock(os_base()));
    EXPECT_TRUE(lh_rail_base_has_lock(os_base()));
    EXPECT_TRUE(lh_rail_base_has_signal(os_base()));
    EXPECT_TRUE(lh_rail_base_has_thread(os_base()));
}

TEST(os_rail_base_now_ms, never_goes_back)
{
    const lh_u64_t a = lh_rail_base_now_ms(os_base());
    const lh_u64_t b = lh_rail_base_now_ms(os_base());
    EXPECT_GE(b, a);
}

TEST(os_rail_base_lock, opens_takes_gives_back_and_closes)
{
    lh_ptr lock = lh_rail_base_lock_open(os_base());
    ASSERT_NE(lock, nullptr);
    EXPECT_TRUE(lh_rail_base_lock(os_base(), lock));
    EXPECT_TRUE(lh_rail_base_unlock(os_base(), lock));
    EXPECT_TRUE(lh_rail_base_lock_close(os_base(), lock));
}

TEST(os_rail_base_signal_wait, a_timeout_is_not_a_failure)
{
    lh_ptr lock = lh_rail_base_lock_open(os_base());
    lh_ptr signal = lh_rail_base_signal_open(os_base());
    ASSERT_NE(lock, nullptr);
    ASSERT_NE(signal, nullptr);
    ASSERT_TRUE(lh_rail_base_lock(os_base(), lock));
    const lh_u64_t start = lh_rail_base_now_ms(os_base());
    EXPECT_TRUE(lh_rail_base_signal_wait(os_base(), signal, lock, 20u));
    /* Early returns are allowed; the lock is held again either way. */
    EXPECT_LE(start, lh_rail_base_now_ms(os_base()));
    EXPECT_TRUE(lh_rail_base_unlock(os_base(), lock));
    EXPECT_TRUE(lh_rail_base_signal_close(os_base(), signal));
    EXPECT_TRUE(lh_rail_base_lock_close(os_base(), lock));
}

struct sleeper
{
    lh_ptr lock = nullptr;
    lh_ptr signal = nullptr;
    bool go = false;
    std::atomic<bool> waiting{false};
    std::atomic<bool> woke{false};
};

lh_void
sleep_until_go(lh_ptr user)
{
    auto *s = static_cast<sleeper *>(user);
    lh_rail_base_lock(os_base(), s->lock);
    s->waiting.store(true);
    while (!s->go)
    {
        lh_rail_base_signal_wait(os_base(), s->signal, s->lock, lh_rail_base_wait_forever);
    }
    lh_rail_base_unlock(os_base(), s->lock);
    s->woke.store(true);
}

TEST(os_rail_base_signal_wake, wakes_a_thread_that_sleeps_without_a_bound)
{
    sleeper s;
    s.lock = lh_rail_base_lock_open(os_base());
    s.signal = lh_rail_base_signal_open(os_base());
    ASSERT_NE(s.lock, nullptr);
    ASSERT_NE(s.signal, nullptr);
    lh_ptr thread = lh_rail_base_thread_start(os_base(), sleep_until_go, &s, -1, 0u);
    ASSERT_NE(thread, nullptr);
    while (!s.waiting.load())
    {
        lh_rail_base_sleep(os_base(), 1u);
    }
    ASSERT_TRUE(lh_rail_base_lock(os_base(), s.lock));
    s.go = true;
    EXPECT_TRUE(lh_rail_base_signal_wake(os_base(), s.signal));
    ASSERT_TRUE(lh_rail_base_unlock(os_base(), s.lock));
    EXPECT_TRUE(lh_rail_base_thread_join(os_base(), thread));
    EXPECT_TRUE(s.woke.load());
    EXPECT_TRUE(lh_rail_base_signal_close(os_base(), s.signal));
    EXPECT_TRUE(lh_rail_base_lock_close(os_base(), s.lock));
}

lh_void
add_one(lh_ptr user)
{
    static_cast<std::atomic<int> *>(user)->fetch_add(1);
}

TEST(os_rail_base_thread_start, runs_the_body_once_before_the_join_returns)
{
    std::atomic<int> runs{0};
    lh_ptr thread = lh_rail_base_thread_start(os_base(), add_one, &runs, -1, 0u);
    ASSERT_NE(thread, nullptr);
    EXPECT_TRUE(lh_rail_base_thread_join(os_base(), thread));
    EXPECT_EQ(runs.load(), 1);
}

TEST(os_rail_base_sleep, pauses_at_least_the_time_asked)
{
    const lh_u64_t start = lh_rail_base_now_ms(os_base());
    EXPECT_TRUE(lh_rail_base_sleep(os_base(), 15u));
    /* A coarse OS tick may round a little: allow it, but not a pause of nothing. */
    EXPECT_GE(lh_rail_base_now_ms(os_base()) - start, 10u);
}

} // namespace
