#include <gtest/gtest.h>

#include <lh/math/rescale.h>

namespace
{

TEST(math_rescale, ends_map_exactly)
{
    EXPECT_EQ(lh_math_rescale_u32(0U, 15U, 255U), 0U);
    EXPECT_EQ(lh_math_rescale_u32(15U, 15U, 255U), 255U);
    EXPECT_EQ(lh_math_rescale_u32(1U, 1U, 255U), 255U);
}

TEST(math_rescale, rounds_to_nearest)
{
    EXPECT_EQ(lh_math_rescale_u32(8U, 15U, 255U), 136U);
    EXPECT_EQ(lh_math_rescale_u32(1U, 3U, 255U), 85U);
    EXPECT_EQ(lh_math_rescale_u32(128U, 255U, 15U), 8U);
}

} // namespace
