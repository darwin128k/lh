#include <gtest/gtest.h>

#include <lh/core/net/modbus/health.h>

/* -- Health: gone only after failures in a row --------------------------------- */

TEST(lh_mb_health, a_stall_is_not_a_lost_device)
{
    lh_mb_health_t h;

    lh_mb_health_init(&h, LH_MB_HEALTH_LIMIT);
    /* A device busy for a second: one or two requests lost, then answers again. */
    lh_mb_health_count_failure(&h);
    lh_mb_health_count_failure(&h);
    EXPECT_FALSE(lh_mb_health_is_lost(&h));
    lh_mb_health_count_answer(&h);
    EXPECT_EQ(lh_mb_health_get_failures(&h), 0);
    for (int i = 0; i < 9; ++i)
    {
        lh_mb_health_count_failure(&h);
    }
    EXPECT_FALSE(lh_mb_health_is_lost(&h)) << "nine in a row";
    lh_mb_health_count_failure(&h);
    EXPECT_TRUE(lh_mb_health_is_lost(&h)) << "the tenth";
    lh_mb_health_count_failure(&h);
    EXPECT_EQ(lh_mb_health_get_failures(&h), LH_MB_HEALTH_LIMIT) << "counted up to the limit only";
    lh_mb_health_count_answer(&h);
    EXPECT_FALSE(lh_mb_health_is_lost(&h)) << "one answer and it is back";
}

TEST(lh_mb_health, a_limit_of_zero_is_one)
{
    lh_mb_health_t h;

    lh_mb_health_init(&h, 0);
    EXPECT_EQ(lh_mb_health_get_limit(&h), 1);
    lh_mb_health_count_failure(&h);
    EXPECT_TRUE(lh_mb_health_is_lost(&h));
}
