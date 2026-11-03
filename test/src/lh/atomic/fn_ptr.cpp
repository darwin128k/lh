#include <gtest/gtest.h>

#include <lh/atomic/fn_ptr.h>
#include <lh/atomic/fn_ptr/initializer.h>
#include <lh/expect/death.h>

#include <thread>

namespace
{

int g_state = 0;

void
fn_first(void)
{
    g_state = 1;
}

void
fn_second(void)
{
    g_state = 2;
}

TEST(atomic_fn_ptr_initializer, holds_its_value_before_any_store)
{
    lh_atomic_fn_ptr_t empty = lh_atomic_fn_ptr_initializer(lh_null);
    lh_atomic_fn_ptr_t set = lh_atomic_fn_ptr_initializer(fn_first);
    EXPECT_EQ(lh_atomic_fn_ptr_get_acquire(&empty), nullptr);
    EXPECT_EQ(lh_atomic_fn_ptr_get_acquire(&set), fn_first);
}

TEST(atomic_fn_ptr_set_release, replaces_the_function_read_back)
{
    lh_atomic_fn_ptr_t slot = lh_atomic_fn_ptr_initializer(lh_null);
    lh_atomic_fn_ptr_set_release(&slot, fn_first);
    EXPECT_EQ(lh_atomic_fn_ptr_get_acquire(&slot), fn_first);
    lh_atomic_fn_ptr_set_release(&slot, fn_second);
    EXPECT_EQ(lh_atomic_fn_ptr_get_acquire(&slot), fn_second);
    lh_atomic_fn_ptr_cb call = lh_atomic_fn_ptr_get_acquire(&slot);
    call();
    EXPECT_EQ(g_state, 2);
    lh_atomic_fn_ptr_set_release(&slot, lh_null);
    EXPECT_EQ(lh_atomic_fn_ptr_get_acquire(&slot), nullptr);
}

/* The reader sees what the writer did before publishing the function. */
TEST(atomic_fn_ptr_get_acquire, sees_what_was_written_before_the_release)
{
    for (int round = 0; round < 1000; ++round)
    {
        int data = 0;
        int seen = -1;
        lh_atomic_fn_ptr_t slot = lh_atomic_fn_ptr_initializer(lh_null);
        std::thread reader(
            [&]
            {
                while (lh_atomic_fn_ptr_get_acquire(&slot) == nullptr)
                {
                    std::this_thread::yield();
                }
                seen = data;
            });
        data = round + 1;
        lh_atomic_fn_ptr_set_release(&slot, fn_first);
        reader.join();
        ASSERT_EQ(seen, round + 1);
    }
}

TEST(atomic_fn_ptr_death, rejects_null_object)
{
    LH_EXPECT_DEATH(lh_atomic_fn_ptr_get_acquire(lh_null));
    LH_EXPECT_DEATH(lh_atomic_fn_ptr_set_release(lh_null, lh_null));
}

} // namespace
