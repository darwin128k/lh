#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/runtime/terminate/handler.h>
#include <lh/runtime/terminate/handler/initializer.h>

namespace
{

int g_calls = 0;
lh_ptr g_seen = lh_null;

void
count_call(lh_self_ptr self)
{
    ++g_calls;
    g_seen = self;
}

TEST(runtime_terminate_handler_initializer, holds_the_call_and_context)
{
    int context = 0;
    const lh_runtime_terminate_handler_t handler =
        lh_runtime_terminate_handler_initializer(count_call, &context);
    EXPECT_EQ(lh_runtime_terminate_handler_get_call_cb(&handler), count_call);
    EXPECT_EQ(lh_runtime_terminate_handler_get_context(&handler), &context);
}

TEST(runtime_terminate_handler_set, each_field_stands_alone)
{
    int context = 0;
    lh_runtime_terminate_handler_t handler =
        lh_runtime_terminate_handler_initializer(lh_null, lh_null);
    lh_runtime_terminate_handler_set_call_cb(&handler, count_call);
    EXPECT_EQ(lh_runtime_terminate_handler_get_call_cb(&handler), count_call);
    EXPECT_EQ(lh_runtime_terminate_handler_get_context(&handler), nullptr);
    lh_runtime_terminate_handler_set_context(&handler, &context);
    EXPECT_EQ(lh_runtime_terminate_handler_get_context(&handler), &context);
}

TEST(runtime_terminate_handler_init_and_assign, copy_both_fields)
{
    int context = 0;
    lh_runtime_terminate_handler_t a;
    lh_runtime_terminate_handler_t b;
    lh_runtime_terminate_handler_init(&a, count_call, &context);
    lh_runtime_terminate_handler_assign(&b, &a);
    EXPECT_EQ(lh_runtime_terminate_handler_get_call_cb(&b), count_call);
    EXPECT_EQ(lh_runtime_terminate_handler_get_context(&b), &context);
}

TEST(runtime_terminate_handler_call, hands_the_context_to_the_call)
{
    int context = 0;
    g_calls = 0;
    const lh_runtime_terminate_handler_t handler =
        lh_runtime_terminate_handler_initializer(count_call, &context);
    lh_runtime_terminate_handler_call(&handler);
    EXPECT_EQ(g_calls, 1);
    EXPECT_EQ(g_seen, &context);
}

TEST(runtime_terminate_handler_call, does_nothing_without_a_call)
{
    g_calls = 0;
    const lh_runtime_terminate_handler_t handler =
        lh_runtime_terminate_handler_initializer(lh_null, lh_null);
    lh_runtime_terminate_handler_call(&handler);
    EXPECT_EQ(g_calls, 0);
}

TEST(runtime_terminate_handler_death, rejects_null_handler)
{
    LH_EXPECT_DEATH(lh_runtime_terminate_handler_get_call_cb(lh_null));
    LH_EXPECT_DEATH(lh_runtime_terminate_handler_set_context(lh_null, lh_null));
    LH_EXPECT_DEATH(lh_runtime_terminate_handler_call(lh_null));
}

} // namespace
