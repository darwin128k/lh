#include <lh/rail/queue.h>
#include <lh/core/assert/runtime.h>

lh_atomic_ptr_t *
lh_rail_queue_get_back_mut(lh_rail_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->back);
}

lh_rail_task_t *
lh_rail_queue_get_front(const lh_rail_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->front;
}

lh_void
lh_rail_queue_set_front(lh_rail_queue_t *self, lh_rail_task_t *front)
{
    lh_assert_runtime_ref(self);
    self->front = front;
}

lh_rail_task_t *
lh_rail_queue_get_stub_mut(lh_rail_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->stub);
}

lh_void
lh_rail_queue_init(lh_rail_queue_t *self)
{
    lh_rail_task_t *stub = lh_rail_queue_get_stub_mut(self);
    lh_rail_task_init(stub, lh_null, lh_null, lh_null, lh_null);
    lh_atomic_ptr_set_relaxed(lh_rail_queue_get_back_mut(self), stub);
    lh_rail_queue_set_front(self, stub);
}

lh_void
lh_rail_queue_push(lh_rail_queue_t *self, lh_rail_task_t *task)
{
    lh_atomic_ptr_set_relaxed(lh_rail_task_get_next_mut(task), lh_null);
    /* Claim the back; the task that was there links to this one. Between the
       two steps the consumer cannot see this task yet. */
    lh_rail_task_t *previous = lh_cast_static(
        lh_rail_task_t *, lh_atomic_ptr_exchange_acq_rel(lh_rail_queue_get_back_mut(self), task));
    lh_atomic_ptr_set_release(lh_rail_task_get_next_mut(previous), task);
}

lh_rail_task_t *
lh_rail_queue_try_pop(lh_rail_queue_t *self)
{
    lh_rail_task_t *stub = lh_rail_queue_get_stub_mut(self);
    lh_rail_task_t *front = lh_rail_queue_get_front(self);
    lh_rail_task_t *next = lh_rail_task_get_next_task(front);
    if (front == stub)
    {
        if (lh_null_eq(next))
        {
            return lh_null; /* empty */
        }
        /* Step over the stub: it is never handed out. */
        lh_rail_queue_set_front(self, next);
        front = next;
        next = lh_rail_task_get_next_task(front);
    }
    if (lh_ptr_is_set(next))
    {
        lh_rail_queue_set_front(self, next);
        return front;
    }
    /* front is the last linked task. If it is not the back, a producer is
       between its exchange and its link: look again later. */
    if (front != lh_atomic_ptr_get_acquire(lh_rail_queue_get_back_mut(self)))
    {
        return lh_null;
    }
    /* Put the stub behind the last task, so it can be handed out. */
    lh_rail_queue_push(self, stub);
    next = lh_rail_task_get_next_task(front);
    if (lh_ptr_is_set(next))
    {
        lh_rail_queue_set_front(self, next);
        return front;
    }
    return lh_null;
}
