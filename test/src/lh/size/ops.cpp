#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/size/limits.h>
#include <lh/size/ops.h>

namespace
{

TEST(size_add, sums_up_to_the_maximum)
{
    EXPECT_EQ(lh_size_add(2, 3), 5u);
    EXPECT_EQ(lh_size_add(LH_USIZE_T_MAX - 1, 1), LH_USIZE_T_MAX);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(size_add, rejects_overflow_death)
{
    LH_EXPECT_DEATH((void)lh_size_add(LH_USIZE_T_MAX, 1));
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
