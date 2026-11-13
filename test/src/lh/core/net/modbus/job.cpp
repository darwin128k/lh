#include <gtest/gtest.h>

#include <lh/core/net/modbus/job.h>

/* -- A job: one request written down ------------------------------------------ */

TEST(lh_mb_job, a_read_is_owned_by_nobody_and_writes_nothing)
{
    lh_mb_job_t job;

    lh_mb_job_init_read(&job, 3, LH_MB_FC_READ_INPUT, 65000, 13);
    EXPECT_EQ(lh_mb_job_get_unit(&job), 3);
    EXPECT_EQ(lh_mb_job_get_fc(&job), LH_MB_FC_READ_INPUT);
    EXPECT_EQ(lh_mb_job_get_address(&job), 65000);
    EXPECT_EQ(lh_mb_job_get_count(&job), 13);
    EXPECT_EQ(lh_mb_job_get_owner(&job), 0U);
    EXPECT_EQ(lh_mb_job_get_ticket(&job), 0U);
    EXPECT_FALSE(lh_mb_job_is_write(&job));
}

TEST(lh_mb_job, a_write_keeps_a_copy_of_its_words)
{
    lh_u16_t words[2] = {0xA120, 0x0007};
    lh_mb_job_t job;

    lh_mb_job_init_write(&job, 1, LH_MB_FC_WRITE_MANY, 32772, words, 2);
    words[0] = 0;
    EXPECT_TRUE(lh_mb_job_is_write(&job));
    EXPECT_EQ(lh_mb_job_get_values(&job)[0], 0xA120) << "the caller may reuse its buffer";
    EXPECT_EQ(lh_mb_job_get_values(&job)[1], 0x0007);
}

TEST(lh_mb_job, a_write_longer_than_the_protocol_allows_is_cut)
{
    lh_u16_t words[200] = {};
    lh_mb_job_t job;

    lh_mb_job_init_write(&job, 1, LH_MB_FC_WRITE_MANY, 0, words, 200);
    EXPECT_EQ(lh_mb_job_get_count(&job), LH_MB_WRITE_MAX);
}
