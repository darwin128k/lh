#include <lh/rail/mailbox.h>
#include <lh/core/assert/runtime.h>

lh_uint_t
lh_rail_mailbox_get_capacity(const lh_rail_mailbox_t *self)
{
    lh_assert_runtime_ref(self);
    return self->capacity;
}

lh_void
lh_rail_mailbox_set_capacity(lh_rail_mailbox_t *self, lh_uint_t capacity)
{
    lh_assert_runtime_ref(self);
    self->capacity = capacity;
}

const lh_atomic_uint_t *
lh_rail_mailbox_get_used(const lh_rail_mailbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->used);
}

lh_atomic_uint_t *
lh_rail_mailbox_get_used_mut(lh_rail_mailbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->used);
}

lh_rail_queue_t *
lh_rail_mailbox_get_finished_mut(lh_rail_mailbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->finished);
}

lh_uint_t
lh_rail_mailbox_get_used_count(const lh_rail_mailbox_t *self)
{
    return lh_atomic_uint_get_acquire(lh_rail_mailbox_get_used(self));
}

lh_void
lh_rail_mailbox_init(lh_rail_mailbox_t *self, lh_uint_t capacity)
{
    lh_rail_mailbox_set_capacity(self, capacity);
    lh_atomic_uint_set_relaxed(lh_rail_mailbox_get_used_mut(self), 0U);
    lh_rail_queue_init(lh_rail_mailbox_get_finished_mut(self));
}

lh_bool_t
lh_rail_mailbox_try_reserve(lh_rail_mailbox_t *self)
{
    lh_uint_t capacity = lh_rail_mailbox_get_capacity(self);
    lh_atomic_uint_t *used = lh_rail_mailbox_get_used_mut(self);
    for (;;)
    {
        lh_uint_t seen = lh_atomic_uint_get_acquire(used);
        if (lh_math_ge(seen, capacity))
        {
            return lh_bool_false;
        }
        /* Another submitter may have taken a place since the read: then read
           again, so the count never passes the capacity. */
        if (lh_atomic_uint_try_compare_exchange_acq_rel(used, seen, lh_math_add_one(seen)))
        {
            return lh_bool_true;
        }
    }
}

lh_void
lh_rail_mailbox_release(lh_rail_mailbox_t *self)
{
    lh_atomic_uint_fetch_sub_acq_rel(lh_rail_mailbox_get_used_mut(self), 1U);
}

lh_void
lh_rail_mailbox_post(lh_rail_mailbox_t *self, lh_rail_task_t *task)
{
    lh_rail_queue_push(lh_rail_mailbox_get_finished_mut(self), task);
}

lh_rail_task_t *
lh_rail_mailbox_try_take(lh_rail_mailbox_t *self)
{
    return lh_rail_queue_try_pop(lh_rail_mailbox_get_finished_mut(self));
}

lh_void
lh_rail_mailbox_deliver(lh_rail_mailbox_t *self, lh_rail_task_t *task)
{
    if (!lh_rail_task_is_cancelled(task))
    {
        lh_rail_task_run_complete(task);
    }
    lh_rail_task_run_destroy(task);
    /* Only now: until destroy ran, the task still held its place. */
    lh_rail_mailbox_release(self);
}

lh_uint_t
lh_rail_mailbox_dispatch(lh_rail_mailbox_t *self)
{
    lh_uint_t limit = lh_rail_mailbox_get_used_count(self);
    lh_uint_t count = 0U;
    while (lh_math_lt(count, limit))
    {
        lh_rail_task_t *task = lh_rail_mailbox_try_take(self);
        if (lh_null_eq(task))
        {
            break;
        }
        lh_rail_mailbox_deliver(self, task);
        count = lh_math_add_one(count);
    }
    return count;
}
