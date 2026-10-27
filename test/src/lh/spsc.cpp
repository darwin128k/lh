#include <gtest/gtest.h>
#include <lh/spsc.h>
#include <lh/config.h>
#if LH_LIBRARY_OPTION_OS
#    include <lh/test/os.h>
#endif
#include <atomic>
#include <memory>

namespace
{
using queue_owner = std::unique_ptr<lh_spsc_t, decltype(&lh_spsc_destroy)>;
#if LH_LIBRARY_OPTION_OS
struct producer_state
{
    lh_spsc_t *queue;
    std::atomic_bool stop{false};
};
void
produce(void *user)
{
    auto &state = *static_cast<producer_state *>(user);
    for (unsigned i = 0; i < 100000 && !state.stop.load(); ++i)
    {
        while (!lh_spsc_push(state.queue, &i))
        {
            if (state.stop.load())
            {
                return;
            }
            EXPECT_TRUE(lh_os_thread_sleep(0));
        }
    }
}
#endif
} // namespace
TEST(spsc, rejects_invalid_capacity_and_rejects_newest_when_full)
{
    unsigned storage[8];
    EXPECT_EQ(lh_spsc_create(storage, 3, sizeof storage[0]), nullptr);
    queue_owner queue(lh_spsc_create(storage, 8, sizeof storage[0]), lh_spsc_destroy);
    ASSERT_NE(queue, nullptr);
    EXPECT_EQ(lh_spsc_peek(queue.get()), nullptr);
    for (unsigned i = 0; i < 8; ++i)
    {
        ASSERT_TRUE(lh_spsc_push(queue.get(), &i));
    }
    unsigned newest = 8;
    EXPECT_FALSE(lh_spsc_push(queue.get(), &newest));
    for (unsigned i = 0; i < 8; ++i)
    {
        const auto item = static_cast<const unsigned *>(lh_spsc_peek(queue.get()));
        ASSERT_NE(item, nullptr);
        EXPECT_EQ(*item, i);
        lh_spsc_release(queue.get());
    }
    EXPECT_EQ(lh_spsc_peek(queue.get()), nullptr);
}
#if LH_LIBRARY_OPTION_OS
TEST(spsc, preserves_order_across_wraparound_and_threads)
{
    unsigned storage[8];
    queue_owner queue(lh_spsc_create(storage, 8, sizeof storage[0]), lh_spsc_destroy);
    ASSERT_NE(queue, nullptr);
    producer_state state{queue.get()};
    lh_test::thread_owner producer;
    lh_test::scope_exit stop(
        [&]
        {
            state.stop = true;
            EXPECT_TRUE(lh_os_thread_join(&producer.value));
        });
    ASSERT_TRUE(lh_os_thread_start(&producer.value, produce, &state, -1, 0));
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);
    for (unsigned i = 0; i < 100000; ++i)
    {
        const unsigned *item = nullptr;
        while (!(item = static_cast<const unsigned *>(lh_spsc_peek(queue.get()))))
        {
            ASSERT_LT(std::chrono::steady_clock::now(), deadline) << "record " << i;
            ASSERT_TRUE(lh_os_thread_sleep(0));
        }
        ASSERT_EQ(*item, i) << "record " << i;
        lh_spsc_release(queue.get());
    }
    ASSERT_TRUE(lh_os_thread_join(&producer.value));
    EXPECT_EQ(lh_spsc_peek(queue.get()), nullptr);
}

#endif
