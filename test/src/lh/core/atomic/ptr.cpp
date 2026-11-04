#include <gtest/gtest.h>

#include <lh/core/atomic/ptr.h>
#include <lh/core/atomic/ptr/initializer.h>
#include <lh/expect/death.h>

#include <atomic>
#include <thread>
#include <vector>

namespace
{

TEST(atomic_ptr_initializer, holds_its_value_before_any_store)
{
    int target = 0;
    lh_atomic_ptr_t empty = lh_atomic_ptr_initializer(lh_null);
    lh_atomic_ptr_t set = lh_atomic_ptr_initializer(&target);
    EXPECT_EQ(lh_atomic_ptr_get_acquire(&empty), nullptr);
    EXPECT_EQ(lh_atomic_ptr_get_acquire(&set), &target);
}

TEST(atomic_ptr_set_release, replaces_the_value_read_back)
{
    int first = 0;
    int second = 0;
    lh_atomic_ptr_t slot = lh_atomic_ptr_initializer(lh_null);
    lh_atomic_ptr_set_release(&slot, &first);
    EXPECT_EQ(lh_atomic_ptr_get_acquire(&slot), &first);
    lh_atomic_ptr_set_release(&slot, &second);
    EXPECT_EQ(lh_atomic_ptr_get_acquire(&slot), &second);
    lh_atomic_ptr_set_release(&slot, lh_null);
    EXPECT_EQ(lh_atomic_ptr_get_acquire(&slot), nullptr);
}

/* The reader sees the object the writer finished before publishing it. */
TEST(atomic_ptr_get_acquire, sees_what_was_written_before_the_release)
{
    struct payload
    {
        int a;
        int b;
    };

    for (int round = 0; round < 1000; ++round)
    {
        payload data{0, 0};
        lh_atomic_ptr_t slot = lh_atomic_ptr_initializer(lh_null);
        payload seen{-1, -1};
        std::thread reader(
            [&]
            {
                lh_ptr got = lh_null;
                while ((got = lh_atomic_ptr_get_acquire(&slot)) == lh_null)
                {
                    std::this_thread::yield();
                }
                seen = *static_cast<const payload *>(got);
            });
        data.a = round;
        data.b = round + 1;
        lh_atomic_ptr_set_release(&slot, &data);
        reader.join();
        ASSERT_EQ(seen.a, round);
        ASSERT_EQ(seen.b, round + 1);
    }
}

TEST(atomic_ptr_set_relaxed, stores_the_value)
{
    int target = 0;
    lh_atomic_ptr_t slot = lh_atomic_ptr_initializer(lh_null);
    lh_atomic_ptr_set_relaxed(&slot, &target);
    EXPECT_EQ(lh_atomic_ptr_get_acquire(&slot), &target);
}

TEST(atomic_ptr_exchange_acq_rel, returns_the_value_before)
{
    int first = 0;
    int second = 0;
    lh_atomic_ptr_t slot = lh_atomic_ptr_initializer(&first);
    EXPECT_EQ(lh_atomic_ptr_exchange_acq_rel(&slot, &second), &first);
    EXPECT_EQ(lh_atomic_ptr_get_acquire(&slot), &second);
    EXPECT_EQ(lh_atomic_ptr_exchange_acq_rel(&slot, lh_null), &second);
}

/* Each exchange takes exactly one value: across threads every token is seen once. */
TEST(atomic_ptr_exchange_acq_rel, hands_each_value_to_one_thread)
{
    constexpr int per_thread = 10000;
    static int tokens[4 * per_thread];
    lh_atomic_ptr_t slot = lh_atomic_ptr_initializer(lh_null);
    std::atomic<int> seen{0};
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t)
    {
        threads.emplace_back(
            [&, t]
            {
                for (int i = 0; i < per_thread; ++i)
                {
                    if (lh_atomic_ptr_exchange_acq_rel(&slot, &tokens[t * per_thread + i]) !=
                        lh_null)
                    {
                        seen.fetch_add(1);
                    }
                }
            });
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    if (lh_atomic_ptr_exchange_acq_rel(&slot, lh_null) != lh_null)
    {
        seen.fetch_add(1);
    }
    EXPECT_EQ(seen.load(), 4 * per_thread);
}

TEST(atomic_ptr_death, rejects_null_object)
{
    LH_EXPECT_DEATH(lh_atomic_ptr_get_acquire(lh_null));
    LH_EXPECT_DEATH(lh_atomic_ptr_set_release(lh_null, lh_null));
}

} // namespace
