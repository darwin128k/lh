#include <gtest/gtest.h>

#include <lh/core/net/modbus/job/queue.h>

/* -- The queue ---------------------------------------------------------------- */

namespace
{

lh_mb_job_t
job_of(lh_u32_t owner, lh_u32_t ticket)
{
    lh_mb_job_t job;

    lh_mb_job_init_read(&job, 1, LH_MB_FC_READ_HOLDING, 0, 1);
    lh_mb_job_set_owner(&job, owner);
    lh_mb_job_set_ticket(&job, ticket);
    return job;
}

} // namespace

TEST(lh_mb_job_queue, first_in_first_out_round_the_ring)
{
    lh_mb_job_t storage[3];
    lh_mb_job_queue_t queue;
    lh_mb_job_t out;

    lh_mb_job_queue_init(&queue, storage, 3);
    for (lh_u32_t round = 0; round < 5; ++round)
    {
        for (lh_u32_t i = 0; i < 3; ++i)
        {
            const lh_mb_job_t job = job_of(1, round * 10 + i);
            ASSERT_TRUE(lh_mb_job_queue_try_push(&queue, &job));
        }
        const lh_mb_job_t extra = job_of(1, 99);
        EXPECT_FALSE(lh_mb_job_queue_try_push(&queue, &extra)) << "full says so";
        for (lh_u32_t i = 0; i < 3; ++i)
        {
            ASSERT_TRUE(lh_mb_job_queue_try_pop(&queue, &out));
            EXPECT_EQ(lh_mb_job_get_ticket(&out), round * 10 + i);
        }
        EXPECT_FALSE(lh_mb_job_queue_try_pop(&queue, &out));
    }
}

TEST(lh_mb_job_queue, a_client_that_goes_takes_its_jobs_and_nobody_elses)
{
    lh_mb_job_t storage[8];
    lh_mb_job_queue_t queue;
    lh_mb_job_t out;

    lh_mb_job_queue_init(&queue, storage, 8);
    /* Start past the middle so the jobs wrap round the end of the storage. */
    for (lh_u32_t i = 0; i < 6; ++i)
    {
        const lh_mb_job_t job = job_of(0, 0);
        lh_mb_job_queue_try_push(&queue, &job);
        lh_mb_job_queue_try_pop(&queue, &out);
    }
    const lh_u32_t owners[7] = {1, 2, 1, 3, 2, 1, 3};
    for (lh_u32_t i = 0; i < 7; ++i)
    {
        const lh_mb_job_t job = job_of(owners[i], i);
        ASSERT_TRUE(lh_mb_job_queue_try_push(&queue, &job));
    }
    EXPECT_EQ(lh_mb_job_queue_discard_owner(&queue, 1), 3U);
    EXPECT_EQ(lh_mb_job_queue_discard_owner(&queue, 9), 0U);
    const lh_u32_t left[4] = {1, 3, 4, 6};
    for (lh_u32_t ticket : left)
    {
        ASSERT_TRUE(lh_mb_job_queue_try_pop(&queue, &out));
        EXPECT_EQ(lh_mb_job_get_ticket(&out), ticket) << "the others keep their order";
    }
    EXPECT_TRUE(lh_mb_job_queue_is_empty(&queue));
}
