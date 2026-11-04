#include <gtest/gtest.h>

#include <lh/core/exception/initializer.h>
#include <lh/expect/death.h>
#include <lh/core/runtime/check/handler.h>
#include <lh/core/runtime/check/handler/initializer.h>
#include <lh/core/runtime/error/initializer.h>

namespace
{

int g_calls = 0;
lh_ptr g_seen_self = lh_null;
const lh_exception_t *g_seen_exception = lh_null;

void
count_fail(lh_self_ptr self, const lh_exception_t *exception)
{
    ++g_calls;
    g_seen_self = self;
    g_seen_exception = exception;
}

TEST(runtime_check_handler_initializer, holds_the_call_and_context)
{
    int context = 0;
    const lh_runtime_check_handler_t handler =
        lh_runtime_check_handler_initializer(count_fail, &context);
    EXPECT_EQ(lh_runtime_check_handler_get_fail_cb(&handler), count_fail);
    EXPECT_EQ(lh_runtime_check_handler_get_context(&handler), &context);
}

TEST(runtime_check_handler_set, each_field_stands_alone)
{
    int context = 0;
    lh_runtime_check_handler_t handler = lh_runtime_check_handler_initializer(lh_null, lh_null);
    lh_runtime_check_handler_set_fail_cb(&handler, count_fail);
    EXPECT_EQ(lh_runtime_check_handler_get_fail_cb(&handler), count_fail);
    EXPECT_EQ(lh_runtime_check_handler_get_context(&handler), nullptr);
    lh_runtime_check_handler_set_context(&handler, &context);
    EXPECT_EQ(lh_runtime_check_handler_get_context(&handler), &context);
}

TEST(runtime_check_handler_init_and_assign, copy_both_fields)
{
    int context = 0;
    lh_runtime_check_handler_t a;
    lh_runtime_check_handler_t b;
    lh_runtime_check_handler_init(&a, count_fail, &context);
    lh_runtime_check_handler_assign(&b, &a);
    EXPECT_EQ(lh_runtime_check_handler_get_fail_cb(&b), count_fail);
    EXPECT_EQ(lh_runtime_check_handler_get_context(&b), &context);
}

TEST(runtime_check_handler_call, hands_context_and_exception_to_the_call)
{
    int context = 0;
    const lh_exception_t exception = lh_exception_initializer(
        lh_runtime_error_initializer(lh_runtime_error_code_overflow, lh_null), lh_null);
    g_calls = 0;
    const lh_runtime_check_handler_t handler =
        lh_runtime_check_handler_initializer(count_fail, &context);
    lh_runtime_check_handler_call(&handler, &exception);
    EXPECT_EQ(g_calls, 1);
    EXPECT_EQ(g_seen_self, &context);
    EXPECT_EQ(g_seen_exception, &exception);
}

TEST(runtime_check_handler_call, does_nothing_without_a_call)
{
    const lh_exception_t exception = lh_exception_initializer(
        lh_runtime_error_initializer(lh_runtime_error_code_overflow, lh_null), lh_null);
    g_calls = 0;
    const lh_runtime_check_handler_t handler =
        lh_runtime_check_handler_initializer(lh_null, lh_null);
    lh_runtime_check_handler_call(&handler, &exception);
    EXPECT_EQ(g_calls, 0);
}

TEST(runtime_check_handler_death, rejects_null_handler)
{
    LH_EXPECT_DEATH(lh_runtime_check_handler_get_fail_cb(lh_null));
    LH_EXPECT_DEATH(lh_runtime_check_handler_set_context(lh_null, lh_null));
}

} // namespace
