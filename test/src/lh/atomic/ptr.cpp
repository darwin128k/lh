#include <gtest/gtest.h>

#include <lh/atomic/ptr.h>
#include <lh/atomic/ptr/initializer.h>
#include <lh/expect/death.h>

#include <thread>

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

TEST(atomic_ptr_death, rejects_null_object)
{
    LH_EXPECT_DEATH(lh_atomic_ptr_get_acquire(lh_null));
    LH_EXPECT_DEATH(lh_atomic_ptr_set_release(lh_null, lh_null));
}

} // namespace
