#include <gtest/gtest.h>

#include <lh/core/atomic/uint.h>
#include <lh/core/atomic/uint/initializer.h>
#include <lh/expect/death.h>

#include <atomic>
#include <thread>
#include <vector>

namespace
{

TEST(atomic_uint_initializer, holds_its_value_before_any_store)
{
    lh_atomic_uint_t counter = lh_atomic_uint_initializer(7U);
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), 7U);
    EXPECT_EQ(lh_atomic_uint_get_acquire(&counter), 7U);
}

TEST(atomic_uint_is_lock_free, is_true_on_every_target_that_builds)
{
    EXPECT_TRUE(lh_atomic_uint_is_lock_free());
}

TEST(atomic_uint_set, relaxed_and_release_store_the_value)
{
    lh_atomic_uint_t counter = lh_atomic_uint_initializer(0U);
    lh_atomic_uint_set_relaxed(&counter, 3U);
    EXPECT_EQ(lh_atomic_uint_get_acquire(&counter), 3U);
    lh_atomic_uint_set_release(&counter, 9U);
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), 9U);
}

TEST(atomic_uint_fetch_add, returns_the_value_before_and_wraps)
{
    lh_atomic_uint_t counter = lh_atomic_uint_initializer(5U);
    EXPECT_EQ(lh_atomic_uint_fetch_add_relaxed(&counter, 2U), 5U);
    EXPECT_EQ(lh_atomic_uint_fetch_add_acq_rel(&counter, 3U), 7U);
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), 10U);
    lh_atomic_uint_set_relaxed(&counter, ~0U);
    EXPECT_EQ(lh_atomic_uint_fetch_add_relaxed(&counter, 1U), ~0U);
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), 0U);
}

TEST(atomic_uint_exchange_acq_rel, returns_the_value_before)
{
    lh_atomic_uint_t counter = lh_atomic_uint_initializer(4U);
    EXPECT_EQ(lh_atomic_uint_exchange_acq_rel(&counter, 11U), 4U);
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), 11U);
}

/* Every addition from every thread counts, none is lost. */
TEST(atomic_uint_fetch_add, no_increment_is_lost_across_threads)
{
    constexpr unsigned per_thread = 20000U;
    lh_atomic_uint_t counter = lh_atomic_uint_initializer(0U);
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t)
    {
        threads.emplace_back(
            [&]
            {
                for (unsigned i = 0; i < per_thread; ++i)
                {
                    lh_atomic_uint_fetch_add_relaxed(&counter, 1U);
                }
            });
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    EXPECT_EQ(lh_atomic_uint_get_acquire(&counter), 4U * per_thread);
}

/* The reader sees what the writer did before its release. */
TEST(atomic_uint_get_acquire, sees_what_was_written_before_the_release)
{
    for (int round = 0; round < 1000; ++round)
    {
        int data = 0;
        int seen = -1;
        lh_atomic_uint_t ready = lh_atomic_uint_initializer(0U);
        std::thread reader(
            [&]
            {
                while (lh_atomic_uint_get_acquire(&ready) == 0U)
                {
                    std::this_thread::yield();
                }
                seen = data;
            });
        data = round + 1;
        lh_atomic_uint_set_release(&ready, 1U);
        reader.join();
        ASSERT_EQ(seen, round + 1);
    }
}

TEST(atomic_uint_fetch_sub, returns_the_old_value_and_wraps)
{
    lh_atomic_uint_t counter = lh_atomic_uint_initializer(5U);
    EXPECT_EQ(lh_atomic_uint_fetch_sub_acq_rel(&counter, 2U), 5U);
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), 3U);
    EXPECT_EQ(lh_atomic_uint_fetch_sub_acq_rel(&counter, 4U), 3U);
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), ~0U);
}

TEST(atomic_uint_try_compare_exchange, stores_only_over_the_expected_value)
{
    lh_atomic_uint_t counter = lh_atomic_uint_initializer(7U);
    EXPECT_FALSE(lh_atomic_uint_try_compare_exchange_acq_rel(&counter, 6U, 1U));
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), 7U);
    EXPECT_TRUE(lh_atomic_uint_try_compare_exchange_acq_rel(&counter, 7U, 1U));
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), 1U);
}

/* A bounded counter built from compare-exchange never passes its bound, and
   every increment that succeeded is matched by a decrement. */
TEST(atomic_uint_try_compare_exchange, bounded_increment_across_threads)
{
    constexpr unsigned bound = 3U;
    lh_atomic_uint_t counter = lh_atomic_uint_initializer(0U);
    std::atomic<bool> passed{false};
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t)
    {
        threads.emplace_back(
            [&]
            {
                for (int i = 0; i < 20000; ++i)
                {
                    unsigned seen = lh_atomic_uint_get_acquire(&counter);
                    if (seen >= bound)
                    {
                        continue;
                    }
                    if (!lh_atomic_uint_try_compare_exchange_acq_rel(&counter, seen, seen + 1U))
                    {
                        continue;
                    }
                    if (lh_atomic_uint_get_relaxed(&counter) > bound)
                    {
                        passed = true;
                    }
                    lh_atomic_uint_fetch_sub_acq_rel(&counter, 1U);
                }
            });
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    EXPECT_FALSE(passed);
    EXPECT_EQ(lh_atomic_uint_get_relaxed(&counter), 0U);
}

TEST(atomic_uint_death, rejects_null_counter)
{
    LH_EXPECT_DEATH(lh_atomic_uint_get_relaxed(lh_null));
    LH_EXPECT_DEATH(lh_atomic_uint_set_release(lh_null, 0U));
    LH_EXPECT_DEATH(lh_atomic_uint_fetch_add_acq_rel(lh_null, 1U));
}

} // namespace
