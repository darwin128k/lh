#include <gtest/gtest.h>

#include <lh/os/module/hook.h>
#include <lh/os/thread.h>
#include <lh/core/attribute/unused.h>

#include <string>

namespace
{

/* What the chain did with the call: the order the handlers ran in. */
struct trace
{
    std::string order;
};

/* A handler that writes its letter (its context) and goes on. */
lh_bool_t
pass(lh_ptr context, lh_ptr call)
{
    static_cast<trace *>(call)->order += *static_cast<const char *>(context);
    return lh_bool_true;
}

/* A handler that writes its letter and stops the chain. */
lh_bool_t
stop(lh_ptr context, lh_ptr call)
{
    static_cast<trace *>(call)->order += *static_cast<const char *>(context);
    return lh_bool_false;
}

/* A handler that overruns any budget of a millisecond, then stops. */
lh_bool_t
slow(lh_ptr context, lh_ptr call)
{
    static_cast<trace *>(call)->order += *static_cast<const char *>(context);
    LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(20U));
    return lh_bool_false;
}

char a = 'a';
char b = 'b';
char c = 'c';

} // namespace

TEST(lh_os_module_hook, last_added_runs_first)
{
    lh_os_module_hook_t hook;
    trace call;
    const lh_char_t *stopped_by = "x";
    lh_bool_t overran = lh_bool_true;
    int owner = 0;

    lh_os_module_hook_init(&hook);
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &owner, "A", pass, &a));
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &owner, "B", pass, &b));
    EXPECT_TRUE(lh_os_module_hook_call(&hook, &call, 0U, &stopped_by, &overran));
    EXPECT_EQ(call.order, "ba");
    EXPECT_EQ(stopped_by, nullptr);
    EXPECT_FALSE(overran);
}

TEST(lh_os_module_hook, a_handler_stops_the_chain_and_is_named)
{
    lh_os_module_hook_t hook;
    trace call;
    const lh_char_t *stopped_by = lh_null;
    int owner = 0;

    lh_os_module_hook_init(&hook);
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &owner, "A", pass, &a));
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &owner, "B", stop, &b));
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &owner, "C", pass, &c));
    EXPECT_FALSE(lh_os_module_hook_call(&hook, &call, 0U, &stopped_by, lh_null));
    EXPECT_EQ(call.order, "cb");
    EXPECT_STREQ(stopped_by, "B");
}

TEST(lh_os_module_hook, an_overrunning_handler_is_dropped)
{
    lh_os_module_hook_t hook;
    trace call;
    lh_bool_t overran = lh_bool_false;
    const lh_char_t *stopped_by = "x";
    int owner = 0;

    lh_os_module_hook_init(&hook);
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &owner, "A", pass, &a));
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &owner, "S", slow, &b));
    /* The slow one stops the call, but it overran: it is dropped and does not count. */
    EXPECT_TRUE(lh_os_module_hook_call(&hook, &call, 1000U, &stopped_by, &overran));
    EXPECT_EQ(call.order, "ba");
    EXPECT_TRUE(overran);
    EXPECT_EQ(stopped_by, nullptr);
    EXPECT_EQ(lh_os_module_hook_get_count(&hook), 1U);
}

TEST(lh_os_module_hook, removal_by_handler_and_by_owner)
{
    lh_os_module_hook_t hook;
    trace call;
    int one = 0;
    int two = 0;

    lh_os_module_hook_init(&hook);
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &one, lh_null, pass, &a));
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &two, lh_null, pass, &b));
    ASSERT_TRUE(lh_os_module_hook_try_add(&hook, &one, lh_null, pass, &c));
    EXPECT_FALSE(lh_os_module_hook_try_remove(&hook, &two, pass, &a));
    EXPECT_TRUE(lh_os_module_hook_try_remove(&hook, &one, pass, &a));
    EXPECT_FALSE(lh_os_module_hook_try_remove(&hook, &one, pass, &a));
    lh_os_module_hook_remove_owner(&hook, &one);
    EXPECT_TRUE(lh_os_module_hook_call(&hook, &call, 0U, lh_null, lh_null));
    EXPECT_EQ(call.order, "b");
}

TEST(lh_os_module_hook, the_chain_is_bounded)
{
    lh_os_module_hook_t hook;
    int owner = 0;

    lh_os_module_hook_init(&hook);
    EXPECT_FALSE(lh_os_module_hook_try_add(&hook, &owner, lh_null, lh_null, lh_null));
    for (lh_usize_t i = 0U; i < LH_OS_MODULE_HOOK_SIZE; ++i)
    {
        EXPECT_TRUE(lh_os_module_hook_try_add(&hook, &owner, lh_null, pass, &a));
    }
    EXPECT_FALSE(lh_os_module_hook_try_add(&hook, &owner, lh_null, pass, &a));
}
