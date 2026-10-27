#define _GNU_SOURCE
#include <lh/os/thread.h>
#include <lh/os/mutex.h>
#include <lh/os/condition.h>
#include <lh/os/tick.h>
#include <lh/os/system/tick.h>
#include <lh/os/system.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#    include <windows.h>
#else
#    include <pthread.h>
#    include <signal.h>
#    include <sched.h>
#endif

#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "concurrency:%d: %s\n", __LINE__, #value);                             \
            abort();                                                                               \
        }                                                                                          \
    } while (0)

typedef struct group
{
    lh_os_mutex_t mutex;
    lh_os_condition_t ready, wake;
    int waiting, permits, finished, counter;
} group;

static void
wait_for_permit(void *user)
{
    group *self = user;
    CHECK(lh_os_mutex_lock(&self->mutex));
    ++self->waiting;
    CHECK(lh_os_condition_signal(&self->ready));
    while (!self->permits)
    {
        CHECK(lh_os_condition_wait(&self->wake, &self->mutex));
    }
    --self->permits;
    ++self->finished;
    CHECK(lh_os_condition_signal(&self->ready));
    CHECK(lh_os_mutex_unlock(&self->mutex));
}
static void
increment(void *user)
{
    group *self = user;
    for (int i = 0; i < 10000; ++i)
    {
        CHECK(lh_os_mutex_lock(&self->mutex));
        ++self->counter;
        CHECK(lh_os_mutex_unlock(&self->mutex));
    }
}
static void
count_callback(void *user)
{
    atomic_fetch_add((atomic_int *)user, 1);
}

typedef struct self_join
{
    lh_os_mutex_t gate;
    lh_os_thread_t thread;
    int refused;
} self_join;
static void
refuse_self_join(void *user)
{
    self_join *self = user;
    CHECK(lh_os_mutex_lock(&self->gate));
    self->refused = !lh_os_thread_join(&self->thread);
    CHECK(lh_os_mutex_unlock(&self->gate));
}

typedef struct affinity_job
{
    int expected, matched;
} affinity_job;
static void
check_affinity(void *user)
{
    affinity_job *job = user;
#if defined(_WIN32)
    DWORD_PTR process_mask, system_mask;
    CHECK(GetProcessAffinityMask(GetCurrentProcess(), &process_mask, &system_mask));
    DWORD_PTR previous = SetThreadAffinityMask(GetCurrentThread(), process_mask);
    job->matched = previous == ((DWORD_PTR)1 << job->expected);
    CHECK(previous && SetThreadAffinityMask(GetCurrentThread(), previous));
#elif defined(__linux__)
    cpu_set_t set;
    CHECK(!pthread_getaffinity_np(pthread_self(), sizeof set, &set));
    job->matched = CPU_COUNT(&set) == 1 && CPU_ISSET(job->expected, &set);
#else
    job->matched = 1;
#endif
}

int
main(void)
{
    lh_u64_t before = lh_os_tick_us();
    CHECK(before > 0);
    CHECK(lh_os_thread_sleep(12));
    CHECK(lh_os_tick_us() - before >= 12000);
    CHECK(lh_os_tick_ms64() >= before / 1000);
    CHECK(!lh_os_system_tick_us(NULL));
    CHECK(lh_os_system_get_last_error_code() != 0);

    atomic_int callbacks = ATOMIC_VAR_INIT(0);
    lh_os_thread_t thread;
    lh_os_thread_init(&thread);
    CHECK(lh_os_thread_join(&thread));
    CHECK(!lh_os_thread_start(&thread, NULL, NULL, -1, 0));
    CHECK(!lh_os_thread_start(&thread, count_callback, &callbacks, -2, 0));
    CHECK(!lh_os_thread_start(&thread, count_callback, &callbacks, 1024, 0));
    CHECK(!lh_os_thread_start(&thread, count_callback, &callbacks, -1, 128));
    CHECK(atomic_load(&callbacks) == 0);
    CHECK(lh_os_thread_start(&thread, count_callback, &callbacks, -1, 0));
    CHECK(!lh_os_thread_start(&thread, count_callback, &callbacks, -1, 0));
    CHECK(lh_os_thread_join(&thread));
    CHECK(lh_os_thread_join(&thread));
    CHECK(atomic_load(&callbacks) == 1);

    self_join self;
    lh_os_thread_init(&self.thread);
    lh_os_mutex_init(&self.gate);
    CHECK(lh_os_mutex_open(&self.gate));
    CHECK(lh_os_mutex_lock(&self.gate));
    self.refused = 0;
    lh_u32_t owner_error = lh_os_system_get_last_error_code();
    CHECK(lh_os_thread_start(&self.thread, refuse_self_join, &self, -1, 0));
    CHECK(lh_os_mutex_unlock(&self.gate));
    CHECK(lh_os_thread_join(&self.thread));
    CHECK(self.refused);
    CHECK(lh_os_system_get_last_error_code() == owner_error);
    CHECK(lh_os_mutex_close(&self.gate));

    for (int round = 0; round < 20; ++round)
    {
        group state = {0};
        lh_os_mutex_init(&state.mutex);
        lh_os_condition_init(&state.ready);
        lh_os_condition_init(&state.wake);
        CHECK(!lh_os_mutex_lock(&state.mutex));
        CHECK(!lh_os_condition_signal(&state.ready));
        CHECK(lh_os_mutex_open(&state.mutex));
        CHECK(!lh_os_mutex_open(&state.mutex));
        CHECK(lh_os_condition_open(&state.ready));
        CHECK(lh_os_condition_open(&state.wake));
        lh_os_thread_t workers[4];
        CHECK(lh_os_mutex_lock(&state.mutex));
        for (int i = 0; i < 3; ++i)
        {
            lh_os_thread_init(&workers[i]);
            CHECK(lh_os_thread_start(&workers[i], wait_for_permit, &state, -1, 0));
        }
        while (state.waiting != 3)
        {
            CHECK(lh_os_condition_wait(&state.ready, &state.mutex));
        }
        state.permits = 1;
        CHECK(lh_os_condition_signal(&state.wake));
        while (state.finished != 1)
        {
            CHECK(lh_os_condition_wait(&state.ready, &state.mutex));
        }
        CHECK(state.permits == 0);
        state.permits = 2;
        CHECK(lh_os_condition_broadcast(&state.wake));
        CHECK(lh_os_mutex_unlock(&state.mutex));
        for (int i = 0; i < 3; ++i)
        {
            CHECK(lh_os_thread_join(&workers[i]));
        }
        CHECK(state.finished == 3);
        for (int i = 0; i < 4; ++i)
        {
            lh_os_thread_init(&workers[i]);
            CHECK(lh_os_thread_start(&workers[i], increment, &state, -1, 0));
        }
        for (int i = 0; i < 4; ++i)
        {
            CHECK(lh_os_thread_join(&workers[i]));
        }
        CHECK(state.counter == 40000);
        CHECK(lh_os_condition_close(&state.ready));
        CHECK(lh_os_condition_close(&state.wake));
        CHECK(lh_os_mutex_close(&state.mutex));
        CHECK(lh_os_mutex_close(&state.mutex));
    }

    affinity_job affinity = {-1, 0};
    int unavailable_cpu = -1;
#if defined(_WIN32)
    DWORD_PTR mask, system_mask;
    CHECK(GetProcessAffinityMask(GetCurrentProcess(), &mask, &system_mask));
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
    CHECK(!pthread_getaffinity_np(pthread_self(), sizeof mask, &mask));
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
        CHECK(!lh_os_thread_start(&thread, count_callback, &callbacks, unavailable_cpu, 0));
        CHECK(atomic_load(&callbacks) == 1);
    }
    CHECK(lh_os_thread_start(&thread, check_affinity, &affinity, affinity.expected, 0));
    CHECK(lh_os_thread_join(&thread));
    CHECK(affinity.matched);
    puts("PASS: clock/sleep, invalid starts, no failed-start callbacks, self-join, mutex "
         "contention, signal/broadcast, affinity");
    return 0;
}
