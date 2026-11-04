#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/expect/death.h>
#include <lh/runtime/terminate.h>
#include <lh/runtime/terminate/handler/initializer.h>

#include <thread>

namespace
{

/* A stand-in compared by address; no test calls it. */
void
test_terminate_b(lh_self_ptr self)
{
    (void)self;
}

/* The call a fresh thread finds in its own handler. */
lh_runtime_terminate_cb
call_cb_on_new_thread(void)
{
    lh_runtime_terminate_cb seen = lh_null;
    std::thread([&]
                { seen = lh_runtime_terminate_handler_get_call_cb(lh_runtime_terminate_get()); })
        .join();
    return seen;
}

TEST(runtime_terminate, unless_returns_when_the_invariant_holds)
{
    lh_runtime_terminate_unless(lh_bool_true);
    SUCCEED();
}

TEST(runtime_terminate_get, is_the_same_object_read_only)
{
    EXPECT_EQ(lh_runtime_terminate_get(), lh_runtime_terminate_get_mut());
}

/* One object for the whole program: a change on one thread is seen on all. */
TEST(runtime_terminate_get_mut, is_one_handler_for_every_thread)
{
    lh_runtime_terminate_handler_t saved;
    lh_runtime_terminate_handler_assign(&saved, lh_runtime_terminate_get());
    lh_runtime_terminate_handler_set_call_cb(lh_runtime_terminate_get_mut(), test_terminate_b);
    EXPECT_EQ(call_cb_on_new_thread(), test_terminate_b);
    lh_runtime_terminate_handler_assign(lh_runtime_terminate_get_mut(), &saved);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(runtime_terminate_death, unless_terminates_when_the_invariant_breaks)
{
    EXPECT_DEATH(lh_runtime_terminate_unless(lh_bool_false), "");
}

/* With no call installed the process still stops: a compiler trap. */
void
terminate_with_an_empty_handler(void)
{
    lh_runtime_terminate_handler_set_call_cb(lh_runtime_terminate_get_mut(), lh_null);
    lh_runtime_terminate();
}

TEST(runtime_terminate_death, an_empty_handler_traps)
{
    EXPECT_DEATH(terminate_with_an_empty_handler(), "");
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
