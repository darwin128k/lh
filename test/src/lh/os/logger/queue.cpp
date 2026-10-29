#include <gtest/gtest.h>

#include <lh/os/logger/queue.h>
#include <lh/os/thread.h>
#include <lh/attribute/unused.h>

#include <atomic>
#include <string>
#include <vector>

namespace
{

/* A sink that keeps what it gets; `stall` holds the worker inside emit. */
struct memory_sink
{
    std::vector<std::string> lines;
    std::atomic<int> flushes{0};
    std::atomic<int> closes{0};
    std::atomic<lh_u32_t> dropped{0};
    std::atomic<bool> stall{false};
    std::atomic<bool> stalled{false};

    static lh_void
    emit(lh_ptr user, const lh_os_logger_record_t *record)
    {
        auto *self = static_cast<memory_sink *>(user);

        while (self->stall.load())
        {
            self->stalled.store(true);
            LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
        }
        self->lines.emplace_back(lh_os_logger_record_get_text(record));
    }

    static lh_void
    flush(lh_ptr user)
    {
        static_cast<memory_sink *>(user)->flushes.fetch_add(1);
    }

    static lh_void
    close(lh_ptr user)
    {
        static_cast<memory_sink *>(user)->closes.fetch_add(1);
    }

    static lh_void
    drop(lh_ptr user, lh_u32_t count)
    {
        static_cast<memory_sink *>(user)->dropped.fetch_add(count);
    }

    lh_os_logger_sink_t
    as_sink()
    {
        lh_os_logger_sink_t sink{};

        lh_os_logger_sink_set_emit_fn(&sink, emit);
        lh_os_logger_sink_set_flush_fn(&sink, flush);
        lh_os_logger_sink_set_close_fn(&sink, close);
        lh_os_logger_sink_set_dropped_fn(&sink, drop);
        lh_os_logger_sink_set_user(&sink, this);
        return sink;
    }
};

lh_os_logger_record_t
line(const char *text)
{
    lh_os_logger_record_t record{};

    lh_os_logger_record_init_line(&record, 1U, lh_logger_level_info, 0, nullptr, text);
    return record;
}

} // namespace

TEST(os_logger_queue, records_reach_the_sink_in_order_and_sync_flushes)
{
    memory_sink memory;
    const lh_os_logger_sink_t sink = memory.as_sink();
    lh_os_logger_queue_t *queue = lh_os_logger_queue_try_create(16U, 1000U, &sink);

    ASSERT_NE(queue, nullptr);
    for (int i = 0; i < 10; ++i)
    {
        const lh_os_logger_record_t record = line(std::to_string(i).c_str());

        ASSERT_TRUE(lh_os_logger_queue_push(queue, &record));
    }
    const int before = memory.flushes.load();
    ASSERT_TRUE(lh_os_logger_queue_sync(queue, 5000U));
    EXPECT_GT(memory.flushes.load(), before);
    ASSERT_EQ(memory.lines.size(), 10U);
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(memory.lines[static_cast<size_t>(i)], std::to_string(i));
    }
    lh_os_logger_queue_destroy(queue);
    EXPECT_EQ(memory.closes.load(), 1);
}

TEST(os_logger_queue, a_full_queue_drops_and_counts_and_the_sink_hears_of_it)
{
    memory_sink memory;
    const lh_os_logger_sink_t sink = memory.as_sink();
    lh_os_logger_queue_t *queue = lh_os_logger_queue_try_create(4U, 1000U, &sink);
    const lh_os_logger_record_t record = line("x");
    int accepted = 0;

    ASSERT_NE(queue, nullptr);
    memory.stall.store(true);
    ASSERT_TRUE(lh_os_logger_queue_push(queue, &record));
    while (!memory.stalled.load())
    {
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
    }
    /* The stalled record is still in its slot: three more fit, the rest drop. */
    for (int i = 0; i < 10; ++i)
    {
        accepted += lh_os_logger_queue_push(queue, &record) ? 1 : 0;
    }
    EXPECT_EQ(accepted, 3);
    EXPECT_EQ(lh_os_logger_queue_get_dropped(queue), 7U);
    memory.stall.store(false);
    ASSERT_TRUE(lh_os_logger_queue_sync(queue, 5000U));
    EXPECT_EQ(memory.lines.size(), 4U);
    EXPECT_EQ(memory.dropped.load(), 7U);
    lh_os_logger_queue_destroy(queue);
}

TEST(os_logger_queue, a_stalled_sink_makes_sync_time_out_and_destroy_drains)
{
    memory_sink memory;
    const lh_os_logger_sink_t sink = memory.as_sink();
    lh_os_logger_queue_t *queue = lh_os_logger_queue_try_create(8U, 1000U, &sink);
    const lh_os_logger_record_t record = line("y");

    ASSERT_NE(queue, nullptr);
    memory.stall.store(true);
    ASSERT_TRUE(lh_os_logger_queue_push(queue, &record));
    ASSERT_TRUE(lh_os_logger_queue_push(queue, &record));
    EXPECT_FALSE(lh_os_logger_queue_sync(queue, 50U));
    memory.stall.store(false);
    lh_os_logger_queue_destroy(queue);
    EXPECT_EQ(memory.lines.size(), 2U);
    EXPECT_EQ(memory.closes.load(), 1);
}

TEST(os_logger_queue, the_worker_flushes_on_its_period_without_a_sync)
{
    memory_sink memory;
    const lh_os_logger_sink_t sink = memory.as_sink();
    lh_os_logger_queue_t *queue = lh_os_logger_queue_try_create(8U, 10U, &sink);

    ASSERT_NE(queue, nullptr);
    LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(100U));
    EXPECT_GE(memory.flushes.load(), 3);
    lh_os_logger_queue_destroy(queue);
}

TEST(os_logger_queue, bad_arguments_create_nothing)
{
    memory_sink memory;
    lh_os_logger_sink_t sink = memory.as_sink();

    EXPECT_EQ(lh_os_logger_queue_try_create(3U, 100U, &sink), nullptr);
    EXPECT_EQ(lh_os_logger_queue_try_create(8U, 0U, &sink), nullptr);
    EXPECT_EQ(lh_os_logger_queue_try_create(8U, 100U, nullptr), nullptr);
    lh_os_logger_sink_set_flush_fn(&sink, nullptr);
    EXPECT_EQ(lh_os_logger_queue_try_create(8U, 100U, &sink), nullptr);
}
