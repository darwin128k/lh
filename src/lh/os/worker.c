/**
 * @file worker.c
 * @brief ::lh_os_worker_t: a bounded queue of jobs run by a loop, results taken by the caller.
 *
 * A submit links the job under the mutex and wakes the loop; its step runs the
 * waiting jobs until none is left or the stop flag is up. A stop sets the flag and
 * destroys the loop, whose last step therefore runs nothing more.
 */

#include <lh/os/worker.h>

#include <lh/assert/runtime.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/terminate.h>

/* The worker is opaque: its fields are reached through the level-1 accessors
   declared in lh/os/worker.h. */
struct lh_os_worker
{
    lh_memory_allocator_t allocator;
    lh_os_worker_job_t *pending;
    lh_os_worker_job_t **pending_tail;
    lh_os_worker_job_t *done;
    lh_os_worker_job_t **done_tail;
    lh_uint_t count;
    lh_uint_t capacity;
    lh_bool_t stop;
    lh_os_loop_t *loop;
    lh_os_mutex_t mutex;
};

/* The loop sleeps between wakes; a wake is never lost, so the period only bounds
   how long an idle worker sleeps at a time. */
#define LH_OS_WORKER_PERIOD_MS 60000U

lh_os_worker_job_t *
lh_os_worker_get_pending(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pending;
}

lh_void
lh_os_worker_set_pending(lh_os_worker_t *self, lh_os_worker_job_t *pending)
{
    lh_assert_runtime_ref(self);
    self->pending = pending;
}

lh_os_worker_job_t **
lh_os_worker_get_pending_tail(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pending_tail;
}

lh_void
lh_os_worker_set_pending_tail(lh_os_worker_t *self, lh_os_worker_job_t **pending_tail)
{
    lh_assert_runtime_ref(self);
    self->pending_tail = pending_tail;
}

lh_os_worker_job_t *
lh_os_worker_get_done(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->done;
}

lh_void
lh_os_worker_set_done(lh_os_worker_t *self, lh_os_worker_job_t *done)
{
    lh_assert_runtime_ref(self);
    self->done = done;
}

lh_os_worker_job_t **
lh_os_worker_get_done_tail(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->done_tail;
}

lh_void
lh_os_worker_set_done_tail(lh_os_worker_t *self, lh_os_worker_job_t **done_tail)
{
    lh_assert_runtime_ref(self);
    self->done_tail = done_tail;
}

lh_uint_t
lh_os_worker_get_count(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_os_worker_set_count(lh_os_worker_t *self, lh_uint_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_uint_t
lh_os_worker_get_capacity(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->capacity;
}

lh_void
lh_os_worker_set_capacity(lh_os_worker_t *self, lh_uint_t capacity)
{
    lh_assert_runtime_ref(self);
    self->capacity = capacity;
}

lh_bool_t
lh_os_worker_is_stop(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stop;
}

lh_void
lh_os_worker_set_stop(lh_os_worker_t *self, lh_bool_t stop)
{
    lh_assert_runtime_ref(self);
    self->stop = stop;
}

const lh_memory_allocator_t *
lh_os_worker_get_allocator(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_memory_allocator_t *
lh_os_worker_get_allocator_mut(lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_os_loop_t *
lh_os_worker_get_loop(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->loop;
}

lh_void
lh_os_worker_set_loop(lh_os_worker_t *self, lh_os_loop_t *loop)
{
    lh_assert_runtime_ref(self);
    self->loop = loop;
}

const lh_os_mutex_t *
lh_os_worker_get_mutex(const lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->mutex);
}

lh_os_mutex_t *
lh_os_worker_get_mutex_mut(lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->mutex);
}

lh_os_worker_job_t **
lh_os_worker_get_pending_mut(lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pending);
}

lh_os_worker_job_t **
lh_os_worker_get_done_mut(lh_os_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->done);
}

/**
 * @brief Run the payload destructor once and release its job record.
 */
lh_void
lh_os_worker_dispose(lh_os_worker_job_t *job)
{
    lh_os_worker_cb const destroy = lh_os_worker_job_get_destroy_fn(job);

    if (lh_null_ne(destroy))
    {
        destroy(lh_os_worker_job_get_user(job));
    }
    lh_memory_allocator_dealloc(lh_os_worker_job_get_allocator_mut(job), job);
}

lh_void
lh_os_worker_lock(lh_os_worker_t *self)
{
    lh_runtime_terminate_unless(lh_os_mutex_lock(lh_os_worker_get_mutex_mut(self)));
}

lh_void
lh_os_worker_unlock(lh_os_worker_t *self)
{
    lh_runtime_terminate_unless(lh_os_mutex_unlock(lh_os_worker_get_mutex_mut(self)));
}

lh_void
lh_os_worker_push_pending(lh_os_worker_t *self, lh_os_worker_job_t *job)
{
    lh_ptr_deref(lh_os_worker_get_pending_tail(self)) = job;
    lh_os_worker_set_pending_tail(self, lh_os_worker_job_get_next_mut(job));
}

lh_os_worker_job_t *
lh_os_worker_pop_pending(lh_os_worker_t *self)
{
    lh_os_worker_job_t *const job = lh_os_worker_get_pending(self);

    if (lh_null_ne(job))
    {
        lh_os_worker_set_pending(self, lh_os_worker_job_get_next(job));
        if (lh_null_eq(lh_os_worker_get_pending(self)))
        {
            lh_os_worker_set_pending_tail(self, lh_os_worker_get_pending_mut(self));
        }
        lh_os_worker_job_set_next(job, lh_null);
    }
    return job;
}

lh_void
lh_os_worker_push_done(lh_os_worker_t *self, lh_os_worker_job_t *job)
{
    lh_ptr_deref(lh_os_worker_get_done_tail(self)) = job;
    lh_os_worker_set_done_tail(self, lh_os_worker_job_get_next_mut(job));
}

lh_os_worker_job_t *
lh_os_worker_pop_done(lh_os_worker_t *self)
{
    lh_os_worker_job_t *const job = lh_os_worker_get_done(self);

    if (lh_null_ne(job))
    {
        lh_os_worker_set_done(self, lh_os_worker_job_get_next(job));
        if (lh_null_eq(lh_os_worker_get_done(self)))
        {
            lh_os_worker_set_done_tail(self, lh_os_worker_get_done_mut(self));
        }
        lh_os_worker_job_set_next(job, lh_null);
    }
    return job;
}

/**
 * @brief Create a bounded worker with initialized synchronization objects.
 */
lh_os_worker_t *
lh_os_worker_create(lh_uint_t capacity)
{
    lh_memory_allocator_t allocator = lh_ptr_deref(lh_runtime_allocator());
    lh_os_worker_t *self;

    if (capacity == 0U || lh_null_eq(lh_memory_allocator_get_dealloc_cb(lh_addr_of(allocator))))
    {
        return lh_null;
    }
    self = lh_ptr_rcast(lh_os_worker_t, lh_memory_allocator_try_alloc_zero(
                                            lh_addr_of(allocator), 1U, sizeof(lh_ptr_deref(self))));
    if (lh_null_eq(self))
    {
        return lh_null;
    }
    lh_memory_allocator_assign(lh_os_worker_get_allocator_mut(self), lh_addr_of(allocator));
    lh_os_worker_set_capacity(self, capacity);
    lh_os_worker_set_pending_tail(self, lh_os_worker_get_pending_mut(self));
    lh_os_worker_set_done_tail(self, lh_os_worker_get_done_mut(self));
    lh_os_mutex_init(lh_os_worker_get_mutex_mut(self));
    lh_os_worker_set_loop(self,
                          lh_os_loop_try_create(LH_OS_WORKER_PERIOD_MS, lh_os_worker_step, self));
    if (!lh_os_mutex_open(lh_os_worker_get_mutex_mut(self)) ||
        lh_null_eq(lh_os_worker_get_loop(self)) ||
        !lh_os_loop_try_start(lh_os_worker_get_loop(self)))
    {
        lh_os_loop_destroy(lh_os_worker_get_loop(self));
        lh_runtime_terminate_unless(lh_os_mutex_close(lh_os_worker_get_mutex_mut(self)));
        lh_memory_allocator_dealloc(lh_os_worker_get_allocator_mut(self), self);
        return lh_null;
    }
    return self;
}

lh_void
lh_os_worker_step(lh_ptr arg)
{
    lh_os_worker_t *const self = lh_ptr_rcast(lh_os_worker_t, arg);

    for (;;)
    {
        lh_os_worker_job_t *job;

        lh_os_worker_lock(self);
        if (lh_os_worker_is_stop(self) || lh_null_eq(lh_os_worker_get_pending(self)))
        {
            lh_os_worker_unlock(self);
            return;
        }
        job = lh_os_worker_pop_pending(self);
        lh_os_worker_unlock(self);
        lh_os_worker_job_run(job);
        lh_os_worker_lock(self);
        lh_os_worker_push_done(self, job);
        lh_os_worker_unlock(self);
    }
}

/**
 * @brief Enqueue a job within the configured outstanding-job limit.
 */
lh_bool_t
lh_os_worker_submit(lh_os_worker_t *self, lh_os_worker_cb work, lh_os_worker_cb complete,
                    lh_os_worker_cb destroy, lh_ptr user)
{
    lh_os_worker_job_t *job;

    if (lh_null_eq(self) || lh_null_eq(work))
    {
        return lh_bool_false;
    }
    job = lh_ptr_rcast(lh_os_worker_job_t,
                       lh_memory_allocator_try_alloc_zero(lh_os_worker_get_allocator_mut(self), 1U,
                                                          sizeof(lh_ptr_deref(job))));
    if (lh_null_eq(job))
    {
        return lh_bool_false;
    }
    lh_memory_allocator_assign(lh_os_worker_job_get_allocator_mut(job),
                               lh_os_worker_get_allocator(self));
    lh_os_worker_job_set_work_fn(job, work);
    lh_os_worker_job_set_complete_fn(job, complete);
    lh_os_worker_job_set_destroy_fn(job, destroy);
    lh_os_worker_job_set_user(job, user);
    lh_os_worker_lock(self);
    if (lh_os_worker_is_stop(self) ||
        lh_os_worker_get_count(self) >= lh_os_worker_get_capacity(self))
    {
        lh_os_worker_unlock(self);
        lh_memory_allocator_dealloc(lh_os_worker_job_get_allocator_mut(job), job);
        return lh_bool_false;
    }
    lh_os_worker_set_count(self, lh_os_worker_get_count(self) + 1U);
    lh_os_worker_push_pending(self, job);
    lh_os_worker_unlock(self);
    lh_os_loop_wake(lh_os_worker_get_loop(self));
    return lh_bool_true;
}

lh_os_worker_job_t *
lh_os_worker_take(lh_os_worker_t *self)
{
    lh_os_worker_lock(self);
    lh_os_worker_job_t *job = lh_os_worker_pop_done(self);

    if (lh_null_ne(job))
    {
        lh_os_worker_set_count(self, lh_os_worker_get_count(self) - 1U);
    }
    lh_os_worker_unlock(self);
    return job;
}

lh_void
lh_os_worker_stop(lh_os_worker_t *self)
{
    lh_os_worker_lock(self);
    lh_os_worker_set_stop(self, lh_bool_true);
    lh_os_worker_unlock(self);
    /* The loop's last step sees the flag and runs nothing; a stop again is a no-op. */
    lh_os_loop_destroy(lh_os_worker_get_loop(self));
    lh_os_worker_set_loop(self, lh_null);
}

lh_void
lh_os_worker_destroy(lh_os_worker_t *self)
{
    lh_os_worker_job_t *job;

    if (lh_null_eq(self))
    {
        return;
    }
    lh_os_worker_stop(self);
    while (lh_null_ne(job = lh_os_worker_pop_pending(self)))
    {
        lh_os_worker_dispose(job);
    }
    while (lh_null_ne(job = lh_os_worker_pop_done(self)))
    {
        lh_os_worker_dispose(job);
    }
    lh_runtime_terminate_unless(lh_os_mutex_close(lh_os_worker_get_mutex_mut(self)));
    lh_memory_allocator_dealloc(lh_os_worker_get_allocator_mut(self), self);
}
