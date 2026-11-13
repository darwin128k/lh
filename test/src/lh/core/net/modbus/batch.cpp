#include <gtest/gtest.h>

#include <lh/core/net/modbus.h>
#include <lh/core/net/modbus/batch.h>

#include <map>
#include <set>
#include <vector>

/* A device that serves the jobs of a batch, and what a device does wrong is a field. */
namespace
{
struct request
{
    lh_u8_t fc;
    lh_u16_t address;
    lh_u16_t count;
};

struct device
{
    std::map<lh_u16_t, lh_u16_t> registers;
    std::map<lh_u16_t, lh_u8_t> refuse_at; /**< a write that covers it is refused with this code */
    std::set<lh_u16_t> lose_write_at;      /**< a write that covers it goes unanswered */
    std::set<lh_u16_t>
        ignore_first_write_at; /**< the first write that covers it is answered and not applied */
    lh_u16_t bias = 0;         /**< added to every word written */
    bool lose_reads = false;   /**< no read is answered */
    std::vector<request> asked;

    lh_u16_t
    at(lh_u16_t address) const
    {
        const auto found = registers.find(address);

        return found == registers.end() ? 0 : found->second;
    }

    int
    count(lh_u8_t fc) const
    {
        int n = 0;

        for (const request &r : asked)
        {
            n += r.fc == fc ? 1 : 0;
        }
        return n;
    }

    int
    writes() const
    {
        return count(LH_MB_FC_WRITE_ONE) + count(LH_MB_FC_WRITE_MANY);
    }

    /* The job as the device would answer it, put into the batch. */
    void
    serve(lh_mb_batch_t *batch, const lh_mb_job_t *job)
    {
        const lh_u16_t address = lh_mb_job_get_address(job);
        const lh_u16_t count = lh_mb_job_get_count(job);

        asked.push_back({lh_mb_job_get_fc(job), address, count});
        if (!lh_mb_job_is_write(job))
        {
            std::vector<lh_u16_t> words;

            for (lh_u16_t i = 0; i < count; ++i)
            {
                words.push_back(at(address + i));
            }
            lh_mb_batch_settle(batch, lose_reads ? lh_mb_status_short : lh_mb_status_ok, 0,
                               words.data(), count);
            return;
        }
        for (lh_u16_t i = 0; i < count; ++i)
        {
            if (lose_write_at.count(address + i) != 0)
            {
                lh_mb_batch_settle(batch, lh_mb_status_short, 0, nullptr, 0);
                return;
            }
            const auto refused = refuse_at.find(address + i);

            if (refused != refuse_at.end())
            {
                lh_mb_batch_settle(batch, lh_mb_status_exception, refused->second, nullptr, 0);
                return;
            }
        }
        for (lh_u16_t i = 0; i < count; ++i)
        {
            if (ignore_first_write_at.erase(address + i) == 0)
            {
                registers[address + i] =
                    lh_cast_static(lh_u16_t, lh_mb_job_get_values(job)[i] + bias);
            }
        }
        lh_mb_batch_settle(batch, lh_mb_status_ok, 0, nullptr, 0);
    }

    /* Every step of the batch, until none is left. */
    void
    run(lh_mb_batch_t *batch)
    {
        lh_mb_job_t job;

        for (int guard = 0; guard < 1000 && lh_mb_batch_try_next(batch, &job); ++guard)
        {
            serve(batch, &job);
        }
    }
};

lh_mb_batch_item_t
item(const lh_mb_batch_t *batch, lh_u16_t index)
{
    return static_cast<lh_mb_batch_item_t>(lh_mb_batch_get_items(batch)[index]);
}

bool
begin(lh_mb_batch_t *batch, std::vector<lh_u16_t> addresses, std::vector<lh_u16_t> values)
{
    return lh_mb_batch_try_begin(batch, 1, addresses.data(), values.data(),
                                 static_cast<lh_u16_t>(addresses.size()));
}

} // namespace

/* The batch, the device and a job to make, which a test needs and does not want to declare. */
class lh_mb_batch_test : public ::testing::Test
{
  protected:
    lh_mb_batch_t batch;
    lh_mb_job_t job;
    device dev;
};

/* -- Taking a batch ----------------------------------------------------------- */

TEST_F(lh_mb_batch_test, a_batch_takes_ascending_addresses_and_one_batch_at_a_time)
{
    lh_mb_batch_init(&batch, 2, 0);
    EXPECT_TRUE(lh_mb_batch_is_idle(&batch));
    EXPECT_FALSE(begin(&batch, {10, 10}, {1, 2})) << "the same register twice";
    EXPECT_FALSE(begin(&batch, {11, 10}, {1, 2})) << "descending";
    EXPECT_TRUE(lh_mb_batch_is_idle(&batch)) << "a refused batch is not taken";
    lh_mb_batch_try_begin(&batch, 1, reinterpret_cast<const lh_u16_t *>(&batch),
                          reinterpret_cast<const lh_u16_t *>(&batch), 0);
    EXPECT_TRUE(lh_mb_batch_is_idle(&batch)) << "no values at all";
    std::vector<lh_u16_t> many(LH_MB_BATCH_MAX + 1);

    for (std::size_t i = 0; i < many.size(); ++i)
    {
        many[i] = static_cast<lh_u16_t>(i);
    }
    EXPECT_FALSE(lh_mb_batch_try_begin(&batch, 1, many.data(), many.data(),
                                       static_cast<lh_u16_t>(many.size())))
        << "more than a batch holds";
    EXPECT_TRUE(begin(&batch, {10, 11}, {1, 2}));
    EXPECT_FALSE(lh_mb_batch_is_idle(&batch));
    EXPECT_FALSE(begin(&batch, {20}, {3})) << "one batch at a time";
    lh_mb_batch_release(&batch);
    EXPECT_TRUE(begin(&batch, {20}, {3})) << "free again once released";
}

/* -- The writes --------------------------------------------------------------- */

TEST_F(lh_mb_batch_test, consecutive_values_go_in_one_write_and_a_lone_value_in_a_write_one)
{
    lh_mb_batch_init(&batch, 2, 0);
    ASSERT_TRUE(begin(&batch, {10, 11, 12, 20, 30, 31}, {1, 2, 3, 4, 5, 6}));
    dev.run(&batch);
    ASSERT_EQ(dev.asked.size(), 6U) << "three writes and three read-backs";
    EXPECT_EQ(dev.asked[0].fc, LH_MB_FC_WRITE_MANY);
    EXPECT_EQ(dev.asked[0].address, 10);
    EXPECT_EQ(dev.asked[0].count, 3);
    EXPECT_EQ(dev.asked[1].fc, LH_MB_FC_WRITE_ONE);
    EXPECT_EQ(dev.asked[1].address, 20);
    EXPECT_EQ(dev.asked[2].fc, LH_MB_FC_WRITE_MANY);
    EXPECT_EQ(dev.asked[2].address, 30);
    EXPECT_EQ(dev.asked[2].count, 2);
    EXPECT_EQ(dev.at(12), 3);
    EXPECT_EQ(dev.at(31), 6);
    EXPECT_TRUE(lh_mb_batch_is_finished(&batch));
    EXPECT_EQ(lh_mb_batch_count_items(&batch, lh_mb_batch_item_confirmed), 6);
}

TEST_F(lh_mb_batch_test, the_step_says_which_values_it_covers)
{
    lh_mb_batch_init(&batch, 2, 0);
    ASSERT_TRUE(begin(&batch, {10, 11, 20, 30}, {1, 2, 3, 4}));
    ASSERT_TRUE(lh_mb_batch_try_next(&batch, &job));
    EXPECT_EQ(lh_mb_batch_get_first(&batch), 0);
    EXPECT_EQ(lh_mb_batch_get_span(&batch), 2);
    ASSERT_TRUE(lh_mb_batch_try_next(&batch, &job));
    EXPECT_EQ(lh_mb_batch_get_first(&batch), 2);
    EXPECT_EQ(lh_mb_batch_get_span(&batch), 1);
}

TEST_F(lh_mb_batch_test, registers_between_two_values_are_never_written)
{
    dev.registers[15] = 777;
    lh_mb_batch_init(&batch, 2, 10);
    ASSERT_TRUE(begin(&batch, {10, 20}, {1, 2}));
    dev.run(&batch);
    EXPECT_EQ(dev.at(15), 777);
    EXPECT_EQ(dev.writes(), 2) << "one write for each, a gap is crossed by a read only";
}

/* -- The read-backs ----------------------------------------------------------- */

TEST_F(lh_mb_batch_test, a_read_back_crosses_a_gap_when_it_may)
{
    lh_mb_batch_init(&batch, 2, 10);
    ASSERT_TRUE(begin(&batch, {10, 11, 12, 20, 30, 31}, {1, 2, 3, 4, 5, 6}));
    dev.run(&batch);
    ASSERT_EQ(dev.asked.size(), 4U) << "three writes and one read-back";
    EXPECT_EQ(dev.asked[3].fc, LH_MB_FC_READ_HOLDING);
    EXPECT_EQ(dev.asked[3].address, 10);
    EXPECT_EQ(dev.asked[3].count, 22);
    EXPECT_EQ(lh_mb_batch_count_items(&batch, lh_mb_batch_item_confirmed), 6);
}

TEST_F(lh_mb_batch_test, a_read_back_does_not_cross_a_gap_wider_than_it_may)
{
    lh_mb_batch_init(&batch, 2, 7);
    ASSERT_TRUE(begin(&batch, {10, 18, 30}, {1, 2, 3}));
    dev.run(&batch);
    /* 10 and 18 are seven registers apart: one read; 30 is eleven past 18: another. */
    ASSERT_EQ(dev.count(LH_MB_FC_READ_HOLDING), 2);
    EXPECT_EQ(dev.asked[3].address, 10);
    EXPECT_EQ(dev.asked[3].count, 9);
    EXPECT_EQ(dev.asked[4].address, 30);
}

TEST_F(lh_mb_batch_test, a_read_back_is_no_wider_than_a_read_may_be)
{
    lh_mb_batch_init(&batch, 2, 1000);
    ASSERT_TRUE(begin(&batch, {0, 100, 200}, {1, 2, 3}));
    dev.run(&batch);
    ASSERT_EQ(dev.count(LH_MB_FC_READ_HOLDING), 2);
    EXPECT_EQ(dev.asked[3].address, 0);
    EXPECT_EQ(dev.asked[3].count, 101);
    EXPECT_EQ(dev.asked[4].address, 200);
    EXPECT_EQ(lh_mb_batch_count_items(&batch, lh_mb_batch_item_confirmed), 3);
}

/* -- What happened to each ---------------------------------------------------- */

TEST_F(lh_mb_batch_test, a_group_of_three_with_the_second_refused_says_so_for_that_one_only)
{
    dev.registers[200] = 55;
    dev.refuse_at[200] = 2;
    lh_mb_batch_init(&batch, 2, 0);
    ASSERT_TRUE(begin(&batch, {100, 200, 300}, {1, 2, 3}));
    dev.run(&batch);
    EXPECT_EQ(item(&batch, 0), lh_mb_batch_item_confirmed);
    EXPECT_EQ(item(&batch, 1), lh_mb_batch_item_refused);
    EXPECT_EQ(item(&batch, 2), lh_mb_batch_item_confirmed);
    EXPECT_EQ(lh_mb_batch_get_exceptions(&batch)[1], 2);
    EXPECT_EQ(dev.at(200), 55) << "the refused value did not land";
    EXPECT_EQ(dev.at(100), 1);
    EXPECT_EQ(dev.at(300), 3);
    EXPECT_EQ(dev.count(LH_MB_FC_READ_HOLDING), 2) << "a refused value is not read back";
    EXPECT_EQ(dev.writes(), 3) << "and not written again";
    EXPECT_TRUE(lh_mb_batch_is_finished(&batch));
}

TEST_F(lh_mb_batch_test, a_write_that_goes_unanswered_is_failed_and_not_read_back)
{
    dev.lose_write_at.insert(200);
    lh_mb_batch_init(&batch, 2, 0);
    ASSERT_TRUE(begin(&batch, {100, 200}, {1, 2}));
    dev.run(&batch);
    EXPECT_EQ(item(&batch, 0), lh_mb_batch_item_confirmed);
    EXPECT_EQ(item(&batch, 1), lh_mb_batch_item_failed);
    EXPECT_EQ(dev.count(LH_MB_FC_READ_HOLDING), 1);
    EXPECT_EQ(dev.writes(), 2) << "a silence is the line's to retry, not the batch's";
}

TEST_F(lh_mb_batch_test, a_read_back_that_goes_unanswered_leaves_the_values_unverified)
{
    dev.lose_reads = true;
    lh_mb_batch_init(&batch, 2, 0);
    ASSERT_TRUE(begin(&batch, {100, 200}, {1, 2}));
    dev.run(&batch);
    EXPECT_EQ(item(&batch, 0), lh_mb_batch_item_unverified);
    EXPECT_EQ(item(&batch, 1), lh_mb_batch_item_unverified);
    EXPECT_EQ(dev.writes(), 2) << "written, and not written again for want of a verdict";
    EXPECT_TRUE(lh_mb_batch_is_finished(&batch));
}

TEST_F(lh_mb_batch_test, a_read_back_that_stops_short_leaves_the_values_past_it_unverified)
{
    const lh_u16_t words[1] = {1};

    lh_mb_batch_init(&batch, 2, 10);
    ASSERT_TRUE(begin(&batch, {10, 12}, {1, 2}));
    ASSERT_TRUE(lh_mb_batch_try_next(&batch, &job));
    lh_mb_batch_settle(&batch, lh_mb_status_ok, 0, nullptr, 0);
    ASSERT_TRUE(lh_mb_batch_try_next(&batch, &job));
    lh_mb_batch_settle(&batch, lh_mb_status_ok, 0, nullptr, 0);
    ASSERT_TRUE(lh_mb_batch_try_next(&batch, &job));
    EXPECT_EQ(lh_mb_job_get_count(&job), 3);
    lh_mb_batch_settle(&batch, lh_mb_status_ok, 0, words, 1);
    EXPECT_EQ(item(&batch, 0), lh_mb_batch_item_confirmed);
    EXPECT_EQ(item(&batch, 1), lh_mb_batch_item_unverified);
}

/* -- Another try -------------------------------------------------------------- */

TEST_F(lh_mb_batch_test, a_value_that_reads_back_as_another_word_is_written_once_more_alone)
{
    dev.ignore_first_write_at.insert(200);
    lh_mb_batch_init(&batch, 2, 0);
    ASSERT_TRUE(begin(&batch, {100, 200}, {1, 2}));
    dev.run(&batch);
    EXPECT_EQ(item(&batch, 0), lh_mb_batch_item_confirmed);
    EXPECT_EQ(item(&batch, 1), lh_mb_batch_item_confirmed);
    EXPECT_EQ(lh_mb_batch_get_tries(&batch), 2);
    ASSERT_EQ(dev.writes(), 3) << "the two, and the one that did not take";
    EXPECT_EQ(dev.asked[4].fc, LH_MB_FC_WRITE_ONE);
    EXPECT_EQ(dev.asked[4].address, 200);
    ASSERT_EQ(dev.count(LH_MB_FC_READ_HOLDING), 3);
    EXPECT_EQ(dev.asked[5].address, 200) << "and only that one read again";
    EXPECT_EQ(dev.asked[5].count, 1);
}

TEST_F(lh_mb_batch_test, a_value_that_never_holds_is_a_mismatch_with_the_word_the_device_holds)
{
    dev.bias = 1;
    lh_mb_batch_init(&batch, 2, 0);
    ASSERT_TRUE(begin(&batch, {100}, {7}));
    dev.run(&batch);
    EXPECT_EQ(item(&batch, 0), lh_mb_batch_item_mismatch);
    EXPECT_EQ(lh_mb_batch_get_held(&batch)[0], 8);
    EXPECT_EQ(dev.writes(), 2) << "as many writes as the batch may make";
}

TEST_F(lh_mb_batch_test, one_try_writes_every_value_once_and_a_mismatch_stays_one)
{
    dev.bias = 1;
    lh_mb_batch_init(&batch, 1, 0);
    ASSERT_TRUE(begin(&batch, {100, 200}, {7, 8}));
    dev.run(&batch);
    EXPECT_EQ(lh_mb_batch_count_items(&batch, lh_mb_batch_item_mismatch), 2);
    EXPECT_EQ(dev.writes(), 2);
}

/* -- Finished ----------------------------------------------------------------- */

TEST_F(lh_mb_batch_test, a_finished_batch_makes_no_step_and_is_free_after_release)
{
    lh_mb_batch_init(&batch, 2, 0);
    EXPECT_FALSE(lh_mb_batch_try_next(&batch, &job)) << "nothing taken";
    ASSERT_TRUE(begin(&batch, {100}, {1}));
    EXPECT_FALSE(lh_mb_batch_is_finished(&batch));
    dev.run(&batch);
    EXPECT_TRUE(lh_mb_batch_is_finished(&batch));
    EXPECT_FALSE(lh_mb_batch_try_next(&batch, &job));
    EXPECT_FALSE(lh_mb_batch_is_idle(&batch)) << "the outcome is still there to be read";
    lh_mb_batch_release(&batch);
    EXPECT_TRUE(lh_mb_batch_is_idle(&batch));
}