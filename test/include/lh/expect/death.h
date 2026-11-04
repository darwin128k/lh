/**
 * @file death.h
 * @brief GoogleTest death-test helper for lh APIs that fail via lh_runtime_check_ref.
 *
 * Include from test sources that assert on null pointers and similar runtime checks.
 * LH_EXPECT_DEATH / LH_EXPECT_CHECK_DEATH are always defined: where death tests cannot
 * run they compile the statement without running it. LH_TEST_EXPECT_DEATH_ENABLED
 * still says whether they run, for guarding whole `TEST` blocks.
 *
 * Death tests rely on lh_runtime_terminate() ending the process with abort().
 * The core starts with no terminate handler; the common runner
 * (test/src/main.cpp) installs one that calls abort() before any test runs.
 *
 * This does not prove anything at runtime: a test can replace abort() on its
 * thread through lh_runtime_terminate_get_mut(); then EXPECT_DEATH may fail unless
 * tests restore abort() or otherwise ensure the process still dies in a way
 * the death-test harness expects.
 */

#ifndef LH_TEST_EXPECT_DEATH_H
#define LH_TEST_EXPECT_DEATH_H

#include <gtest/gtest.h>
#include <lh/config.h>

#include <lh/runtime/assert.h>

/**
 * LH_TEST_EXPECT_DEATH_SKIP(stmt): compile @p stmt behind `if (false)` and never run
 * it. The test body keeps being checked, and the locals it uses do not turn into
 * "unused variable" warnings (MSVC C4101).
 */
#define LH_TEST_EXPECT_DEATH_SKIP(stmt)                                                            \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
        {                                                                                          \
            (void)(stmt);                                                                          \
        }                                                                                          \
    } while (0)

#if GTEST_HAS_DEATH_TEST

/**
 * A failed check calls lh_runtime_terminate(), which is abort() (the stdlib default
 * or the runner's); the child process exits and EXPECT_DEATH matches stderr with ".*".
 */

/**
 * LH_EXPECT_DEATH(stmt): @p stmt breaks a contract (lh_runtime_assert_* /
 * lh_assert_runtime_*). Those checks are compiled out without
 * LH_RUNTIME_ASSERT_ENABLED (Release), and running a contract violation with
 * no check is undefined behaviour — so there @p stmt is not run at all
 * (LH_TEST_EXPECT_DEATH_SKIP).
 */
#    if LH_RUNTIME_ASSERT_ENABLED
#        define LH_EXPECT_DEATH(stmt) EXPECT_DEATH((stmt), ".*")
#    else
#        define LH_EXPECT_DEATH(stmt) LH_TEST_EXPECT_DEATH_SKIP(stmt)
#    endif

/**
 * LH_EXPECT_CHECK_DEATH(stmt): @p stmt trips an always-on lh_runtime_check_*
 * (environment failure, e.g. allocation) — it dies in every build.
 */
#    define LH_EXPECT_CHECK_DEATH(stmt) EXPECT_DEATH((stmt), ".*")

#    define LH_TEST_EXPECT_DEATH_ENABLED 1

#else

/* No death tests here (no GTEST_HAS_DEATH_TEST): both macros compile their
   statement and never run it. */
#    define LH_EXPECT_DEATH(stmt) LH_TEST_EXPECT_DEATH_SKIP(stmt)
#    define LH_EXPECT_CHECK_DEATH(stmt) LH_TEST_EXPECT_DEATH_SKIP(stmt)

#    define LH_TEST_EXPECT_DEATH_ENABLED 0

#endif

#endif /* LH_TEST_EXPECT_DEATH_H */
