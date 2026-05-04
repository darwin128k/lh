/**
 * @file expect_death.h
 * @brief GoogleTest death-test helper for lh APIs that fail via lh_runtime_check_ref.
 *
 * Include from test sources that assert on null pointers and similar runtime checks.
 * Use LH_TEST_EXPECT_DEATH_ENABLED to guard whole `TEST` blocks.
 *
 * LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB is checked because it selects
 * the **compile-time default** for lh_runtime_terminate(): ON → initial handler
 * is abort(), which EXPECT_DEATH can rely on; OFF → initial handler is null,
 * calling terminate on failed checks is undefined — these tests are disabled.
 *
 * This does not prove anything at runtime: lh_runtime_terminate_set() can
 * replace abort() with another function; then EXPECT_DEATH may fail unless
 * tests restore abort() or otherwise ensure the process still dies in a way
 * the death-test harness expects.
 */

#ifndef LH_TEST_EXPECT_DEATH_H
#define LH_TEST_EXPECT_DEATH_H

#include <gtest/gtest.h>
#include <lh/config.h>

#if GTEST_HAS_DEATH_TEST && LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB

/**
 * Failed lh_runtime_check_ref unwinds via lh_runtime_throw; with no catch frame this calls
 * lh_runtime_terminate(). With the default stdlib-backed handler that is abort(); the child
 * process exits and EXPECT_DEATH matches stderr with ".*".
 */
#    define LH_EXPECT_DEATH(stmt) EXPECT_DEATH((stmt), ".*")

#    define LH_TEST_EXPECT_DEATH_ENABLED 1

#else

#    define LH_TEST_EXPECT_DEATH_ENABLED 0

#endif

#endif // LH_TEST_EXPECT_DEATH_H
