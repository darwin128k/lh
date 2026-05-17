#include <gtest/gtest.h>

#include <lh/runtime/assert.h>
#include <lh/runtime/error.h>
#include <lh/runtime/exception/catch/stack.h>
#include <lh/exception/catch.h>
#include <lh/null.h>

namespace
{

TEST(runtime_assert_if, throws_when_condition_true)
{
    lh_exception_catch_t frame;
    volatile bool caught = false;
    volatile lh_runtime_error_code_t caught_code = lh_runtime_error_code_ok;

    if (lh_runtime_exception_catch_stack_capture(&frame) == 0)
    {
        lh_runtime_assert_if(1, lh_runtime_error_make_by_code(lh_runtime_error_code_interrupt));
        lh_runtime_exception_catch_stack_prev();
    }
    else
    {
        caught      = true;
        caught_code = lh_exception_catch_get_code(&frame);
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(caught_code, lh_runtime_error_code_interrupt);
}

TEST(runtime_assert_if, no_throw_when_condition_false)
{
    lh_exception_catch_t frame;
    volatile bool reached = false;
    volatile bool caught  = false;

    if (lh_runtime_exception_catch_stack_capture(&frame) == 0)
    {
        lh_runtime_assert_if(0, lh_runtime_error_make_by_code(lh_runtime_error_code_interrupt));
        reached = true;
        lh_runtime_exception_catch_stack_prev();
    }
    else
    {
        caught = true;
    }

    EXPECT_TRUE(reached);
    EXPECT_FALSE(caught);
}

TEST(runtime_assert_if, preserves_error_desc)
{
    lh_exception_catch_t frame;
    volatile bool caught = false;

    if (lh_runtime_exception_catch_stack_capture(&frame) == 0)
    {
        lh_runtime_assert_if(1, lh_runtime_error_make(lh_runtime_error_code_interrupt, "detail"));
        lh_runtime_exception_catch_stack_prev();
    }
    else
    {
        caught = true;
    }

    EXPECT_TRUE(caught);
    EXPECT_STREQ(lh_exception_catch_get_desc(&frame), "detail");
}

TEST(runtime_assert_ifn, throws_when_condition_false)
{
    lh_exception_catch_t frame;
    volatile bool caught = false;
    volatile lh_runtime_error_code_t caught_code = lh_runtime_error_code_ok;

    if (lh_runtime_exception_catch_stack_capture(&frame) == 0)
    {
        lh_runtime_assert_ifn(0,
                              lh_runtime_error_make_by_code(lh_runtime_error_code_null_pointer));
        lh_runtime_exception_catch_stack_prev();
    }
    else
    {
        caught      = true;
        caught_code = lh_exception_catch_get_code(&frame);
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(caught_code, lh_runtime_error_code_null_pointer);
}

TEST(runtime_assert_ifn, no_throw_when_condition_true)
{
    lh_exception_catch_t frame;
    volatile bool reached = false;
    volatile bool caught  = false;

    if (lh_runtime_exception_catch_stack_capture(&frame) == 0)
    {
        lh_runtime_assert_ifn(1,
                              lh_runtime_error_make_by_code(lh_runtime_error_code_null_pointer));
        reached = true;
        lh_runtime_exception_catch_stack_prev();
    }
    else
    {
        caught = true;
    }

    EXPECT_TRUE(reached);
    EXPECT_FALSE(caught);
}

TEST(runtime_assert, throws_when_condition_false)
{
    lh_exception_catch_t frame;
    volatile bool caught = false;
    volatile lh_runtime_error_code_t caught_code = lh_runtime_error_code_ok;

    if (lh_runtime_exception_catch_stack_capture(&frame) == 0)
    {
        lh_runtime_assert(0,
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));
        lh_runtime_exception_catch_stack_prev();
    }
    else
    {
        caught      = true;
        caught_code = lh_exception_catch_get_code(&frame);
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(caught_code, lh_runtime_error_code_out_of_range);
}

TEST(runtime_assert, no_throw_when_condition_true)
{
    lh_exception_catch_t frame;
    volatile bool reached = false;
    volatile bool caught  = false;

    if (lh_runtime_exception_catch_stack_capture(&frame) == 0)
    {
        lh_runtime_assert(1,
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));
        reached = true;
        lh_runtime_exception_catch_stack_prev();
    }
    else
    {
        caught = true;
    }

    EXPECT_TRUE(reached);
    EXPECT_FALSE(caught);
}

TEST(runtime_assert_ref, throws_on_null_pointer)
{
    lh_exception_catch_t frame;
    volatile bool caught = false;
    volatile lh_runtime_error_code_t caught_code = lh_runtime_error_code_ok;

    if (lh_runtime_exception_catch_stack_capture(&frame) == 0)
    {
        lh_runtime_assert_ref(lh_null);
        lh_runtime_exception_catch_stack_prev();
    }
    else
    {
        caught      = true;
        caught_code = lh_exception_catch_get_code(&frame);
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(caught_code, lh_runtime_error_code_null_pointer);
}

TEST(runtime_assert_ref, no_throw_on_non_null_pointer)
{
    lh_exception_catch_t frame;
    volatile bool reached = false;
    volatile bool caught  = false;
    int dummy             = 0;

    if (lh_runtime_exception_catch_stack_capture(&frame) == 0)
    {
        lh_runtime_assert_ref(&dummy);
        reached = true;
        lh_runtime_exception_catch_stack_prev();
    }
    else
    {
        caught = true;
    }

    EXPECT_TRUE(reached);
    EXPECT_FALSE(caught);
}

} // namespace
