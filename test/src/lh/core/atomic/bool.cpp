#include <gtest/gtest.h>

#include <lh/core/atomic/bool.h>
#include <lh/core/atomic/bool/initializer.h>
#include <lh/expect/death.h>

#include <thread>

namespace
{

TEST(atomic_bool_initializer, holds_its_value_before_any_store)
{
    lh_atomic_bool_t off = lh_atomic_bool_initializer(lh_bool_false);
    lh_atomic_bool_t on = lh_atomic_bool_initializer(lh_bool_true);
    EXPECT_FALSE(lh_atomic_bool_get_acquire(&off));
    EXPECT_TRUE(lh_atomic_bool_get_acquire(&on));
}

TEST(atomic_bool_get_value, is_the_counter_that_holds_the_flag)
{
    lh_atomic_bool_t flag = lh_atomic_bool_initializer(lh_bool_true);
    EXPECT_EQ(lh_atomic_uint_get_relaxed(lh_atomic_bool_get_value(&flag)), 1U);
    EXPECT_EQ(lh_atomic_bool_get_value(&flag), lh_atomic_bool_get_value_mut(&flag));
}

TEST(atomic_bool_set, relaxed_and_release_store_the_flag)
{
    lh_atomic_bool_t flag = lh_atomic_bool_initializer(lh_bool_false);
    lh_atomic_bool_set_relaxed(&flag, lh_bool_true);
    EXPECT_TRUE(lh_atomic_bool_get_acquire(&flag));
    lh_atomic_bool_set_release(&flag, lh_bool_false);
    EXPECT_FALSE(lh_atomic_bool_get_acquire(&flag));
}

TEST(atomic_bool_exchange_acq_rel, returns_whether_it_was_set)
{
    lh_atomic_bool_t flag = lh_atomic_bool_initializer(lh_bool_true);
    EXPECT_TRUE(lh_atomic_bool_exchange_acq_rel(&flag, lh_bool_false));
    EXPECT_FALSE(lh_atomic_bool_exchange_acq_rel(&flag, lh_bool_false));
    EXPECT_FALSE(lh_atomic_bool_get_acquire(&flag));
}

/* The thread that clears the flag sees what the setter did before setting it. */
TEST(atomic_bool_exchange_acq_rel, sees_what_was_written_before_the_set)
{
    for (int round = 0; round < 1000; ++round)
    {
        int data = 0;
        int seen = -1;
        lh_atomic_bool_t pending = lh_atomic_bool_initializer(lh_bool_false);
        std::thread taker(
            [&]
            {
                while (!lh_atomic_bool_exchange_acq_rel(&pending, lh_bool_false))
                {
                    std::this_thread::yield();
                }
                seen = data;
            });
        data = round + 1;
        lh_atomic_bool_set_release(&pending, lh_bool_true);
        taker.join();
        ASSERT_EQ(seen, round + 1);
    }
}

TEST(atomic_bool_death, rejects_null_flag)
{
    LH_EXPECT_DEATH(lh_atomic_bool_get_acquire(lh_null));
    LH_EXPECT_DEATH(lh_atomic_bool_set_release(lh_null, lh_bool_true));
}

} // namespace
