#include <gtest/gtest.h>

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/expect/death.h>
#include <lh/runtime/assert.h>
#include <lh/runtime/check.h>
#include <lh/runtime/error.h>

#include <cstdio>

namespace
{

LH_COMPILER_EXTERN_C_BEGIN

void
test_check_handler_noop(const lh_runtime_check_site_t *site)
{
    (void)site;
}

void
test_check_handler_prints_line(const lh_runtime_check_site_t *site)
{
    std::fprintf(stderr, "custom handler saw line %u\n", site != nullptr ? site->line : 0u);
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
    lh_runtime_check_if(1 == 2,
                        lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    lh_runtime_check_ifn(1 == 1,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    SUCCEED();
}

#if LH_TEST_EXPECT_DEATH_ENABLED

/* The failing statements live in functions: macro arguments cannot carry
   the commas of a check call directly into EXPECT_DEATH. */
void
fail_two_plus_two(void)
{
    lh_runtime_check_if(2 + 2 == 4,
                        lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
}

void
fail_with_custom_handler(void)
{
    lh_runtime_check_set(test_check_handler_prints_line);
    lh_runtime_check_if(1 == 1,
                        lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
}

#    if (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT == LH_RUNTIME_CHECK_REPORT_FULL) &&                \
        (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_ON)
TEST(runtime_check_death, default_handler_reports_condition_and_file)
{
    EXPECT_DEATH(fail_two_plus_two(), "runtime check failed: 2 \\+ 2 == 4");
    EXPECT_DEATH(fail_two_plus_two(), "check\\.cpp:\\d+ in fail_two_plus_two");
}
#    endif

TEST(runtime_check_death, custom_handler_gets_the_site_then_terminates)
{
#    if (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT == LH_RUNTIME_CHECK_REPORT_NONE)
    EXPECT_DEATH(fail_with_custom_handler(), "custom handler saw line 0");
#    else
    EXPECT_DEATH(fail_with_custom_handler(), "custom handler saw line \\d\\d");
#    endif
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
