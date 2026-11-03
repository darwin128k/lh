#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/expect/death.h>
#include <lh/runtime/terminate.h>

#include <thread>

namespace
{

/* Stand-ins compared by address; no test calls them. */
void
test_terminate_a(void)
{
}

void
test_terminate_b(void)
{
}

/* The handler a fresh thread finds as its own. */
lh_runtime_terminate_cb
terminate_on_new_thread(void)
{
    lh_runtime_terminate_cb seen = lh_null;
    std::thread([&] { seen = lh_runtime_terminate_get(); }).join();
    return seen;
}

TEST(runtime_terminate_set_process, roundtrips_through_get_process)
{
    lh_runtime_terminate_cb saved = lh_runtime_terminate_get_process();
    lh_runtime_terminate_set_process(test_terminate_a);
    EXPECT_EQ(lh_runtime_terminate_get_process(), test_terminate_a);
    lh_runtime_terminate_set_process(saved);
}

#if (LH_LIBRARY_OPTION_THREAD_LOCAL == LH_LIBRARY_OPTION_ON)

TEST(runtime_terminate_process, seeds_a_thread_on_its_first_use)
{
    /* Seed this thread first: the change below must not reach it. */
    lh_runtime_terminate_cb own = lh_runtime_terminate_get();
    lh_runtime_terminate_cb saved = lh_runtime_terminate_get_process();
    lh_runtime_terminate_set_process(test_terminate_a);
    EXPECT_EQ(terminate_on_new_thread(), test_terminate_a);
    EXPECT_EQ(lh_runtime_terminate_get(), own);
    lh_runtime_terminate_set_process(saved);
}

TEST(runtime_terminate_set, changes_the_calling_thread_only)
{
    lh_runtime_terminate_cb saved = lh_runtime_terminate_get();
    lh_runtime_terminate_set(test_terminate_b);
    EXPECT_EQ(lh_runtime_terminate_get(), test_terminate_b);
    EXPECT_EQ(terminate_on_new_thread(), lh_runtime_terminate_get_process());
    lh_runtime_terminate_set(saved);
}

#else

/* THREAD_LOCAL=OFF: the thread handler is one object for every thread. */
TEST(runtime_terminate_set, is_one_handler_for_every_thread)
{
    lh_runtime_terminate_cb saved = lh_runtime_terminate_get();
    lh_runtime_terminate_set(test_terminate_b);
    EXPECT_EQ(terminate_on_new_thread(), test_terminate_b);
    lh_runtime_terminate_set(saved);
}

#endif /* LH_LIBRARY_OPTION_THREAD_LOCAL */

TEST(runtime_terminate, unless_returns_when_the_invariant_holds)
{
    lh_runtime_terminate_unless(lh_bool_true);
    SUCCEED();
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(runtime_terminate_death, unless_terminates_when_the_invariant_breaks)
{
    EXPECT_DEATH(lh_runtime_terminate_unless(lh_bool_false), "");
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
