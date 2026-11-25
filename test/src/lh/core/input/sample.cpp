#include <gtest/gtest.h>

#include <lh/core/input/sample.h>

TEST(input_sample, find_returns_the_first_match)
{
    lh_input_sample_t samples[2];
    lh_u8_t index = 9U;

    lh_input_sample_init(lh_addr_of(samples[0]), 4U, lh_input_coord_from_int(1),
                         lh_input_coord_from_int(2));
    lh_input_sample_init(lh_addr_of(samples[1]), 8U, lh_input_coord_from_int(3),
                         lh_input_coord_from_int(4));
    EXPECT_FALSE(lh_input_sample_find(lh_null, 1U, 4U, lh_addr_of(index)));
    EXPECT_EQ(index, 9U);
    ASSERT_TRUE(lh_input_sample_find(samples, 2U, 8U, lh_addr_of(index)));
    EXPECT_EQ(index, 1U);
    EXPECT_EQ(lh_input_sample_get_x(lh_addr_of(samples[1])), lh_input_coord_from_int(3));
}
