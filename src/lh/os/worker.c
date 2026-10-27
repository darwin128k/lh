#include <lh/os/worker.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/terminate.h>
#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/os/thread.h>
#include <lh/os/mutex.h>
#include <lh/os/condition.h>

/**
 * @brief Stop on a broken synchronization invariant rather than release live callback state.
 */
static lh_void
lh_os_worker_require(lh_bool_t ok)
{
    /* Continuing after an invalid lock/join could free live callback state. */
    if (!ok)
    {
        lh_runtime_terminate();
    }
}

struct lh_os_worker
{
    lh_memory_allocator_t allocator;
    lh_os_worker_job_t *pending, **pending_tail, *done, **done_tail;
    lh_uint_t count, capacity;
    lh_bool_t stop;
    lh_os_thread_t thread;
    lh_os_mutex_t mutex;
    lh_os_condition_t wake;
};

/**
 * @brief Acquire the worker queue mutex through the LH OS layer.
 */
static lh_void
lh_os_worker_lock(lh_os_worker_t *w)
{
    lh_os_worker_require(lh_os_mutex_lock(&w->mutex));
}

/**
 * @brief Release the worker queue mutex through the LH OS layer.
 */
static lh_void
lh_os_worker_unlock(lh_os_worker_t *w)
{
    lh_os_worker_require(lh_os_mutex_unlock(&w->mutex));
}

/**
 * @brief Notify the worker after its queue or shutdown predicate changes.
 */
static lh_void
lh_os_worker_wake(lh_os_worker_t *w)
{
    lh_os_worker_require(lh_os_condition_signal(&w->wake));
}

/**
 * @brief Execute pending work and queue completions for the caller.
 */
static lh_void
lh_os_worker_run(lh_ptr arg)
{
    lh_os_worker_t *w = arg;
    for (;;)
    {
        lh_os_worker_job_t *job;
        lh_os_worker_lock(w);
        while (!w->stop && !w->pending)
        {
            lh_os_worker_require(lh_os_condition_wait(&w->wake, &w->mutex));
        }
        if (w->stop)
        {
            lh_os_worker_unlock(w);
            break;
        }
        job = w->pending;
        w->pending = job->next;
        if (!w->pending)
        {
            w->pending_tail = &w->pending;
        }
        lh_os_worker_unlock(w);
        job->work(job->user);
        job->next = lh_null;
        lh_os_worker_lock(w);
        *w->done_tail = job;
        w->done_tail = &job->next;
        lh_os_worker_unlock(w);
    }
}

/**
 * @brief Create a bounded worker with initialized synchronization objects.
 */
lh_os_worker_t *
lh_os_worker_create(lh_uint_t capacity)
{
    lh_memory_allocator_t allocator = *lh_runtime_allocator();
    lh_os_worker_t *w;
    if (!capacity || !allocator.alloc_cb || !allocator.dealloc_cb)
    {
        return lh_null;
    }
    w = allocator.alloc_cb(allocator.context, sizeof *w);
    if (!w)
    {
        return lh_null;
    }
    lh_memory_std_set(w, 0, sizeof *w);
    w->allocator = allocator;
    w->capacity = capacity;
    w->pending_tail = &w->pending;
    w->done_tail = &w->done;
    lh_os_thread_init(&w->thread);
    lh_os_mutex_init(&w->mutex);
    lh_os_condition_init(&w->wake);
    if (!lh_os_mutex_open(&w->mutex) || !lh_os_condition_open(&w->wake) ||
        !lh_os_thread_start(&w->thread, lh_os_worker_run, w, -1, 0))
    {
        lh_os_worker_require(lh_os_condition_close(&w->wake));
        lh_os_worker_require(lh_os_mutex_close(&w->mutex));
        lh_memory_allocator_dealloc(&w->allocator, w);
        return lh_null;
    }
    return w;
}

/**
 * @brief Enqueue a job within the configured outstanding-job limit.
 */
lh_bool_t
lh_os_worker_submit(lh_os_worker_t *w, lh_os_worker_cb work, lh_os_worker_cb complete,
                    lh_os_worker_cb destroy, lh_ptr user)
{
    lh_os_worker_job_t *job;
    if (!w || !work)
    {
        return lh_bool_false;
    }
    job = w->allocator.alloc_cb(w->allocator.context, sizeof *job);
    if (!job)
    {
        return lh_bool_false;
    }
    lh_memory_std_set(job, 0, sizeof *job);
    job->allocator = w->allocator;
    job->work = work;
    job->complete = complete;
    job->destroy = destroy;
    job->user = user;
    lh_os_worker_lock(w);
    if (w->stop || w->count >= w->capacity)
    {
        lh_os_worker_unlock(w);
        lh_memory_allocator_dealloc(&job->allocator, job);
        return lh_bool_false;
    }
    ++w->count;
    *w->pending_tail = job;
    w->pending_tail = &job->next;
    lh_os_worker_wake(w);
    lh_os_worker_unlock(w);
    return lh_bool_true;
}

/**
 * @brief Run the payload destructor once and release its job record.
 */
lh_void
lh_os_worker_dispose(lh_os_worker_job_t *job)
{
    lh_assert_runtime_ref(job);
    if (job->destroy)
    {
        job->destroy(job->user);
    }
    lh_memory_allocator_dealloc(&job->allocator, job);
}

lh_os_worker_job_t *
lh_os_worker_take(lh_os_worker_t *w)
{
    lh_assert_runtime_ref(w);
    lh_os_worker_job_t *job;
    lh_os_worker_lock(w);
    job = w->done;
    if (job)
    {
        w->done = job->next;
        if (!w->done)
        {
            w->done_tail = &w->done;
        }
        --w->count;
        job->next = lh_null;
    }
    lh_os_worker_unlock(w);
    return job;
}

lh_void
lh_os_worker_stop(lh_os_worker_t *w)
{
    lh_assert_runtime_ref(w);
    lh_os_worker_lock(w);
    w->stop = lh_bool_true;
    lh_os_worker_wake(w);
    lh_os_worker_unlock(w);
    lh_os_worker_require(lh_os_thread_join(&w->thread));
}

lh_void
lh_os_worker_destroy(lh_os_worker_t *w)
{
    lh_os_worker_job_t *job;
    if (!w)
    {
        return;
    }
    lh_os_worker_stop(w);
    while ((job = w->pending))
    {
        w->pending = job->next;
        lh_os_worker_dispose(job);
    }
    while ((job = w->done))
    {
        w->done = job->next;
        lh_os_worker_dispose(job);
    }
    lh_os_worker_require(lh_os_condition_close(&w->wake));
    lh_os_worker_require(lh_os_mutex_close(&w->mutex));
    lh_memory_allocator_dealloc(&w->allocator, w);
}
