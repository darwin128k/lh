/**
 * @file job.c
 * @brief Level-1 accessors of ::lh_os_worker_job_t and the bricks on them.
 */

#include <lh/os/worker/job.h>

#include <lh/assert/runtime.h>

lh_os_worker_job_t *
lh_os_worker_job_get_next(const lh_os_worker_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_void
lh_os_worker_job_set_next(lh_os_worker_job_t *self, lh_os_worker_job_t *next)
{
    lh_assert_runtime_ref(self);
    self->next = next;
}

lh_os_worker_cb
lh_os_worker_job_get_work_fn(const lh_os_worker_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->work;
}

lh_void
lh_os_worker_job_set_work_fn(lh_os_worker_job_t *self, lh_os_worker_cb work)
{
    lh_assert_runtime_ref(self);
    self->work = work;
}

lh_os_worker_cb
lh_os_worker_job_get_complete_fn(const lh_os_worker_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->complete;
}

lh_void
lh_os_worker_job_set_complete_fn(lh_os_worker_job_t *self, lh_os_worker_cb complete)
{
    lh_assert_runtime_ref(self);
    self->complete = complete;
}

lh_os_worker_cb
lh_os_worker_job_get_destroy_fn(const lh_os_worker_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->destroy;
}

lh_void
lh_os_worker_job_set_destroy_fn(lh_os_worker_job_t *self, lh_os_worker_cb destroy)
{
    lh_assert_runtime_ref(self);
    self->destroy = destroy;
}

lh_ptr
lh_os_worker_job_get_user(const lh_os_worker_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_os_worker_job_set_user(lh_os_worker_job_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

const lh_memory_allocator_t *
lh_os_worker_job_get_allocator(const lh_os_worker_job_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_memory_allocator_t *
lh_os_worker_job_get_allocator_mut(lh_os_worker_job_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_os_worker_job_t **
lh_os_worker_job_get_next_mut(lh_os_worker_job_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->next);
}

lh_void
lh_os_worker_job_run(lh_os_worker_job_t *self)
{
    lh_os_worker_job_get_work_fn(self)(lh_os_worker_job_get_user(self));
}

lh_void
lh_os_worker_job_complete(lh_os_worker_job_t *self)
{
    lh_os_worker_cb const complete = lh_os_worker_job_get_complete_fn(self);

    if (lh_null_ne(complete))
    {
        complete(lh_os_worker_job_get_user(self));
    }
}
