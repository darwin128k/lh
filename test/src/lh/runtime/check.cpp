#include <gtest/gtest.h>

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/exception.h>
#include <lh/expect/death.h>
#include <lh/runtime/assert.h>
#include <lh/runtime/check.h>
#include <lh/runtime/error/code.h>
#include <lh/str/view.h>

#include <cstdio>

namespace
{

LH_COMPILER_EXTERN_C_BEGIN

void
test_check_handler_noop(const lh_exception_t *exception)
{
    (void)exception;
}

void
test_check_handler_prints(const lh_exception_t *exception)
{
    const lh_exception_origin_t *origin = lh_exception_get_origin(exception);
    lh_str_view_t desc = lh_runtime_error_get_desc(lh_exception_get_error(exception));
    const bool has_desc = !lh_str_view_is_empty(&desc);

    std::fprintf(stderr, "custom handler saw error %d line %u message %.*s\n",
                 lh_exception_get_code(exception), origin != nullptr ? origin->line : 0u,
                 has_desc ? static_cast<int>(lh_str_view_get_size(&desc)) : 4,
                 has_desc ? lh_str_view_get_data(&desc) : "none");
    std::fflush(stderr);
}

LH_COMPILER_EXTERN_C_END

TEST(runtime_check_handler, default_is_installed)
{
    EXPECT_NE(lh_runtime_check_get(), nullptr);
}

TEST(runtime_check_handler, set_replaces_and_null_restores_default)
{
    lh_runtime_check_fail_cb original = lh_runtime_check_get();

    lh_runtime_check_set(test_check_handler_noop);
    EXPECT_EQ(lh_runtime_check_get(), test_check_handler_noop);

    lh_runtime_check_set(nullptr);
    EXPECT_EQ(lh_runtime_check_get(), original);
}

TEST(runtime_check, passing_check_does_nothing)
{
    lh_runtime_check_if(1 == 2, lh_runtime_error_code_invalid_argument);
    lh_runtime_check_ifn(1 == 1, lh_runtime_error_code_invalid_argument, "never shown");
    SUCCEED();
}

#if LH_TEST_EXPECT_DEATH_ENABLED

/* The failing statements live in functions: macro arguments cannot carry
   the commas of a check call directly into EXPECT_DEATH. */
void
fail_two_plus_two(void)
{
    lh_runtime_check_if(2 + 2 == 4, lh_runtime_error_code_invalid_argument, "math still works");
}

void
fail_with_code_only(void)
{
    lh_runtime_check_set(test_check_handler_prints);
    lh_runtime_check_if(1 == 1, lh_runtime_error_code_invalid_range);
}

void
fail_with_message(void)
{
    lh_runtime_check_set(test_check_handler_prints);
    lh_runtime_check_if(1 == 1, lh_runtime_error_code_overflow, "too big");
}

void
fail_with_computed_code(lh_runtime_error_code_t code)
{
    lh_runtime_check_set(test_check_handler_prints);
    lh_runtime_check_if(code != lh_runtime_error_code_ok, code);
}

#    if (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT == LH_RUNTIME_CHECK_REPORT_FULL) &&                \
        (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_ON)
TEST(runtime_check_death, default_handler_reports_error_condition_and_file)
{
    EXPECT_DEATH(fail_two_plus_two(), "runtime check failed: error 9, math still works");
    EXPECT_DEATH(fail_two_plus_two(), "condition: 2 \\+ 2 == 4");
    EXPECT_DEATH(fail_two_plus_two(), "check\\.cpp:\\d+ in fail_two_plus_two");
}
#    endif

/* The code reaches the handler at every report level; the message at all
   but NONE; the line only where the level keeps a origin. */
TEST(runtime_check_death, custom_handler_gets_the_code_then_terminates)
{
#    if (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT == LH_RUNTIME_CHECK_REPORT_NONE)
    EXPECT_DEATH(fail_with_code_only(), "custom handler saw error 6 line 0 message none");
    EXPECT_DEATH(fail_with_message(), "custom handler saw error 13 line 0 message none");
#    elif (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT == LH_RUNTIME_CHECK_REPORT_LOCATION)
    EXPECT_DEATH(fail_with_code_only(), "custom handler saw error 6 line \\d\\d message none");
    EXPECT_DEATH(fail_with_message(), "custom handler saw error 13 line \\d\\d message too big");
#    else
    EXPECT_DEATH(fail_with_code_only(), "custom handler saw error 6 line \\d\\d message none");
    EXPECT_DEATH(fail_with_message(), "custom handler saw error 13 line \\d\\d message too big");
#    endif
}

TEST(runtime_check_death, code_computed_at_run_time_reaches_the_handler)
{
    EXPECT_DEATH(fail_with_computed_code(lh_runtime_error_code_underflow),
                 "custom handler saw error 14 ");
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
