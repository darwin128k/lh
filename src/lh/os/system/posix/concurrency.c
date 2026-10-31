#ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#endif
#include <lh/os/system/posix/concurrency/thread.h>
#include <lh/os/system/error/capture.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#if defined(__linux__)
#    include <sched.h>
#endif
#include <lh/assert.h>

/**
 * @brief Return success for zero, or capture the POSIX error on this thread.
 */
static lh_bool_t
lh_os_concurrency_capture(int code)
{
    if (!code)
    {
        return lh_bool_true;
    }
    errno = code;
    lh_os_system_error_capture();
    return lh_bool_false;
}

/**
 * @brief Complete the startup handshake and invoke user code only when startup succeeds.
 */
static void *
lh_os_thread_native_run(void *arg)
{
    lh_os_native_thread_t *self = arg;
    int error = 0;
    if (lh_os_native_thread_get_flags(self) & lh_os_thread_block_sigpipe)
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGPIPE);
        error = pthread_sigmask(SIG_BLOCK, &mask, NULL);
    }
    pthread_mutex_lock(lh_os_native_thread_get_gate_mut(self));
    lh_os_native_thread_set_preparation_error(self, error);
    lh_os_native_thread_set_prepared(self, 1);
    pthread_cond_signal(lh_os_native_thread_get_ready_mut(self));
    while (!lh_os_native_thread_get_proceed(self))
    {
        pthread_cond_wait(lh_os_native_thread_get_ready_mut(self),
                          lh_os_native_thread_get_gate_mut(self));
    }
    int proceed = lh_os_native_thread_get_proceed(self);
    pthread_mutex_unlock(lh_os_native_thread_get_gate_mut(self));
    if (proceed > 0)
    {
        lh_os_native_thread_get_run_fn(self)(lh_os_native_thread_get_user(self));
    }
    return NULL;
}

/**
 * @brief Release startup synchronization and storage after the native worker has exited.
 */
static void
lh_os_thread_native_free(lh_os_native_thread_t *self)
{
    pthread_cond_destroy(lh_os_native_thread_get_ready_mut(self));
    pthread_mutex_destroy(lh_os_native_thread_get_gate_mut(self));
    free(self);
}

/**
 * @brief Create a native worker and validate its startup options before user code runs.
 */
lh_os_system_thread_handle_t
lh_os_system_thread_start(lh_os_thread_fn run, lh_ptr user, lh_s32_t cpu, lh_u32_t flags)
{
    if (!run || cpu < -1 || (flags & ~lh_os_thread_block_sigpipe))
    {
        lh_os_concurrency_capture(EINVAL);
        return NULL;
    }
#if defined(__linux__)
    if (cpu >= CPU_SETSIZE)
    {
        lh_os_concurrency_capture(EINVAL);
        return NULL;
    }
#else
    if (cpu >= 0)
    {
        lh_os_concurrency_capture(ENOTSUP);
        return NULL;
    }
#endif
    lh_os_native_thread_t *self = calloc(1, sizeof *self);
    if (!self)
    {
        lh_os_concurrency_capture(ENOMEM);
        return NULL;
    }
    lh_os_native_thread_set_run_fn(self, run);
    lh_os_native_thread_set_user(self, user);
    lh_os_native_thread_set_flags(self, flags);
    int code = pthread_mutex_init(lh_os_native_thread_get_gate_mut(self), NULL);
    if (code)
    {
        free(self);
        lh_os_concurrency_capture(code);
        return NULL;
    }
    code = pthread_cond_init(lh_os_native_thread_get_ready_mut(self), NULL);
    if (code)
    {
        pthread_mutex_destroy(lh_os_native_thread_get_gate_mut(self));
        free(self);
        lh_os_concurrency_capture(code);
        return NULL;
    }
    pthread_attr_t attributes;
    code = pthread_attr_init(&attributes);
    if (code)
    {
        lh_os_thread_native_free(self);
        lh_os_concurrency_capture(code);
        return NULL;
    }
#if defined(__linux__)
    if (cpu >= 0)
    {
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(cpu, &set);
        code = pthread_attr_setaffinity_np(&attributes, sizeof set, &set);
    }
#endif
    pthread_mutex_lock(lh_os_native_thread_get_gate_mut(self));
    if (!code)
    {
        code = pthread_create(lh_os_native_thread_get_thread_mut(self), &attributes,
                              lh_os_thread_native_run, self);
    }
    pthread_attr_destroy(&attributes);
    if (code)
    {
        pthread_mutex_unlock(lh_os_native_thread_get_gate_mut(self));
        lh_os_thread_native_free(self);
        lh_os_concurrency_capture(code);
        return NULL;
    }
    while (!lh_os_native_thread_get_prepared(self) && !code)
    {
        code = pthread_cond_wait(lh_os_native_thread_get_ready_mut(self),
                                 lh_os_native_thread_get_gate_mut(self));
    }
    if (!code)
    {
        code = lh_os_native_thread_get_preparation_error(self);
    }
    lh_os_native_thread_set_proceed(self, code ? -1 : 1);
    pthread_cond_signal(lh_os_native_thread_get_ready_mut(self));
    pthread_mutex_unlock(lh_os_native_thread_get_gate_mut(self));
    if (code)
    {
        pthread_join(lh_os_native_thread_get_thread(self), NULL);
        lh_os_thread_native_free(self);
        lh_os_concurrency_capture(code);
        return NULL;
    }
    return self;
}

/**
 * @brief Wait for a native worker and release its handle.
 */
lh_bool_t
lh_os_system_thread_join(lh_os_system_thread_handle_t handle)
{
    lh_os_native_thread_t *self = handle;
    if (!self)
    {
        return lh_os_concurrency_capture(EINVAL);
    }
    if (!lh_os_concurrency_capture(pthread_join(lh_os_native_thread_get_thread(self), NULL)))
    {
        return lh_bool_false;
    }
    lh_os_thread_native_free(self);
    return lh_bool_true;
}

/**
 * @brief Allocate and initialize one native mutex object.
 */
lh_os_system_mutex_handle_t
lh_os_system_mutex_open(void)
{
    pthread_mutex_t *self = malloc(sizeof *self);
    if (!self)
    {
        lh_os_concurrency_capture(ENOMEM);
        return NULL;
    }
    int code = pthread_mutex_init(self, NULL);
    if (code)
    {
        free(self);
        lh_os_concurrency_capture(code);
        return NULL;
    }
    return self;
}

/**
 * @brief Release a native mutex after its last use.
 */
lh_bool_t
lh_os_system_mutex_close(lh_os_system_mutex_handle_t handle)
{
    if (!handle)
    {
        return lh_os_concurrency_capture(EINVAL);
    }
    if (!lh_os_concurrency_capture(pthread_mutex_destroy(handle)))
    {
        return lh_bool_false;
    }
    free(handle);
    return lh_bool_true;
}

/**
 * @brief Lock a native mutex on the calling thread.
 */
lh_bool_t
lh_os_system_mutex_lock(lh_os_system_mutex_handle_t handle)
{
    return lh_os_concurrency_capture(handle ? pthread_mutex_lock(handle) : EINVAL);
}

/**
 * @brief Unlock the native mutex held by the calling thread.
 */
lh_bool_t
lh_os_system_mutex_unlock(lh_os_system_mutex_handle_t handle)
{
    return lh_os_concurrency_capture(handle ? pthread_mutex_unlock(handle) : EINVAL);
}

/**
 * @brief Allocate and initialize one native condition object.
 */
lh_os_system_condition_handle_t
lh_os_system_condition_open(void)
{
    pthread_cond_t *self = malloc(sizeof *self);
    if (!self)
    {
        lh_os_concurrency_capture(ENOMEM);
        return NULL;
    }
    int code = pthread_cond_init(self, NULL);
    if (code)
    {
        free(self);
        lh_os_concurrency_capture(code);
        return NULL;
    }
    return self;
}

/**
 * @brief Release a native condition after its last use.
 */
lh_bool_t
lh_os_system_condition_close(lh_os_system_condition_handle_t handle)
{
    if (!handle)
    {
        return lh_os_concurrency_capture(EINVAL);
    }
    if (!lh_os_concurrency_capture(pthread_cond_destroy(handle)))
    {
        return lh_bool_false;
    }
    free(handle);
    return lh_bool_true;
}

/**
 * @brief Wait on a native condition, atomically releasing and reacquiring the mutex.
 */
lh_bool_t
lh_os_system_condition_wait(lh_os_system_condition_handle_t handle,
                            lh_os_system_mutex_handle_t mutex)
{
    return lh_os_concurrency_capture(handle && mutex ? pthread_cond_wait(handle, mutex) : EINVAL);
}

/**
 * @brief Wait on a native condition at most @p timeout_ms; a timeout is a success.
 *
 * The deadline is on the real-time clock, as `pthread_cond_timedwait` reads it by
 * default: a clock step shortens or lengthens one wait, which the caller's
 * predicate loop absorbs.
 */
lh_bool_t
lh_os_system_condition_wait_for(lh_os_system_condition_handle_t handle,
                                lh_os_system_mutex_handle_t mutex, lh_u32_t timeout_ms)
{
    struct timespec deadline;

    if (!handle || !mutex || clock_gettime(CLOCK_REALTIME, &deadline))
    {
        return lh_os_concurrency_capture(EINVAL);
    }
    deadline.tv_sec += (time_t)(timeout_ms / 1000U);
    deadline.tv_nsec += (long)(timeout_ms % 1000U) * 1000000L;
    if (deadline.tv_nsec >= 1000000000L)
    {
        deadline.tv_sec += 1;
        deadline.tv_nsec -= 1000000000L;
    }
    int code = pthread_cond_timedwait(handle, mutex, &deadline);

    return lh_os_concurrency_capture(code == ETIMEDOUT ? 0 : code);
}

/**
 * @brief Notify one native condition waiter, if any.
 */
lh_bool_t
lh_os_system_condition_signal(lh_os_system_condition_handle_t handle)
{
    return lh_os_concurrency_capture(handle ? pthread_cond_signal(handle) : EINVAL);
}

/**
 * @brief Notify all current native condition waiters.
 */
lh_bool_t
lh_os_system_condition_broadcast(lh_os_system_condition_handle_t handle)
{
    return lh_os_concurrency_capture(handle ? pthread_cond_broadcast(handle) : EINVAL);
}

pthread_t
lh_os_native_thread_get_thread(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thread;
}

lh_void
lh_os_native_thread_set_thread(lh_os_native_thread_t *self, pthread_t thread)
{
    lh_assert_runtime_ref(self);
    self->thread = thread;
}

const pthread_mutex_t *
lh_os_native_thread_get_gate(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gate);
}

pthread_mutex_t *
lh_os_native_thread_get_gate_mut(lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gate);
}

const pthread_cond_t *
lh_os_native_thread_get_ready(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->ready);
}

pthread_cond_t *
lh_os_native_thread_get_ready_mut(lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->ready);
}

lh_os_thread_fn
lh_os_native_thread_get_run_fn(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->run;
}

lh_void
lh_os_native_thread_set_run_fn(lh_os_native_thread_t *self, lh_os_thread_fn run)
{
    lh_assert_runtime_ref(self);
    self->run = run;
}

lh_ptr
lh_os_native_thread_get_user(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_os_native_thread_set_user(lh_os_native_thread_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

lh_u32_t
lh_os_native_thread_get_flags(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->flags;
}

lh_void
lh_os_native_thread_set_flags(lh_os_native_thread_t *self, lh_u32_t flags)
{
    lh_assert_runtime_ref(self);
    self->flags = flags;
}

int
lh_os_native_thread_get_prepared(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->prepared;
}

lh_void
lh_os_native_thread_set_prepared(lh_os_native_thread_t *self, int prepared)
{
    lh_assert_runtime_ref(self);
    self->prepared = prepared;
}

int
lh_os_native_thread_get_preparation_error(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->preparation_error;
}

lh_void
lh_os_native_thread_set_preparation_error(lh_os_native_thread_t *self, int preparation_error)
{
    lh_assert_runtime_ref(self);
    self->preparation_error = preparation_error;
}

int
lh_os_native_thread_get_proceed(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->proceed;
}

lh_void
lh_os_native_thread_set_proceed(lh_os_native_thread_t *self, int proceed)
{
    lh_assert_runtime_ref(self);
    self->proceed = proceed;
}

pthread_t *
lh_os_native_thread_get_thread_mut(lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->thread);
}
