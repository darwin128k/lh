#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/expect/death.h>
#include <lh/runtime/terminate.h>

namespace
{

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
