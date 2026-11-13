#include <gtest/gtest.h>

#include <lh/core/lanes.h>

/* -- Lanes: priority that starves nobody ------------------------------------
   Three lanes: 0 urgent requests, 1 interactive ones, 2 background work. Ready
   masks are written as bits: 0b101 is lanes 0 and 2. */

TEST(lh_lanes, with_no_patience_the_lowest_ready_lane_always_goes)
{
    lh_lanes_t lanes;

    lh_lanes_init(&lanes, 3);
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(lh_lanes_pick(&lanes, 0b110), 1) << "plain priority: 2 waits for ever";
    }
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b100), 2) << "until 1 is idle";
    EXPECT_EQ(lh_lanes_pick(&lanes, 0), 3) << "nothing ready: the count";
}

TEST(lh_lanes, patience_one_makes_two_busy_lanes_alternate)
{
    lh_lanes_t lanes;

    lh_lanes_init(&lanes, 3);
    lh_lanes_set_patience(&lanes, 2, 1);
    int got[6];
    for (int &g : got)
    {
        g = lh_lanes_pick(&lanes, 0b110);
    }
    const int expected[6] = {1, 2, 1, 2, 1, 2};
    for (int i = 0; i < 6; ++i)
    {
        EXPECT_EQ(got[i], expected[i]) << "turn " << i;
    }
}

TEST(lh_lanes, patience_three_gives_a_lane_every_fourth_turn)
{
    lh_lanes_t lanes;

    lh_lanes_init(&lanes, 3);
    lh_lanes_set_patience(&lanes, 2, 3);
    int background = 0;
    for (int i = 0; i < 40; ++i)
    {
        background += lh_lanes_pick(&lanes, 0b111) == 2 ? 1 : 0;
    }
    EXPECT_EQ(background, 10);
}

TEST(lh_lanes, a_lane_that_was_idle_has_not_been_waiting)
{
    lh_lanes_t lanes;

    lh_lanes_init(&lanes, 3);
    lh_lanes_set_patience(&lanes, 2, 2);
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b011), 0);
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b011), 0);
    EXPECT_EQ(lh_lanes_get_waited(&lanes, 2), 0) << "turns it was not ready for do not count";
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b101), 0);
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b101), 0);
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b101), 2) << "two turns let go by: its patience";
}

TEST(lh_lanes, the_highest_impatient_lane_goes_first)
{
    lh_lanes_t lanes;

    lh_lanes_init(&lanes, 3);
    lh_lanes_set_patience(&lanes, 1, 1);
    lh_lanes_set_patience(&lanes, 2, 1);
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b111), 0);
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b111), 1) << "1 and 2 both impatient: 1 is higher";
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b111), 2);
}

TEST(lh_lanes, waiting_saturates_instead_of_wrapping)
{
    lh_lanes_t lanes;

    lh_lanes_init(&lanes, 2);
    for (int i = 0; i < 300; ++i)
    {
        lh_lanes_pick(&lanes, 0b11);
    }
    EXPECT_EQ(lh_lanes_get_waited(&lanes, 1), 255);
    lh_lanes_set_patience(&lanes, 1, 200);
    EXPECT_EQ(lh_lanes_pick(&lanes, 0b11), 1) << "255 waited is past any patience";
}

TEST(lh_lanes, more_lanes_than_the_most_are_the_most)
{
    lh_lanes_t lanes;

    lh_lanes_init(&lanes, 200);
    EXPECT_EQ(lh_lanes_get_count(&lanes), LH_LANES_MAX);
}
