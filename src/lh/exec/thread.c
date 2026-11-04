#include <lh/exec/thread.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/runtime/terminate.h>
#include <lh/os/tick.h>

const lh_exec_executor_t *
lh_exec_thread_get_executor(const lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->executor);
}

lh_exec_executor_t *
lh_exec_thread_get_executor_mut(lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->executor);
}

lh_exec_queue_t *
lh_exec_thread_get_pending_mut(lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pending);
}

const lh_exec_context_t *
lh_exec_thread_get_context(const lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->context);
}

lh_exec_context_t *
lh_exec_thread_get_context_mut(lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->context);
}

lh_ptr
lh_exec_thread_get_local(const lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->local;
}

lh_void
lh_exec_thread_set_local(lh_exec_thread_t *self, lh_ptr local)
{
    lh_assert_runtime_ref(self);
    self->local = local;
}

lh_exec_stop_kind_t
lh_exec_thread_get_stop(const lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stop;
}

lh_void
lh_exec_thread_set_stop(lh_exec_thread_t *self, lh_exec_stop_kind_t stop)
{
    lh_assert_runtime_ref(self);
    self->stop = stop;
}

lh_bool_t
lh_exec_thread_is_ended(const lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ended;
}

lh_void
lh_exec_thread_set_ended(lh_exec_thread_t *self, lh_bool_t ended)
{
    lh_assert_runtime_ref(self);
    self->ended = ended;
}

lh_os_mutex_t *
lh_exec_thread_get_mutex_mut(lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->mutex);
}

lh_os_condition_t *
lh_exec_thread_get_wake_mut(lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->wake);
}

lh_os_condition_t *
lh_exec_thread_get_end_mut(lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->end);
}

lh_os_thread_t *
lh_exec_thread_get_thread_mut(lh_exec_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->thread);
}

lh_void
lh_exec_thread_broadcast(lh_os_condition_t *condition)
{
    lh_runtime_terminate_unless(lh_os_condition_broadcast(condition));
}

lh_void
lh_exec_thread_lock(lh_exec_thread_t *self)
{
    lh_runtime_terminate_unless(lh_os_mutex_lock(lh_exec_thread_get_mutex_mut(self)));
}

lh_void
lh_exec_thread_unlock(lh_exec_thread_t *self)
{
    lh_runtime_terminate_unless(lh_os_mutex_unlock(lh_exec_thread_get_mutex_mut(self)));
}

lh_void
lh_exec_thread_release(lh_exec_thread_t *self)
{
    lh_runtime_terminate_unless(lh_os_condition_close(lh_exec_thread_get_end_mut(self)));
    lh_runtime_terminate_unless(lh_os_condition_close(lh_exec_thread_get_wake_mut(self)));
    lh_runtime_terminate_unless(lh_os_mutex_close(lh_exec_thread_get_mutex_mut(self)));
}

lh_void
lh_exec_thread_run_task(lh_exec_thread_t *self, lh_exec_task_t *task)
{
    if (!lh_exec_task_is_cancelled(task))
    {
        lh_exec_task_run_work(task, lh_exec_thread_get_local(self));
    }
    /* After the post the owner may dispatch and free the task: not touched again. */
    lh_exec_mailbox_post(lh_exec_task_get_mailbox_mut(task), task);
}

lh_bool_t
lh_exec_thread_try_accept(lh_ptr context, lh_exec_task_t *task)
{
    lh_exec_thread_t *self = lh_ptr_rcast(lh_exec_thread_t, context);
    lh_exec_thread_lock(self);
    if (lh_math_ne(lh_exec_thread_get_stop(self), lh_exec_stop_kind_none))
    {
        lh_exec_thread_unlock(self);
        return lh_bool_false;
    }
    /* Linked under the mutex: the thread, which pops under it too, sees either
       the whole task or none of it, and a stop cannot slip in between. */
    lh_exec_queue_push(lh_exec_thread_get_pending_mut(self), task);
    lh_exec_thread_broadcast(lh_exec_thread_get_wake_mut(self));
    lh_exec_thread_unlock(self);
    return lh_bool_true;
}

lh_exec_task_t *
lh_exec_thread_wait_task(lh_exec_thread_t *self)
{
    lh_exec_thread_lock(self);
    lh_exec_task_t *task = lh_exec_queue_try_pop(lh_exec_thread_get_pending_mut(self));
    while (lh_null_eq(task) && lh_math_eq(lh_exec_thread_get_stop(self), lh_exec_stop_kind_none))
    {
        lh_runtime_terminate_unless(lh_os_condition_wait(lh_exec_thread_get_wake_mut(self),
                                                         lh_exec_thread_get_mutex_mut(self)));
        task = lh_exec_queue_try_pop(lh_exec_thread_get_pending_mut(self));
    }
    if (lh_ptr_is_set(task) && lh_math_eq(lh_exec_thread_get_stop(self), lh_exec_stop_kind_cancel))
    {
        lh_exec_task_set_cancelled(task, lh_bool_true);
    }
    lh_exec_thread_unlock(self);
    return task;
}

lh_bool_t
lh_exec_thread_try_start(lh_exec_thread_t *self, const lh_exec_context_t *context)
{
    lh_exec_executor_init(lh_exec_thread_get_executor_mut(self), lh_exec_thread_try_accept, self);
    lh_exec_queue_init(lh_exec_thread_get_pending_mut(self));
    lh_exec_context_assign(lh_exec_thread_get_context_mut(self), context);
    lh_exec_thread_set_local(self, lh_null);
    lh_exec_thread_set_stop(self, lh_exec_stop_kind_none);
    lh_exec_thread_set_ended(self, lh_bool_false);
    lh_os_mutex_init(lh_exec_thread_get_mutex_mut(self));
    lh_os_condition_init(lh_exec_thread_get_wake_mut(self));
    lh_os_condition_init(lh_exec_thread_get_end_mut(self));
    lh_os_thread_init(lh_exec_thread_get_thread_mut(self));
    if (!lh_os_mutex_open(lh_exec_thread_get_mutex_mut(self)) ||
        !lh_os_condition_open(lh_exec_thread_get_wake_mut(self)) ||
        !lh_os_condition_open(lh_exec_thread_get_end_mut(self)) ||
        !lh_os_thread_start(lh_exec_thread_get_thread_mut(self), lh_exec_thread_run, self, -1, 0U))
    {
        /* Closing an object that never opened does nothing. */
        lh_exec_thread_release(self);
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_exec_thread_try_stop(lh_exec_thread_t *self, lh_exec_stop_kind_t kind, lh_u32_t timeout_ms)
{
    lh_u64_t start = lh_os_tick_ms64();
    lh_exec_thread_lock(self);
    /* none < drain < cancel: a stop only ever gets stricter. */
    if (lh_math_lt(lh_exec_thread_get_stop(self), kind))
    {
        lh_exec_thread_set_stop(self, kind);
        lh_exec_thread_broadcast(lh_exec_thread_get_wake_mut(self));
    }
    while (!lh_exec_thread_is_ended(self))
    {
        lh_u64_t elapsed = lh_math_sub(lh_os_tick_ms64(), start);
        if (lh_math_ge(elapsed, timeout_ms))
        {
            break;
        }
        lh_runtime_terminate_unless(lh_os_condition_wait_for(
            lh_exec_thread_get_end_mut(self), lh_exec_thread_get_mutex_mut(self),
            lh_cast_static(lh_u32_t, lh_math_sub(timeout_ms, elapsed))));
    }
    lh_bool_t ended = lh_exec_thread_is_ended(self);
    lh_exec_thread_unlock(self);
    return ended;
}

lh_void
lh_exec_thread_destroy(lh_exec_thread_t *self)
{
    lh_exec_thread_lock(self);
    if (lh_math_eq(lh_exec_thread_get_stop(self), lh_exec_stop_kind_none))
    {
        lh_exec_thread_set_stop(self, lh_exec_stop_kind_drain);
        lh_exec_thread_broadcast(lh_exec_thread_get_wake_mut(self));
    }
    while (!lh_exec_thread_is_ended(self))
    {
        lh_runtime_terminate_unless(lh_os_condition_wait(lh_exec_thread_get_end_mut(self),
                                                         lh_exec_thread_get_mutex_mut(self)));
    }
    lh_exec_thread_unlock(self);
    lh_runtime_terminate_unless(lh_os_thread_join(lh_exec_thread_get_thread_mut(self)));
    lh_exec_thread_release(self);
}

lh_void
lh_exec_thread_run(lh_ptr user)
{
    lh_exec_thread_t *self = lh_ptr_rcast(lh_exec_thread_t, user);
    lh_exec_thread_set_local(self, lh_exec_context_run_open(lh_exec_thread_get_context(self)));
    for (lh_exec_task_t *task = lh_exec_thread_wait_task(self); lh_ptr_is_set(task);
         task = lh_exec_thread_wait_task(self))
    {
        lh_exec_thread_run_task(self, task);
    }
    lh_exec_context_run_close(lh_exec_thread_get_context(self), lh_exec_thread_get_local(self));
    lh_exec_thread_lock(self);
    lh_exec_thread_set_ended(self, lh_bool_true);
    lh_exec_thread_broadcast(lh_exec_thread_get_end_mut(self));
    lh_exec_thread_unlock(self);
}
