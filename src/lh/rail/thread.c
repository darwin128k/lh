#include <lh/rail/thread.h>
#include <lh/core/assert/runtime.h>
#include <lh/rail/base/wait/forever.h>
#include <lh/rail/runtime/base.h>
#include <lh/core/runtime/terminate.h>

const lh_rail_executor_t *
lh_rail_thread_get_executor(const lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->executor);
}

lh_rail_executor_t *
lh_rail_thread_get_executor_mut(lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->executor);
}

lh_rail_queue_t *
lh_rail_thread_get_pending_mut(lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pending);
}

const lh_rail_context_t *
lh_rail_thread_get_context(const lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->context);
}

lh_rail_context_t *
lh_rail_thread_get_context_mut(lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->context);
}

lh_ptr
lh_rail_thread_get_local(const lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->local;
}

lh_void
lh_rail_thread_set_local(lh_rail_thread_t *self, lh_ptr local)
{
    lh_assert_runtime_ref(self);
    self->local = local;
}

lh_rail_stop_kind_t
lh_rail_thread_get_stop(const lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stop;
}

lh_void
lh_rail_thread_set_stop(lh_rail_thread_t *self, lh_rail_stop_kind_t stop)
{
    lh_assert_runtime_ref(self);
    self->stop = stop;
}

lh_bool_t
lh_rail_thread_is_ended(const lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ended;
}

lh_void
lh_rail_thread_set_ended(lh_rail_thread_t *self, lh_bool_t ended)
{
    lh_assert_runtime_ref(self);
    self->ended = ended;
}

lh_ptr
lh_rail_thread_get_lock(const lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lock;
}

lh_void
lh_rail_thread_set_lock(lh_rail_thread_t *self, lh_ptr lock)
{
    lh_assert_runtime_ref(self);
    self->lock = lock;
}

lh_ptr
lh_rail_thread_get_wake(const lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wake;
}

lh_void
lh_rail_thread_set_wake(lh_rail_thread_t *self, lh_ptr wake)
{
    lh_assert_runtime_ref(self);
    self->wake = wake;
}

lh_ptr
lh_rail_thread_get_end(const lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->end;
}

lh_void
lh_rail_thread_set_end(lh_rail_thread_t *self, lh_ptr end)
{
    lh_assert_runtime_ref(self);
    self->end = end;
}

lh_ptr
lh_rail_thread_get_thread(const lh_rail_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thread;
}

lh_void
lh_rail_thread_set_thread(lh_rail_thread_t *self, lh_ptr thread)
{
    lh_assert_runtime_ref(self);
    self->thread = thread;
}

lh_void
lh_rail_thread_broadcast(lh_ptr signal)
{
    lh_runtime_terminate_unless(lh_rail_base_signal_wake(lh_rail_runtime_base_get(), signal));
}

lh_void
lh_rail_thread_lock(lh_rail_thread_t *self)
{
    lh_runtime_terminate_unless(
        lh_rail_base_lock(lh_rail_runtime_base_get(), lh_rail_thread_get_lock(self)));
}

lh_void
lh_rail_thread_unlock(lh_rail_thread_t *self)
{
    lh_runtime_terminate_unless(
        lh_rail_base_unlock(lh_rail_runtime_base_get(), lh_rail_thread_get_lock(self)));
}

lh_void
lh_rail_thread_wait(lh_rail_thread_t *self, lh_ptr signal, lh_u32_t timeout_ms)
{
    lh_runtime_terminate_unless(lh_rail_base_signal_wait(
        lh_rail_runtime_base_get(), signal, lh_rail_thread_get_lock(self), timeout_ms));
}

lh_void
lh_rail_thread_release(lh_rail_thread_t *self)
{
    const lh_rail_base_t *base = lh_rail_runtime_base_get();
    if (lh_ptr_is_set(lh_rail_thread_get_end(self)))
    {
        lh_runtime_terminate_unless(lh_rail_base_signal_close(base, lh_rail_thread_get_end(self)));
        lh_rail_thread_set_end(self, lh_null);
    }
    if (lh_ptr_is_set(lh_rail_thread_get_wake(self)))
    {
        lh_runtime_terminate_unless(lh_rail_base_signal_close(base, lh_rail_thread_get_wake(self)));
        lh_rail_thread_set_wake(self, lh_null);
    }
    if (lh_ptr_is_set(lh_rail_thread_get_lock(self)))
    {
        lh_runtime_terminate_unless(lh_rail_base_lock_close(base, lh_rail_thread_get_lock(self)));
        lh_rail_thread_set_lock(self, lh_null);
    }
}

lh_void
lh_rail_thread_run_task(lh_rail_thread_t *self, lh_rail_task_t *task)
{
    if (!lh_rail_task_is_cancelled(task))
    {
        lh_rail_task_run_work(task, lh_rail_thread_get_local(self));
    }
    /* After the post the owner may dispatch and free the task: not touched again. */
    lh_rail_mailbox_post(lh_rail_task_get_mailbox_mut(task), task);
}

lh_bool_t
lh_rail_thread_try_accept(lh_ptr context, lh_rail_task_t *task)
{
    lh_rail_thread_t *self = lh_ptr_rcast(lh_rail_thread_t, context);
    lh_rail_thread_lock(self);
    if (lh_math_ne(lh_rail_thread_get_stop(self), lh_rail_stop_kind_none))
    {
        lh_rail_thread_unlock(self);
        return lh_bool_false;
    }
    /* Linked under the mutex: the thread, which pops under it too, sees either
       the whole task or none of it, and a stop cannot slip in between. */
    lh_rail_queue_push(lh_rail_thread_get_pending_mut(self), task);
    lh_rail_thread_broadcast(lh_rail_thread_get_wake(self));
    lh_rail_thread_unlock(self);
    return lh_bool_true;
}

lh_rail_task_t *
lh_rail_thread_wait_task(lh_rail_thread_t *self)
{
    lh_rail_thread_lock(self);
    lh_rail_task_t *task = lh_rail_queue_try_pop(lh_rail_thread_get_pending_mut(self));
    while (lh_null_eq(task) && lh_math_eq(lh_rail_thread_get_stop(self), lh_rail_stop_kind_none))
    {
        lh_rail_thread_wait(self, lh_rail_thread_get_wake(self), lh_rail_base_wait_forever);
        task = lh_rail_queue_try_pop(lh_rail_thread_get_pending_mut(self));
    }
    if (lh_ptr_is_set(task) && lh_math_eq(lh_rail_thread_get_stop(self), lh_rail_stop_kind_cancel))
    {
        lh_rail_task_set_cancelled(task, lh_bool_true);
    }
    lh_rail_thread_unlock(self);
    return task;
}

lh_bool_t
lh_rail_thread_try_start(lh_rail_thread_t *self, const lh_rail_context_t *context)
{
    const lh_rail_base_t *base = lh_rail_runtime_base_get();
    if (!lh_rail_base_has_clock(base) || !lh_rail_base_has_lock(base) ||
        !lh_rail_base_has_signal(base) || !lh_rail_base_has_thread(base))
    {
        return lh_bool_false;
    }
    lh_rail_executor_init(lh_rail_thread_get_executor_mut(self), lh_rail_thread_try_accept, self);
    lh_rail_queue_init(lh_rail_thread_get_pending_mut(self));
    lh_rail_context_assign(lh_rail_thread_get_context_mut(self), context);
    lh_rail_thread_set_local(self, lh_null);
    lh_rail_thread_set_stop(self, lh_rail_stop_kind_none);
    lh_rail_thread_set_ended(self, lh_bool_false);
    lh_rail_thread_set_lock(self, lh_rail_base_lock_open(base));
    lh_rail_thread_set_wake(self, lh_rail_base_signal_open(base));
    lh_rail_thread_set_end(self, lh_rail_base_signal_open(base));
    lh_rail_thread_set_thread(self, lh_null);
    if (lh_ptr_is_set(lh_rail_thread_get_lock(self)) &&
        lh_ptr_is_set(lh_rail_thread_get_wake(self)) && lh_ptr_is_set(lh_rail_thread_get_end(self)))
    {
        lh_rail_thread_set_thread(
            self, lh_rail_base_thread_start(base, lh_rail_thread_run, self, -1, 0U));
    }
    if (lh_null_eq(lh_rail_thread_get_thread(self)))
    {
        /* What was never made is skipped. */
        lh_rail_thread_release(self);
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_rail_thread_try_stop(lh_rail_thread_t *self, lh_rail_stop_kind_t kind, lh_u32_t timeout_ms)
{
    const lh_rail_base_t *base = lh_rail_runtime_base_get();
    lh_u64_t start = lh_rail_base_now_ms(base);
    lh_rail_thread_lock(self);
    /* none < drain < cancel: a stop only ever gets stricter. */
    if (lh_math_lt(lh_rail_thread_get_stop(self), kind))
    {
        lh_rail_thread_set_stop(self, kind);
        lh_rail_thread_broadcast(lh_rail_thread_get_wake(self));
    }
    while (!lh_rail_thread_is_ended(self))
    {
        lh_u64_t elapsed = lh_math_sub(lh_rail_base_now_ms(base), start);
        if (lh_math_ge(elapsed, timeout_ms))
        {
            break;
        }
        lh_rail_thread_wait(self, lh_rail_thread_get_end(self),
                            lh_cast_static(lh_u32_t, lh_math_sub(timeout_ms, elapsed)));
    }
    lh_bool_t ended = lh_rail_thread_is_ended(self);
    lh_rail_thread_unlock(self);
    return ended;
}

lh_void
lh_rail_thread_destroy(lh_rail_thread_t *self)
{
    lh_rail_thread_lock(self);
    if (lh_math_eq(lh_rail_thread_get_stop(self), lh_rail_stop_kind_none))
    {
        lh_rail_thread_set_stop(self, lh_rail_stop_kind_drain);
        lh_rail_thread_broadcast(lh_rail_thread_get_wake(self));
    }
    while (!lh_rail_thread_is_ended(self))
    {
        lh_rail_thread_wait(self, lh_rail_thread_get_end(self), lh_rail_base_wait_forever);
    }
    lh_rail_thread_unlock(self);
    lh_runtime_terminate_unless(
        lh_rail_base_thread_join(lh_rail_runtime_base_get(), lh_rail_thread_get_thread(self)));
    lh_rail_thread_set_thread(self, lh_null);
    lh_rail_thread_release(self);
}

lh_void
lh_rail_thread_run(lh_ptr user)
{
    lh_rail_thread_t *self = lh_ptr_rcast(lh_rail_thread_t, user);
    lh_rail_thread_set_local(self, lh_rail_context_run_open(lh_rail_thread_get_context(self)));
    for (lh_rail_task_t *task = lh_rail_thread_wait_task(self); lh_ptr_is_set(task);
         task = lh_rail_thread_wait_task(self))
    {
        lh_rail_thread_run_task(self, task);
    }
    lh_rail_context_run_close(lh_rail_thread_get_context(self), lh_rail_thread_get_local(self));
    lh_rail_thread_lock(self);
    lh_rail_thread_set_ended(self, lh_bool_true);
    lh_rail_thread_broadcast(lh_rail_thread_get_end(self));
    lh_rail_thread_unlock(self);
}
