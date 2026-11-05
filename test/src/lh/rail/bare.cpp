#include <gtest/gtest.h>

#include <lh/core/timer.h>
#include <lh/rail/bare.h>
#include <lh/rail/base/initializer.h>
#include <lh/rail/base/wait/forever.h>
#include <lh/rail/loop.h>
#include <lh/rail/runtime/base.h>
#include <lh/rail/system/sim/cpu.h>

namespace
{

/* The board's 1 ms timer interrupt, in the CPU model: every wait for an
   interrupt is one millisecond. */
lh_void
timer_interrupt(lh_ptr user)
{
    (void)user;
    lh_rail_bare_tick();
}

/* Runs a test on the base without an OS: the model ticks, the rail slot holds
   the bare base; both are put back afterwards. */
class rail_bare : public ::testing::Test
{
  protected:
    void
    SetUp() override
    {
        lh_rail_base_assign(&saved, lh_rail_runtime_base_get());
        lh_rail_base_assign(lh_rail_runtime_base_get_mut(), lh_rail_bare_get());
        lh_rail_system_sim_cpu_set_interrupt(timer_interrupt, lh_null);
    }

    void
    TearDown() override
    {
        lh_rail_system_sim_cpu_set_interrupt(lh_null, lh_null);
        lh_rail_base_assign(lh_rail_runtime_base_get_mut(), &saved);
    }

    lh_rail_base_t saved = lh_rail_base_empty_initializer();
};

TEST_F(rail_bare, has_clock_lock_and_signal_but_no_threads)
{
    EXPECT_TRUE(lh_rail_base_has_clock(lh_rail_bare_get()));
    EXPECT_TRUE(lh_rail_base_has_lock(lh_rail_bare_get()));
    EXPECT_TRUE(lh_rail_base_has_signal(lh_rail_bare_get()));
    EXPECT_FALSE(lh_rail_base_has_thread(lh_rail_bare_get()));
}

TEST_F(rail_bare, a_tick_is_one_millisecond)
{
    const lh_u64_t start = lh_rail_bare_now_ms();
    lh_rail_bare_tick();
    lh_rail_bare_tick();
    EXPECT_EQ(lh_rail_bare_now_ms() - start, 2u);
}

TEST_F(rail_bare, sections_nest_and_restore_the_mask_they_found)
{
    ASSERT_FALSE(lh_rail_system_sim_cpu_is_masked());
    const lh_usize_t outer = lh_rail_bare_critical_enter();
    EXPECT_TRUE(lh_rail_system_sim_cpu_is_masked());
    const lh_usize_t inner = lh_rail_bare_critical_enter();
    lh_rail_bare_critical_leave(inner);
    EXPECT_TRUE(lh_rail_system_sim_cpu_is_masked());
    lh_rail_bare_critical_leave(outer);
    EXPECT_FALSE(lh_rail_system_sim_cpu_is_masked());
    EXPECT_FALSE(lh_critical_is_empty(lh_rail_bare_critical_get()));
}

TEST_F(rail_bare, a_lock_masks_while_held)
{
    lh_ptr lock = lh_rail_bare_lock_open();
    ASSERT_NE(lock, nullptr);
    EXPECT_TRUE(lh_rail_bare_lock(lock));
    EXPECT_TRUE(lh_rail_system_sim_cpu_is_masked());
    EXPECT_TRUE(lh_rail_bare_unlock(lock));
    EXPECT_FALSE(lh_rail_system_sim_cpu_is_masked());
    EXPECT_TRUE(lh_rail_bare_lock_close(lock));
}

TEST_F(rail_bare, a_wait_without_a_wake_lasts_its_timeout)
{
    lh_ptr lock = lh_rail_bare_lock_open();
    lh_ptr signal = lh_rail_bare_signal_open();
    ASSERT_NE(lock, nullptr);
    ASSERT_NE(signal, nullptr);
    ASSERT_TRUE(lh_rail_bare_lock(lock));
    const lh_u64_t start = lh_rail_bare_now_ms();
    EXPECT_TRUE(lh_rail_bare_signal_wait(signal, lock, 15u));
    EXPECT_EQ(lh_rail_bare_now_ms() - start, 15u);
    EXPECT_TRUE(lh_rail_system_sim_cpu_is_masked());
    ASSERT_TRUE(lh_rail_bare_unlock(lock));
    EXPECT_TRUE(lh_rail_bare_signal_close(signal));
    EXPECT_TRUE(lh_rail_bare_lock_close(lock));
}

/* An interrupt handler that raises the signal on its third tick. */
struct raiser
{
    lh_ptr signal = nullptr;
    int ticks = 0;
};

lh_void
raise_on_third_tick(lh_ptr user)
{
    auto *r = static_cast<raiser *>(user);
    lh_rail_bare_tick();
    if (++r->ticks == 3)
    {
        lh_rail_bare_signal_wake(r->signal);
    }
}

TEST_F(rail_bare, a_wake_from_an_interrupt_ends_the_wait)
{
    raiser r;
    lh_ptr lock = lh_rail_bare_lock_open();
    r.signal = lh_rail_bare_signal_open();
    lh_rail_system_sim_cpu_set_interrupt(raise_on_third_tick, &r);
    ASSERT_TRUE(lh_rail_bare_lock(lock));
    const lh_u64_t start = lh_rail_bare_now_ms();
    EXPECT_TRUE(lh_rail_bare_signal_wait(r.signal, lock, lh_rail_base_wait_forever));
    EXPECT_EQ(lh_rail_bare_now_ms() - start, 3u);
    ASSERT_TRUE(lh_rail_bare_unlock(lock));
    /* The wake was consumed: a second wait lasts its timeout. */
    ASSERT_TRUE(lh_rail_bare_lock(lock));
    EXPECT_TRUE(lh_rail_bare_signal_wait(r.signal, lock, 5u));
    EXPECT_GE(lh_rail_bare_now_ms() - start, 8u);
    ASSERT_TRUE(lh_rail_bare_unlock(lock));
    lh_rail_bare_signal_close(r.signal);
    lh_rail_bare_lock_close(lock);
}

TEST_F(rail_bare, a_wake_before_the_wait_ends_it_at_once)
{
    lh_ptr lock = lh_rail_bare_lock_open();
    lh_ptr signal = lh_rail_bare_signal_open();
    lh_rail_bare_signal_wake(signal);
    ASSERT_TRUE(lh_rail_bare_lock(lock));
    const lh_u64_t start = lh_rail_bare_now_ms();
    EXPECT_TRUE(lh_rail_bare_signal_wait(signal, lock, 1000u));
    EXPECT_EQ(lh_rail_bare_now_ms(), start);
    ASSERT_TRUE(lh_rail_bare_unlock(lock));
    lh_rail_bare_signal_close(signal);
    lh_rail_bare_lock_close(lock);
}

/* A loop driven from "main" by passes, on the base without an OS. */
struct board
{
    lh_rail_loop_t *loop = nullptr;
    int steps = 0;
    int fired = 0;
};

lh_void
board_step(lh_ptr user)
{
    auto *b = static_cast<board *>(user);
    ++b->steps;
    if (b->fired > 0 && !lh_rail_loop_is_last_step(b->loop))
    {
        lh_rail_loop_ask_stop(b->loop);
    }
}

lh_void
board_timer(lh_timer_t *, lh_ptr user)
{
    ++static_cast<board *>(user)->fired;
}

TEST_F(rail_bare, a_loop_runs_by_passes_without_a_thread)
{
    board b;
    b.loop = lh_rail_loop_try_create(10U, board_step, &b);
    ASSERT_NE(b.loop, nullptr);
    EXPECT_FALSE(lh_rail_loop_try_start(b.loop));

    lh_timer_t timer;
    lh_timer_init(&timer);
    const lh_u64_t start = lh_rail_bare_now_ms();
    lh_timer_start(lh_rail_loop_get_timers_mut(b.loop), &timer, 25U, lh_bool_false, 0U, board_timer,
                   &b, static_cast<lh_tick_t>(start));

    for (int guard = 0; guard < 1000 && !lh_rail_loop_is_last_step(b.loop); ++guard)
    {
        lh_rail_loop_sleep(b.loop, lh_rail_loop_pass(b.loop));
    }

    EXPECT_TRUE(lh_rail_loop_is_last_step(b.loop));
    EXPECT_EQ(b.fired, 1);
    const lh_u64_t elapsed = lh_rail_bare_now_ms() - start;
    EXPECT_GE(elapsed, 25u);
    EXPECT_LT(elapsed, 40u);
    /* Steps at 0, 10, 20, 25 (the timer), then the last one. */
    EXPECT_GE(b.steps, 4);
    EXPECT_LE(b.steps, 6);
    lh_rail_loop_destroy(b.loop);
}

} // namespace
