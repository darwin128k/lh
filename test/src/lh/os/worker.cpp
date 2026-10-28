#include <gtest/gtest.h>
#include <lh/os/worker.h>
#include <lh/runtime/allocator.h>
#include <lh/spsc.h>
#include <lh/test/os.h>
#include <atomic>
#include <cstdlib>
#include <memory>

namespace
{
using worker_owner = std::unique_ptr<lh_os_worker_t, decltype(&lh_os_worker_destroy)>;
using job_owner = std::unique_ptr<lh_os_worker_job_t, decltype(&lh_os_worker_dispose)>;
struct payload
{
    std::atomic_uint worked{0};
    unsigned destroyed = 0;
    lh_os_worker_t *closing = nullptr;
};

void
work(void *user)
{
    ++static_cast<payload *>(user)->worked;
}

void
dispose(void *user)
{
    auto &state = *static_cast<payload *>(user);
    ++state.destroyed;
    if (state.closing)
    {
        EXPECT_FALSE(lh_os_worker_submit(state.closing, work, nullptr, nullptr, user));
    }
}

struct allocator_case
{
    unsigned live = 0;
    bool reject = false;
    static lh_ptr
    alloc(lh_ptr context, lh_usize_t size)
    {
        auto &self = *static_cast<allocator_case *>(context);
        if (self.reject)
        {
            return nullptr;
        }
        auto block = std::malloc(size);
        if (block)
        {
            ++self.live;
        }
        return block;
    }
    static lh_void
    free(lh_ptr context, lh_ptr block)
    {
        auto &self = *static_cast<allocator_case *>(context);
        EXPECT_GT(self.live, 0u);
        if (self.live)
        {
            --self.live;
        }
        std::free(block);
    }
};
} // namespace
TEST(os_worker, rejects_zero_capacity)
{
    EXPECT_EQ(lh_os_worker_create(0), nullptr);
}

TEST(os_worker, retains_completed_jobs_in_capacity_and_rejects_submission_during_teardown)
{
    payload state;
    worker_owner worker(lh_os_worker_create(2), lh_os_worker_destroy);
    ASSERT_NE(worker, nullptr);
    ASSERT_TRUE(lh_os_worker_submit(worker.get(), work, nullptr, dispose, &state));
    ASSERT_TRUE(lh_os_worker_submit(worker.get(), work, nullptr, dispose, &state));
    ASSERT_TRUE(lh_test::wait_until([&] { return state.worked.load() == 2; }));
    EXPECT_FALSE(lh_os_worker_submit(worker.get(), work, nullptr, dispose, &state));
    job_owner job(lh_os_worker_take(worker.get()), lh_os_worker_dispose);
    ASSERT_NE(job, nullptr);
    job.reset();
    EXPECT_EQ(state.destroyed, 1u);
    ASSERT_TRUE(lh_os_worker_submit(worker.get(), work, nullptr, dispose, &state));
    lh_os_worker_stop(worker.get());
    EXPECT_FALSE(lh_os_worker_submit(worker.get(), work, nullptr, dispose, &state));
    state.closing = worker.get();
    worker.reset();
    EXPECT_EQ(state.destroyed, 3u);
}

TEST(os_worker, preserves_allocator_ownership_and_allocation_failures_are_recoverable)
{
    allocator_case state;
    const auto previous = *lh_runtime_allocator();
    lh_test::scope_exit restore([&] { *lh_runtime_allocator() = previous; });
    auto custom = previous;
    lh_memory_allocator_set(&custom, allocator_case::alloc, allocator_case::free);
    lh_memory_allocator_set_context(&custom, &state);
    *lh_runtime_allocator() = custom;
    state.reject = true;
    unsigned storage[2];
    EXPECT_EQ(lh_spsc_create(storage, 2, sizeof storage[0]), nullptr);
    EXPECT_EQ(lh_os_worker_create(2), nullptr);
    EXPECT_EQ(state.live, 0u);
    state.reject = false;
    std::unique_ptr<lh_spsc_t, decltype(&lh_spsc_destroy)> records(
        lh_spsc_create(storage, 2, sizeof storage[0]), lh_spsc_destroy);
    ASSERT_NE(records, nullptr);
    payload data;
    worker_owner worker(lh_os_worker_create(2), lh_os_worker_destroy);
    ASSERT_NE(worker, nullptr);
    state.reject = true;
    EXPECT_FALSE(lh_os_worker_submit(worker.get(), work, nullptr, nullptr, &data));
    state.reject = false;
    ASSERT_TRUE(lh_os_worker_submit(worker.get(), work, nullptr, nullptr, &data));
    job_owner job(nullptr, lh_os_worker_dispose);
    ASSERT_TRUE(lh_test::wait_until(
        [&]
        {
            job.reset(lh_os_worker_take(worker.get()));
            return !!job;
        }));
    *lh_runtime_allocator() = previous;
    worker.reset();
    job.reset();
    records.reset();
    EXPECT_EQ(state.live, 0u);
}
