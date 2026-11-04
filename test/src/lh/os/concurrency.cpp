#ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#endif
#include <gtest/gtest.h>
#include <lh/core/library/fallback.h>
#include <lh/os/thread.h>
#include <lh/os/mutex.h>
#include <lh/os/condition.h>
#include <lh/os/tick.h>
#include <lh/os/system/tick.h>
#include <lh/os/system.h>
#include <lh/test/os.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/core/runtime/allocator.h>
#include <cstdlib>
#include <atomic>
#if defined(_WIN32)
#    include <windows.h>
#else
#    include <pthread.h>
#    include <signal.h>
#    include <sched.h>
#endif
namespace
{
struct group
{
    lh_os_mutex_t mutex;
    lh_os_condition_t ready, wake;
    int waiting, permits, finished, counter;
};

/**
 * @brief Exercise predicate-based waiting and one/all-waiter notification.
 */
static void
wait_for_permit(void *user)
{
    group *self = static_cast<group *>(user);
    ASSERT_TRUE(lh_os_mutex_lock(&self->mutex));
    lh_test::scope_exit unlock([&] { EXPECT_TRUE(lh_os_mutex_unlock(&self->mutex)); });
    ++self->waiting;
    ASSERT_TRUE(lh_os_condition_signal(&self->ready));
    while (!self->permits)
    {
        ASSERT_TRUE(lh_os_condition_wait(&self->wake, &self->mutex));
    }
    --self->permits;
    ++self->finished;
    ASSERT_TRUE(lh_os_condition_signal(&self->ready));
}

/**
 * @brief Exercise mutual exclusion by incrementing a shared counter from several workers.
 */
static void
increment(void *user)
{
    group *self = static_cast<group *>(user);
    for (int i = 0; i < 10000; ++i)
    {
        ASSERT_TRUE(lh_os_mutex_lock(&self->mutex));
        lh_test::scope_exit unlock([&] { EXPECT_TRUE(lh_os_mutex_unlock(&self->mutex)); });
        ++self->counter;
    }
}

/**
 * @brief Record whether a worker callback executed, including rejected-start checks.
 */
static void
count_callback(void *user)
{
    static_cast<std::atomic_int *>(user)->fetch_add(1);
}

struct self_join
{
    lh_os_mutex_t gate;
    lh_os_thread_t thread;
    int refused;
};

/**
 * @brief Verify that self-join fails without consuming the worker handle.
 */
static void
refuse_self_join(void *user)
{
    self_join *self = static_cast<self_join *>(user);
    ASSERT_TRUE(lh_os_mutex_lock(&self->gate));
    lh_test::scope_exit unlock([&] { EXPECT_TRUE(lh_os_mutex_unlock(&self->gate)); });
    self->refused = !lh_os_thread_join(&self->thread);
}

struct affinity_job
{
    int expected, matched;
};

/**
 * @brief Verify that an explicit CPU request is applied to the running worker.
 */
static void
check_affinity(void *user)
{
    affinity_job *job = static_cast<affinity_job *>(user);
#if defined(_WIN32)
    DWORD_PTR process_mask, system_mask;
    ASSERT_TRUE(GetProcessAffinityMask(GetCurrentProcess(), &process_mask, &system_mask));
    DWORD_PTR previous = SetThreadAffinityMask(GetCurrentThread(), process_mask);
    job->matched = previous == ((DWORD_PTR)1 << job->expected);
    ASSERT_TRUE(previous && SetThreadAffinityMask(GetCurrentThread(), previous));
#elif defined(__linux__)
    cpu_set_t set;
    ASSERT_FALSE(pthread_getaffinity_np(pthread_self(), sizeof set, &set));
    job->matched = CPU_COUNT(&set) == 1 && CPU_ISSET(job->expected, &set);
#else
    job->matched = 1;
#endif
}

class os_condition : public testing::TestWithParam<int>
{
};

/* Counts the native storage taken from and given back to the runtime
   allocator; refuses every allocation while reject is set. */
struct storage_counter
{
    int allocs = 0;
    int frees = 0;
    bool reject = false;
};

lh_ptr
storage_alloc(lh_self_ptr self, lh_usize_t size)
{
    auto *counter = static_cast<storage_counter *>(self);
    if (counter->reject)
    {
        return nullptr;
    }
    ++counter->allocs;
    return std::malloc(size);
}

lh_void
storage_free(lh_self_ptr self, lh_ptr ptr)
{
    auto *counter = static_cast<storage_counter *>(self);
    if (ptr != nullptr)
    {
        ++counter->frees;
        std::free(ptr);
    }
}

/* Puts the counting allocator on the runtime slot for one scope. */
class counting_runtime_allocator
{
  public:
    explicit counting_runtime_allocator(storage_counter &counter)
    {
        lh_memory_allocator_assign(&m_saved, lh_runtime_allocator_get());
        lh_memory_allocator_set(lh_runtime_allocator_get_mut(), storage_alloc, storage_free);
        lh_memory_allocator_set_context(lh_runtime_allocator_get_mut(), &counter);
    }

    ~counting_runtime_allocator()
    {
        lh_memory_allocator_assign(lh_runtime_allocator_get_mut(), &m_saved);
    }

    counting_runtime_allocator(const counting_runtime_allocator &) = delete;
    counting_runtime_allocator &operator=(const counting_runtime_allocator &) = delete;

  private:
    lh_memory_allocator_t m_saved;
};

} // namespace

/* The native object lives in storage from the runtime allocator, and goes back
   to the same allocator on close even when the runtime slot changed since. */
TEST(os_mutex, native_storage_comes_from_the_runtime_allocator)
{
    storage_counter counter;
    lh_os_mutex_t mutex;
    lh_os_mutex_init(&mutex);
    {
        counting_runtime_allocator scope(counter);
        ASSERT_TRUE(lh_os_mutex_open(&mutex));
    }
    EXPECT_EQ(counter.allocs, 1);
    EXPECT_EQ(counter.frees, 0);
    EXPECT_TRUE(lh_os_mutex_lock(&mutex));
    EXPECT_TRUE(lh_os_mutex_unlock(&mutex));
    EXPECT_TRUE(lh_os_mutex_close(&mutex));
    EXPECT_EQ(counter.frees, 1);
}

TEST(os_mutex, open_fails_without_memory)
{
    storage_counter counter;
    counter.reject = true;
    counting_runtime_allocator scope(counter);
    lh_os_mutex_t mutex;
    lh_os_mutex_init(&mutex);
    EXPECT_FALSE(lh_os_mutex_open(&mutex));
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_out_of_memory);
    EXPECT_EQ(lh_os_mutex_get_handle(&mutex), nullptr);
    EXPECT_TRUE(lh_os_mutex_close(&mutex));
}

TEST(os_condition, native_storage_comes_from_the_runtime_allocator)
{
    storage_counter counter;
    lh_os_condition_t condition;
    lh_os_condition_init(&condition);
    {
        counting_runtime_allocator scope(counter);
        ASSERT_TRUE(lh_os_condition_open(&condition));
    }
    EXPECT_EQ(counter.allocs, 1);
    EXPECT_TRUE(lh_os_condition_signal(&condition));
    EXPECT_TRUE(lh_os_condition_close(&condition));
    EXPECT_EQ(counter.frees, 1);
}

TEST(os_condition, open_fails_without_memory)
{
    storage_counter counter;
    counter.reject = true;
    counting_runtime_allocator scope(counter);
    lh_os_condition_t condition;
    lh_os_condition_init(&condition);
    EXPECT_FALSE(lh_os_condition_open(&condition));
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_out_of_memory);
    EXPECT_EQ(lh_os_condition_get_handle(&condition), nullptr);
}

TEST(os_thread, native_storage_comes_from_the_runtime_allocator)
{
    storage_counter counter;
    std::atomic_int callbacks{0};
    lh_os_thread_t thread;
    lh_os_thread_init(&thread);
    {
        counting_runtime_allocator scope(counter);
        ASSERT_TRUE(lh_os_thread_start(&thread, count_callback, &callbacks, -1, 0));
    }
    EXPECT_EQ(counter.allocs, 1);
    ASSERT_TRUE(lh_os_thread_join(&thread));
    EXPECT_EQ(counter.frees, 1);
    EXPECT_EQ(callbacks.load(), 1);
}

TEST(os_thread, a_rejected_start_gives_its_storage_back)
{
    storage_counter counter;
    std::atomic_int callbacks{0};
    lh_os_thread_t thread;
    lh_os_thread_init(&thread);
    {
        counting_runtime_allocator scope(counter);
        EXPECT_FALSE(lh_os_thread_start(&thread, count_callback, &callbacks, -2, 0));
    }
    EXPECT_EQ(counter.allocs, counter.frees);
    EXPECT_EQ(callbacks.load(), 0);
}

TEST(os_thread, start_fails_without_memory)
{
    storage_counter counter;
    counter.reject = true;
    std::atomic_int callbacks{0};
    counting_runtime_allocator scope(counter);
    lh_os_thread_t thread;
    lh_os_thread_init(&thread);
    EXPECT_FALSE(lh_os_thread_start(&thread, count_callback, &callbacks, -1, 0));
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_out_of_memory);
    EXPECT_EQ(callbacks.load(), 0);
}

TEST(os_clock, monotonic_sleep_and_invalid_output)
{

    lh_u64_t before = lh_os_tick_us();
    ASSERT_GT(before, 0);
    ASSERT_TRUE(lh_os_thread_sleep(12));
    ASSERT_GE(lh_os_tick_us() - before, 12000);
    ASSERT_GE(lh_os_tick_ms64(), before / 1000);
    ASSERT_FALSE(lh_os_system_tick_us(nullptr));
    ASSERT_NE(lh_os_system_get_last_error_code(), 0);
}

TEST(os_thread, invalid_starts_never_run_callbacks_and_join_is_idempotent)
{
    std::atomic_int callbacks{0};
    lh_test::thread_owner owner;
    auto &thread = owner.value;
    ASSERT_TRUE(lh_os_thread_join(&thread));
    ASSERT_FALSE(lh_os_thread_start(&thread, nullptr, nullptr, -1, 0));
    ASSERT_FALSE(lh_os_thread_start(&thread, count_callback, &callbacks, -2, 0));
    ASSERT_FALSE(lh_os_thread_start(&thread, count_callback, &callbacks, 1024, 0));
    ASSERT_FALSE(lh_os_thread_start(&thread, count_callback, &callbacks, -1, 128));
    ASSERT_EQ(callbacks.load(), 0);
    ASSERT_TRUE(lh_os_thread_start(&thread, count_callback, &callbacks, -1, 0));
    ASSERT_FALSE(lh_os_thread_start(&thread, count_callback, &callbacks, -1, 0));
    ASSERT_TRUE(lh_os_thread_join(&thread));
    ASSERT_TRUE(lh_os_thread_join(&thread));
    ASSERT_EQ(callbacks.load(), 1);
}

TEST(os_thread, self_join_preserves_worker_and_owner_error)
{
    self_join self;
    bool locked = false;
    lh_os_thread_init(&self.thread);
    lh_os_mutex_init(&self.gate);
    lh_test::scope_exit cleanup(
        [&]
        {
            if (locked)
            {
                EXPECT_TRUE(lh_os_mutex_unlock(&self.gate));
            }
            EXPECT_TRUE(lh_os_thread_join(&self.thread));
            EXPECT_TRUE(lh_os_mutex_close(&self.gate));
        });
    ASSERT_TRUE(lh_os_mutex_open(&self.gate));
    ASSERT_TRUE(locked = lh_os_mutex_lock(&self.gate));
    self.refused = 0;
    auto owner_error = lh_os_system_get_last_error_code();
    ASSERT_TRUE(lh_os_thread_start(&self.thread, refuse_self_join, &self, -1, 0));
    ASSERT_TRUE(lh_os_mutex_unlock(&self.gate));
    locked = false;
    ASSERT_TRUE(lh_os_thread_join(&self.thread));
    ASSERT_TRUE(self.refused);
#if (LH_LIBRARY_OPTION_THREAD_LOCAL == LH_LIBRARY_OPTION_ON)
    /* The captured error is per thread only with thread-local storage; with
       THREAD_LOCAL=OFF every thread shares one slot, so the worker's error stays. */
    ASSERT_EQ(lh_os_system_get_last_error_code(), owner_error);
#else
    (void)owner_error;
#endif
    ASSERT_TRUE(lh_os_mutex_close(&self.gate));
}

TEST_P(os_condition, signal_broadcast_and_mutex_contention)
{
    SCOPED_TRACE(GetParam());
    group state{};
    bool mutex_open = false, ready_open = false, wake_open = false, locked = false;
    lh_os_thread_t workers[4];
    for (auto &worker : workers)
    {
        lh_os_thread_init(&worker);
    }
    lh_os_mutex_init(&state.mutex);
    lh_os_condition_init(&state.ready);
    lh_os_condition_init(&state.wake);
    lh_test::scope_exit cleanup(
        [&]
        {
            if (locked)
            {
                EXPECT_TRUE(lh_os_mutex_unlock(&state.mutex));
            }
            if (mutex_open && wake_open && lh_os_mutex_lock(&state.mutex))
            {
                state.permits = 4;
                EXPECT_TRUE(lh_os_condition_broadcast(&state.wake));
                EXPECT_TRUE(lh_os_mutex_unlock(&state.mutex));
            }
            for (auto &worker : workers)
            {
                EXPECT_TRUE(lh_os_thread_join(&worker));
            }
            if (ready_open)
            {
                EXPECT_TRUE(lh_os_condition_close(&state.ready));
            }
            if (wake_open)
            {
                EXPECT_TRUE(lh_os_condition_close(&state.wake));
            }
            if (mutex_open)
            {
                EXPECT_TRUE(lh_os_mutex_close(&state.mutex));
            }
        });
    ASSERT_FALSE(lh_os_mutex_lock(&state.mutex));
    ASSERT_FALSE(lh_os_condition_signal(&state.ready));
    ASSERT_TRUE(mutex_open = lh_os_mutex_open(&state.mutex));
    ASSERT_FALSE(lh_os_mutex_open(&state.mutex));
    ASSERT_TRUE(ready_open = lh_os_condition_open(&state.ready));
    ASSERT_TRUE(wake_open = lh_os_condition_open(&state.wake));
    ASSERT_TRUE(locked = lh_os_mutex_lock(&state.mutex));
    auto wait_locked = [&](auto predicate)
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        while (!predicate())
        {
            if (std::chrono::steady_clock::now() >= deadline)
            {
                return false;
            }
            if (!lh_os_mutex_unlock(&state.mutex))
            {
                return false;
            }
            locked = false;
            if (!lh_os_thread_sleep(1))
            {
                return false;
            }
            locked = lh_os_mutex_lock(&state.mutex);
            if (!locked)
            {
                return false;
            }
        }
        return true;
    };
    for (int i = 0; i < 3; ++i)
    {
        lh_os_thread_init(&workers[i]);
        ASSERT_TRUE(lh_os_thread_start(&workers[i], wait_for_permit, &state, -1, 0));
    }
    ASSERT_TRUE(wait_locked([&] { return state.waiting == 3; }));
    state.permits = 1;
    ASSERT_TRUE(lh_os_condition_signal(&state.wake));
    ASSERT_TRUE(wait_locked([&] { return state.finished == 1; }));
    ASSERT_EQ(state.permits, 0);
    state.permits = 2;
    ASSERT_TRUE(lh_os_condition_broadcast(&state.wake));
    ASSERT_TRUE(lh_os_mutex_unlock(&state.mutex));
    locked = false;
    for (int i = 0; i < 3; ++i)
    {
        ASSERT_TRUE(lh_os_thread_join(&workers[i]));
    }
    ASSERT_EQ(state.finished, 3);
    for (int i = 0; i < 4; ++i)
    {
        lh_os_thread_init(&workers[i]);
        ASSERT_TRUE(lh_os_thread_start(&workers[i], increment, &state, -1, 0));
    }
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_TRUE(lh_os_thread_join(&workers[i]));
    }
    ASSERT_EQ(state.counter, 40000);
    ASSERT_TRUE(lh_os_condition_close(&state.ready));
    ready_open = false;
    ASSERT_TRUE(lh_os_condition_close(&state.wake));
    wake_open = false;
    ASSERT_TRUE(lh_os_mutex_close(&state.mutex));
    ASSERT_TRUE(lh_os_mutex_close(&state.mutex));
    mutex_open = false;
}

INSTANTIATE_TEST_SUITE_P(repeated_synchronization, os_condition, testing::Range(0, 20));

TEST(os_thread, applies_affinity_and_rejects_unavailable_cpu)
{
    std::atomic_int callbacks{0};
    lh_test::thread_owner owner;
    auto &thread = owner.value;
    affinity_job affinity = {-1, 0};
    int unavailable_cpu = -1;
#if defined(_WIN32)
    DWORD_PTR mask, system_mask;
    ASSERT_TRUE(GetProcessAffinityMask(GetCurrentProcess(), &mask, &system_mask));
    for (int i = 0; i < (int)(8 * sizeof mask); ++i)
    {
        if (mask & ((DWORD_PTR)1 << i))
        {
            affinity.expected = i;
            break;
        }
    }
    for (int i = 0; i < (int)(8 * sizeof mask); ++i)
    {
        if (!(mask & ((DWORD_PTR)1 << i)))
        {
            unavailable_cpu = i;
            break;
        }
    }
#elif defined(__linux__)
    cpu_set_t mask;
    ASSERT_FALSE(pthread_getaffinity_np(pthread_self(), sizeof mask, &mask));
    for (int i = 0; i < CPU_SETSIZE; ++i)
    {
        if (CPU_ISSET(i, &mask))
        {
            affinity.expected = i;
            break;
        }
    }
#endif
    if (unavailable_cpu >= 0)
    {
        ASSERT_FALSE(lh_os_thread_start(&thread, count_callback, &callbacks, unavailable_cpu, 0));
        ASSERT_EQ(callbacks.load(), 0);
    }
    ASSERT_TRUE(lh_os_thread_start(&thread, check_affinity, &affinity, affinity.expected, 0));
    ASSERT_TRUE(lh_os_thread_join(&thread));
    ASSERT_TRUE(affinity.matched);
}

/* Nobody signals: the wait ends on its own, with the mutex held again. */
TEST(os_condition, wait_for_returns_after_the_timeout)
{
    lh_os_condition_t condition;
    lh_os_mutex_t mutex;

    lh_os_condition_init(&condition);
    lh_os_mutex_init(&mutex);
    ASSERT_TRUE(lh_os_condition_open(&condition));
    ASSERT_TRUE(lh_os_mutex_open(&mutex));
    ASSERT_TRUE(lh_os_mutex_lock(&mutex));
    const lh_u64_t before = lh_os_tick_us();
    EXPECT_TRUE(lh_os_condition_wait_for(&condition, &mutex, 50U));
    const lh_u64_t waited = (lh_os_tick_us() - before) / 1000U;
    EXPECT_GE(waited, 30U);
    EXPECT_LT(waited, 2000U);
    EXPECT_TRUE(lh_os_mutex_unlock(&mutex));
    EXPECT_TRUE(lh_os_mutex_close(&mutex));
    EXPECT_TRUE(lh_os_condition_close(&condition));
}
