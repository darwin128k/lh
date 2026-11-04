#include <gtest/gtest.h>

#include <lh/core/attribute/unused.h>
#include <lh/exec/loop.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/wake.h>
#include <lh/os/thread.h>
#include <lh/os/tick.h>
#include <lh/core/timer.h>

#include <atomic>

namespace
{

struct counter
{
    std::atomic<int> steps{0};
    std::atomic<int> last{0};
    lh_exec_loop_t *loop = nullptr;
};

lh_void
count(lh_ptr user)
{
    auto *self = static_cast<counter *>(user);

    self->steps.fetch_add(1);
    if (self->loop != nullptr && lh_exec_loop_is_last_step(self->loop))
    {
        self->last.fetch_add(1);
    }
}

void
wait_for_steps(const counter &c, int at_least)
{
    for (int i = 0; i < 2000 && c.steps.load() < at_least; ++i)
    {
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
    }
}

lh_void
fire(lh_timer_t *, lh_ptr user)
{
    static_cast<std::atomic<int> *>(user)->fetch_add(1);
}

} // namespace

TEST(exec_loop, a_wake_runs_a_step_well_before_the_period)
{
    counter c;
    c.loop = lh_exec_loop_try_create(60000U, count, &c);

    ASSERT_NE(c.loop, nullptr);
    ASSERT_TRUE(lh_exec_loop_try_start(c.loop));
    wait_for_steps(c, 1);
    const int before = c.steps.load();
    lh_exec_loop_wake(c.loop);
    wait_for_steps(c, before + 1);
    EXPECT_GE(c.steps.load(), before + 1);
    lh_exec_loop_destroy(c.loop);
}

TEST(exec_loop, the_period_runs_steps_without_a_wake)
{
    counter c;
    c.loop = lh_exec_loop_try_create(10U, count, &c);

    ASSERT_NE(c.loop, nullptr);
    ASSERT_TRUE(lh_exec_loop_try_start(c.loop));
    LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(120U));
    EXPECT_GE(c.steps.load(), 4);
    lh_exec_loop_destroy(c.loop);
}

TEST(exec_loop, destroy_runs_one_last_step_that_sees_the_stop)
{
    counter c;
    c.loop = lh_exec_loop_try_create(60000U, count, &c);

    ASSERT_NE(c.loop, nullptr);
    ASSERT_TRUE(lh_exec_loop_try_start(c.loop));
    wait_for_steps(c, 1);
    lh_exec_loop_destroy(c.loop);
    EXPECT_EQ(c.last.load(), 1);
}

TEST(exec_loop, a_timer_cuts_a_long_period_short)
{
    std::atomic<int> fired{0};
    counter c;
    lh_timer_t timer;
    c.loop = lh_exec_loop_try_create(60000U, count, &c);

    ASSERT_NE(c.loop, nullptr);
    lh_timer_init(&timer);
    lh_timer_start(lh_exec_loop_get_timers_mut(c.loop), &timer, 20U, lh_bool_true, 0U, fire, &fired,
                   lh_os_tick_ms());
    ASSERT_TRUE(lh_exec_loop_try_start(c.loop));
    for (int i = 0; i < 2000 && fired.load() < 3; ++i)
    {
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
    }
    EXPECT_GE(fired.load(), 3);
    lh_exec_loop_destroy(c.loop);
}

TEST(exec_loop, a_loop_never_started_is_just_freed)
{
    counter c;
    lh_exec_loop_t *loop = lh_exec_loop_try_create(10U, count, &c);

    ASSERT_NE(loop, nullptr);
    lh_exec_loop_destroy(loop);
    EXPECT_EQ(c.steps.load(), 0);
    EXPECT_EQ(lh_exec_loop_try_create(0U, count, &c), nullptr);
    EXPECT_EQ(lh_exec_loop_try_create(10U, nullptr, &c), nullptr);
}

TEST(exec_loop, a_hint_also_runs_a_step)
{
    counter c;
    c.loop = lh_exec_loop_try_create(60000U, count, &c);

    ASSERT_NE(c.loop, nullptr);
    ASSERT_TRUE(lh_exec_loop_try_start(c.loop));
    wait_for_steps(c, 1);
    const int before = c.steps.load();
    /* A missed hint waits for the period: repeat until one lands. */
    for (int i = 0; i < 200 && c.steps.load() <= before; ++i)
    {
        lh_exec_loop_hint(c.loop);
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(5U));
    }
    EXPECT_GT(c.steps.load(), before);
    lh_exec_loop_destroy(c.loop);
}

/* A waiter that sleeps in poll on a wake signal, as a loop over sockets does. */
namespace
{

lh_void
poll_wait(lh_ptr waiter, lh_u32_t timeout_ms)
{
    auto *wake = static_cast<lh_os_wake_t *>(waiter);
    lh_os_net_socket_poll_t entry;

    lh_os_net_socket_poll_init(&entry, lh_os_wake_get_handle(wake), lh_os_net_socket_poll_read);
    if (lh_os_net_socket_poll(&entry, 1U, timeout_ms) > 0 &&
        lh_os_net_socket_poll_is_readable(&entry))
    {
        LH_ATTRIBUTE_UNUSED(lh_os_wake_reset(wake));
    }
}

lh_void
poll_wake(lh_ptr waiter)
{
    LH_ATTRIBUTE_UNUSED(lh_os_wake_signal(static_cast<lh_os_wake_t *>(waiter)));
}

} // namespace

TEST(exec_loop, a_waiter_on_a_wake_signal_runs_steps_and_stops)
{
    lh_os_wake_t wake;
    counter c;

    ASSERT_TRUE(lh_os_net_init());
    lh_os_wake_init(&wake);
    ASSERT_TRUE(lh_os_wake_open(&wake, 1000U));
    c.loop = lh_exec_loop_try_create_waiting(60000U, count, &c, poll_wait, poll_wake, &wake);
    ASSERT_NE(c.loop, nullptr);
    ASSERT_TRUE(lh_exec_loop_try_start(c.loop));
    wait_for_steps(c, 1);
    const int before = c.steps.load();
    lh_exec_loop_wake(c.loop);
    wait_for_steps(c, before + 1);
    EXPECT_GT(c.steps.load(), before);
    lh_exec_loop_destroy(c.loop);
    EXPECT_EQ(c.last.load(), 1);
    lh_os_wake_close(&wake);
    lh_os_net_deinit();
    EXPECT_EQ(lh_exec_loop_try_create_waiting(10U, count, &c, nullptr, poll_wake, &wake), nullptr);
}

/* A step that can be held inside a call, as one stuck on a network share is. */
namespace
{

struct stuck
{
    std::atomic<bool> hold{false};
    std::atomic<bool> inside{false};
    std::atomic<int> steps{0};
    std::atomic<int> released{0};
};

lh_void
stuck_step(lh_ptr user)
{
    auto *self = static_cast<stuck *>(user);

    self->steps.fetch_add(1);
    while (self->hold.load())
    {
        self->inside = true;
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
    }
}

lh_void
stuck_release(lh_ptr user)
{
    static_cast<stuck *>(user)->released.fetch_add(1);
}

} // namespace

TEST(exec_loop_try_stop, a_free_step_ends_within_the_timeout)
{
    stuck s;
    lh_exec_loop_t *loop = lh_exec_loop_try_create(60000U, stuck_step, &s);

    ASSERT_NE(loop, nullptr);
    EXPECT_TRUE(lh_exec_loop_try_stop(loop, 0U)); /* never started: nothing to wait for */
    ASSERT_TRUE(lh_exec_loop_try_start(loop));
    EXPECT_TRUE(lh_exec_loop_try_stop(loop, 5000U));
    EXPECT_TRUE(lh_exec_loop_is_ended(loop));
    EXPECT_EQ(lh_exec_loop_load_state(loop), lh_exec_loop_state_ended);
    /* It ended first: abandoning is refused, the owner destroys as usual. */
    EXPECT_FALSE(lh_exec_loop_try_abandon(loop, stuck_release, &s));
    lh_exec_loop_destroy(loop);
    EXPECT_EQ(s.released.load(), 0);
}

TEST(exec_loop_try_stop, a_stuck_step_times_out_and_ends_when_freed)
{
    stuck s;
    s.hold = true;
    lh_exec_loop_t *loop = lh_exec_loop_try_create(60000U, stuck_step, &s);

    ASSERT_NE(loop, nullptr);
    ASSERT_TRUE(lh_exec_loop_try_start(loop));
    for (int i = 0; i < 2000 && !s.inside.load(); ++i)
    {
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
    }
    EXPECT_FALSE(lh_exec_loop_try_stop(loop, 30U));
    EXPECT_EQ(lh_exec_loop_load_state(loop), lh_exec_loop_state_running);
    s.hold = false;
    EXPECT_TRUE(lh_exec_loop_try_stop(loop, 5000U));
    lh_exec_loop_destroy(loop);
}

TEST(exec_loop_try_abandon, the_thread_releases_and_the_owner_reaps)
{
    stuck s;
    s.hold = true;
    lh_exec_loop_t *loop = lh_exec_loop_try_create(60000U, stuck_step, &s);

    ASSERT_NE(loop, nullptr);
    ASSERT_TRUE(lh_exec_loop_try_start(loop));
    for (int i = 0; i < 2000 && !s.inside.load(); ++i)
    {
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
    }
    EXPECT_FALSE(lh_exec_loop_try_stop(loop, 10U));
    ASSERT_TRUE(lh_exec_loop_try_abandon(loop, stuck_release, &s));
    EXPECT_EQ(lh_exec_loop_load_state(loop), lh_exec_loop_state_abandoned);
    EXPECT_FALSE(lh_exec_loop_try_reap(loop)); /* still inside its step */
    EXPECT_EQ(s.released.load(), 0);
    s.hold = false;
    bool reaped = false;
    for (int i = 0; i < 5000 && !reaped; ++i)
    {
        reaped = lh_exec_loop_try_reap(loop);
        if (!reaped)
        {
            LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
        }
    }
    EXPECT_TRUE(reaped);
    EXPECT_EQ(s.released.load(), 1);
    EXPECT_GE(s.steps.load(), 2); /* the stuck step, then the last one */
}

TEST(exec_loop_try_abandon, works_for_a_loop_with_a_waiter)
{
    lh_os_wake_t wake;
    stuck s;
    s.hold = true;

    ASSERT_TRUE(lh_os_net_init());
    lh_os_wake_init(&wake);
    ASSERT_TRUE(lh_os_wake_open(&wake, 1000U));
    lh_exec_loop_t *loop =
        lh_exec_loop_try_create_waiting(60000U, stuck_step, &s, poll_wait, poll_wake, &wake);
    ASSERT_NE(loop, nullptr);
    ASSERT_TRUE(lh_exec_loop_try_start(loop));
    for (int i = 0; i < 2000 && !s.inside.load(); ++i)
    {
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
    }
    EXPECT_FALSE(lh_exec_loop_try_stop(loop, 10U));
    ASSERT_TRUE(lh_exec_loop_try_abandon(loop, stuck_release, &s));
    s.hold = false;
    bool reaped = false;
    for (int i = 0; i < 5000 && !reaped; ++i)
    {
        reaped = lh_exec_loop_try_reap(loop);
        if (!reaped)
        {
            LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
        }
    }
    EXPECT_TRUE(reaped);
    EXPECT_EQ(s.released.load(), 1);
    lh_os_wake_close(&wake);
    lh_os_net_deinit();
}
