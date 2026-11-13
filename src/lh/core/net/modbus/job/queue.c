#include <lh/core/net/modbus/job/queue.h>

#include <lh/core/assert/runtime.h>

const lh_mb_job_t *
lh_mb_job_queue_get_jobs(const lh_mb_job_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->jobs;
}

lh_mb_job_t *
lh_mb_job_queue_get_jobs_mut(lh_mb_job_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->jobs;
}

lh_void
lh_mb_job_queue_set_jobs(lh_mb_job_queue_t *self, lh_mb_job_t *jobs)
{
    lh_assert_runtime_ref(self);
    self->jobs = jobs;
}

lh_u32_t
lh_mb_job_queue_get_capacity(const lh_mb_job_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->capacity;
}

lh_void
lh_mb_job_queue_set_capacity(lh_mb_job_queue_t *self, lh_u32_t capacity)
{
    lh_assert_runtime_ref(self);
    self->capacity = capacity;
}

lh_u32_t
lh_mb_job_queue_get_head(const lh_mb_job_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->head;
}

lh_void
lh_mb_job_queue_set_head(lh_mb_job_queue_t *self, lh_u32_t head)
{
    lh_assert_runtime_ref(self);
    self->head = head;
}

lh_u32_t
lh_mb_job_queue_get_count(const lh_mb_job_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_mb_job_queue_set_count(lh_mb_job_queue_t *self, lh_u32_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_void
lh_mb_job_queue_init(lh_mb_job_queue_t *self, lh_mb_job_t *jobs, lh_u32_t capacity)
{
    lh_assert_runtime_ref(jobs);
    lh_mb_job_queue_set_jobs(self, jobs);
    lh_mb_job_queue_set_capacity(self, capacity);
    lh_mb_job_queue_set_head(self, 0);
    lh_mb_job_queue_set_count(self, 0);
}

lh_bool_t
lh_mb_job_queue_is_empty(const lh_mb_job_queue_t *self)
{
    return lh_cast_static(lh_bool_t, lh_mb_job_queue_get_count(self) == 0);
}

lh_bool_t
lh_mb_job_queue_is_full(const lh_mb_job_queue_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_mb_job_queue_get_count(self) >= lh_mb_job_queue_get_capacity(self));
}

const lh_mb_job_t *
lh_mb_job_queue_get_job_at(const lh_mb_job_queue_t *self, lh_u32_t index)
{
    return lh_mb_job_queue_get_jobs(self) +
           (lh_mb_job_queue_get_head(self) + index) % lh_mb_job_queue_get_capacity(self);
}

lh_mb_job_t *
lh_mb_job_queue_get_job_at_mut(lh_mb_job_queue_t *self, lh_u32_t index)
{
    return lh_mb_job_queue_get_jobs_mut(self) +
           (lh_mb_job_queue_get_head(self) + index) % lh_mb_job_queue_get_capacity(self);
}

lh_bool_t
lh_mb_job_queue_try_push(lh_mb_job_queue_t *self, const lh_mb_job_t *job)
{
    lh_assert_runtime_ref(job);
    if (lh_mb_job_queue_is_full(self))
    {
        return lh_bool_false;
    }
    const lh_u32_t count = lh_mb_job_queue_get_count(self);

    lh_mb_job_queue_set_count(self, count + 1U);
    lh_ptr_deref(lh_mb_job_queue_get_job_at_mut(self, count)) = lh_ptr_deref(job);
    return lh_bool_true;
}

lh_bool_t
lh_mb_job_queue_try_pop(lh_mb_job_queue_t *self, lh_mb_job_t *job)
{
    lh_assert_runtime_ref(job);
    if (lh_mb_job_queue_is_empty(self))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(job) = lh_ptr_deref(lh_mb_job_queue_get_job_at(self, 0));
    lh_mb_job_queue_set_head(self, (lh_mb_job_queue_get_head(self) + 1U) %
                                       lh_mb_job_queue_get_capacity(self));
    lh_mb_job_queue_set_count(self, lh_mb_job_queue_get_count(self) - 1U);
    return lh_bool_true;
}

lh_u32_t
lh_mb_job_queue_discard_owner(lh_mb_job_queue_t *self, lh_u32_t owner)
{
    const lh_u32_t count = lh_mb_job_queue_get_count(self);
    lh_u32_t kept = 0;

    /* Slide the jobs that stay towards the head, in order, over the ones that go. */
    for (lh_u32_t i = 0; i < count; ++i)
    {
        if (lh_mb_job_get_owner(lh_mb_job_queue_get_job_at(self, i)) == owner)
        {
            continue;
        }
        if (kept != i)
        {
            lh_ptr_deref(lh_mb_job_queue_get_job_at_mut(self, kept)) =
                lh_ptr_deref(lh_mb_job_queue_get_job_at(self, i));
        }
        kept += 1U;
    }
    lh_mb_job_queue_set_count(self, kept);
    return count - kept;
}
