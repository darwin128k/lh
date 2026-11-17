/**
 * @file app.cpp
 * @brief Tests for `lh/os/app.h` — the portable half: the step and the quit flag.
 *
 * The application is what the window side of the tree sits on, and it is the
 * half that works without an operating system: a step, the timers it fires,
 * and a flag. What is checked here is that the step reaches the call it was
 * given with the state it was given, and that quitting is a flag and not a
 * call into a platform.
 */

#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/expect/death.h>
#include <lh/os/app.h>

namespace
{

/** What the step heard. */
struct heard
{
    lh_u32_t calls;
    lh_ptr user;
};

void
on_step(lh_ptr user)
{
    heard *h = lh_ptr_rcast(heard, user);

    ++h->calls;
    h->user = user;
}

TEST(os_app, a_new_app_is_empty_and_does_not_quit)
{
    lh_os_app_t app;

    lh_os_app_init(lh_addr_of(app));

    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_false);
    EXPECT_EQ(lh_os_app_get_step_fn(lh_addr_of(app)), lh_null);
    EXPECT_EQ(lh_os_app_get_step_context(lh_addr_of(app)), lh_null);
}

TEST(os_app, the_step_calls_what_it_was_given)
{
    lh_os_app_t app;
    heard h = {0u, lh_null};

    lh_os_app_init(lh_addr_of(app));
    lh_os_app_set_step_fn(lh_addr_of(app), on_step);
    lh_os_app_set_step_context(lh_addr_of(app), lh_addr_of(h));

    lh_os_app_step(lh_addr_of(app), 100u);

    EXPECT_EQ(h.calls, 1u);
    EXPECT_EQ(h.user, lh_addr_of(h));
}

TEST(os_app, an_app_without_a_step_says_nothing)
{
    lh_os_app_t app;

    lh_os_app_init(lh_addr_of(app));
    lh_os_app_step(lh_addr_of(app), 100u);

    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_false);
}

TEST(os_app, quitting_is_a_flag)
{
    lh_os_app_t app;

    lh_os_app_init(lh_addr_of(app));
    lh_os_app_quit(lh_addr_of(app));

    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_true);

    lh_os_app_set_quit(lh_addr_of(app), lh_bool_false);
    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_false);
}

/* -- The loop on a backend of the test's own ------------------------------- */

/* A loop backend that is not a system: a clock the test moves, a wait that counts and
   says "quit" after a number of turns, and a wake that is recorded. */
struct FakeLoop
{
    lh_os_app_backend_t table;
    lh_tick_t now;
    int waits;
    int quit_after;
    lh_tick_t last_timeout;
    int wakes;
    heard steps;
};

lh_tick_t
loop_now(lh_ptr context)
{
    return static_cast<FakeLoop *>(context)->now;
}

lh_bool_t
loop_wait(lh_ptr context, lh_tick_t timeout)
{
    auto *loop = static_cast<FakeLoop *>(context);

    loop->waits++;
    loop->last_timeout = timeout;
    loop->now += 10U;
    return lh_cast_static(lh_bool_t, loop->waits >= loop->quit_after);
}

lh_void
loop_wake(lh_ptr context)
{
    static_cast<FakeLoop *>(context)->wakes++;
}

void
loop_step(lh_ptr user)
{
    static_cast<heard *>(user)->calls++;
}

lh_void
fake_loop(lh_os_app_t *app, FakeLoop *loop)
{
    lh_os_app_backend_init(lh_addr_of(loop->table));
    lh_os_app_backend_set_now(lh_addr_of(loop->table), loop_now);
    lh_os_app_backend_set_wait(lh_addr_of(loop->table), loop_wait);
    lh_os_app_backend_set_wake(lh_addr_of(loop->table), loop_wake);
    lh_os_app_init(app);
    lh_os_app_install_backend(app, lh_addr_of(loop->table), loop);
}

TEST(os_app, a_new_app_has_no_backends)
{
    lh_os_app_t app;

    lh_os_app_init(lh_addr_of(app));
    EXPECT_EQ(lh_os_app_get_backend(lh_addr_of(app)), nullptr);
    EXPECT_EQ(lh_os_app_get_window_backend(lh_addr_of(app)), nullptr);
    EXPECT_EQ(lh_os_app_get_window(lh_addr_of(app)), nullptr);
    lh_os_app_deinit(lh_addr_of(app));
}

/* The loop is the step and a wait, turn after turn, until the backend says quit. */
TEST(os_app, run_steps_and_waits_until_the_backend_says_quit)
{
    FakeLoop loop{};
    lh_os_app_t app;

    fake_loop(lh_addr_of(app), lh_addr_of(loop));
    loop.quit_after = 3;
    lh_os_app_set_step_fn(lh_addr_of(app), loop_step);
    lh_os_app_set_step_context(lh_addr_of(app), lh_addr_of(loop.steps));

    lh_os_app_run(lh_addr_of(app));

    EXPECT_EQ(loop.waits, 3);
    EXPECT_EQ(loop.steps.calls, 3u);
    EXPECT_EQ(lh_os_app_is_quit(lh_addr_of(app)), lh_bool_true);
    lh_os_app_deinit(lh_addr_of(app));
}

/* A step that quits ends the loop before another wait. */
void
quitting_step(lh_ptr user)
{
    lh_os_app_quit(static_cast<lh_os_app_t *>(user));
}

TEST(os_app, a_step_that_quits_ends_the_loop_and_wakes_the_backend)
{
    FakeLoop loop{};
    lh_os_app_t app;

    fake_loop(lh_addr_of(app), lh_addr_of(loop));
    loop.quit_after = 100;
    lh_os_app_set_step_fn(lh_addr_of(app), quitting_step);
    lh_os_app_set_step_context(lh_addr_of(app), lh_addr_of(app));

    lh_os_app_run(lh_addr_of(app));

    EXPECT_EQ(loop.waits, 0);
    EXPECT_EQ(loop.wakes, 1);
    lh_os_app_deinit(lh_addr_of(app));
}

TEST(os_app, an_app_without_a_loop_backend_cannot_run)
{
    lh_os_app_t app;

    lh_os_app_init(lh_addr_of(app));
    LH_EXPECT_CHECK_DEATH(lh_os_app_run(lh_addr_of(app)));
}

/* An application that lives in windows stops when the last one is gone; one without a
   window backend — a remote that only reads its buttons — runs until it quits. */
TEST(os_app, running_needs_a_window_only_where_there_are_windows)
{
    lh_os_app_t app;
    lh_os_window_backend_t windows;

    lh_os_app_init(lh_addr_of(app));
    EXPECT_EQ(lh_os_app_is_running(lh_addr_of(app)), lh_bool_true);

    lh_os_window_backend_init(lh_addr_of(windows));
    lh_os_app_install_window_backend(lh_addr_of(app), lh_addr_of(windows), lh_null);
    EXPECT_EQ(lh_os_app_is_running(lh_addr_of(app)), lh_bool_false);

    lh_os_app_install_window_backend(lh_addr_of(app), lh_null, lh_null);
    lh_os_app_quit(lh_addr_of(app));
    EXPECT_EQ(lh_os_app_is_running(lh_addr_of(app)), lh_bool_false);
    lh_os_app_deinit(lh_addr_of(app));
}

} // namespace
