#include <gtest/gtest.h>

#include <lh/math.h>
#include <lh/math/scalar/centre.h>

namespace
{

TEST(math_scalar_slack, non_negative_leftover)
{
    EXPECT_EQ(lh_math_scalar_slack(10, 4), 6);
    EXPECT_EQ(lh_math_scalar_slack(4, 10), 0);
    EXPECT_EQ(lh_math_scalar_slack(5, 5), 0);
}

TEST(math_scalar_centre, half_up_far_side)
{
    /* room 16, own 15 → slack 1 → (1+1)/2 = 1 */
    EXPECT_EQ(lh_math_scalar_centre(16, 15), 1);
    /* room 16, own 9 → slack 7 → 4 */
    EXPECT_EQ(lh_math_scalar_centre(16, 9), 4);
    /* even slack 6 → 3 */
    EXPECT_EQ(lh_math_scalar_centre(10, 4), 3);
    /* own larger than room → 0 */
    EXPECT_EQ(lh_math_scalar_centre(4, 10), 0);
    /* centre(left, 0) is justify-center lead */
    EXPECT_EQ(lh_math_scalar_centre(5, 0), 3);
    EXPECT_EQ(lh_math_scalar_centre(4, 0), 2);
}

TEST(math_div_round, half_up_non_negative)
{
    EXPECT_EQ(lh_math_div_round(0U, 3U), 0U);
    EXPECT_EQ(lh_math_div_round(1U, 3U), 0U);
    EXPECT_EQ(lh_math_div_round(2U, 3U), 1U);
    EXPECT_EQ(lh_math_div_round(10, 4), 3);
    EXPECT_EQ(lh_math_div_round(255 * 128, 256), 128);
}

} // namespace
