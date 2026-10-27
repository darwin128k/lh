#ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#endif
#include <lh/os/system/concurrency.h>
#include <lh/os/system/error/capture.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#if defined(__linux__)
#    include <sched.h>
#endif

typedef struct lh_os_native_thread
{
    pthread_t thread;
    pthread_mutex_t gate;
    pthread_cond_t ready;
    lh_os_thread_fn run;
    lh_ptr user;
    lh_u32_t flags;
    int prepared, preparation_error, proceed;
} lh_os_native_thread_t;

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
    if (self->flags & lh_os_thread_block_sigpipe)
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGPIPE);
        error = pthread_sigmask(SIG_BLOCK, &mask, NULL);
    }
    pthread_mutex_lock(&self->gate);
    self->preparation_error = error;
    self->prepared = 1;
    pthread_cond_signal(&self->ready);
    while (!self->proceed)
    {
        pthread_cond_wait(&self->ready, &self->gate);
    }
    int proceed = self->proceed;
    pthread_mutex_unlock(&self->gate);
    if (proceed > 0)
    {
        self->run(self->user);
    }
    return NULL;
}

/**
 * @brief Release startup synchronization and storage after the native worker has exited.
 */
static void
lh_os_thread_native_free(lh_os_native_thread_t *self)
{
    pthread_cond_destroy(&self->ready);
    pthread_mutex_destroy(&self->gate);
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
    self->run = run;
    self->user = user;
    self->flags = flags;
    int code = pthread_mutex_init(&self->gate, NULL);
    if (code)
    {
        free(self);
        lh_os_concurrency_capture(code);
        return NULL;
    }
    code = pthread_cond_init(&self->ready, NULL);
    if (code)
    {
        pthread_mutex_destroy(&self->gate);
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
    pthread_mutex_lock(&self->gate);
    if (!code)
    {
        code = pthread_create(&self->thread, &attributes, lh_os_thread_native_run, self);
    }
    pthread_attr_destroy(&attributes);
    if (code)
    {
        pthread_mutex_unlock(&self->gate);
        lh_os_thread_native_free(self);
        lh_os_concurrency_capture(code);
        return NULL;
    }
    while (!self->prepared && !code)
    {
        code = pthread_cond_wait(&self->ready, &self->gate);
    }
    if (!code)
    {
        code = self->preparation_error;
    }
    self->proceed = code ? -1 : 1;
    pthread_cond_signal(&self->ready);
    pthread_mutex_unlock(&self->gate);
    if (code)
    {
        pthread_join(self->thread, NULL);
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
    if (!lh_os_concurrency_capture(pthread_join(self->thread, NULL)))
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
