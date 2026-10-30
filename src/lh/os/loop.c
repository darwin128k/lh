/**
 * @file loop.c
 * @brief ::lh_os_loop_t: a thread, a condition to sleep on and a pending-wake flag.
 *
 * The thread clears the pending flag before its step and, under the mutex, sleeps
 * only while the flag is clear. A wake sets it under the mutex, so it is never
 * lost; a hint sets it without, so one that races the sleep itself waits for the
 * period. A loop with a waiter sleeps in it instead and wakes it for every wake,
 * hint and stop; the waiter keeps the signal pending, so nothing is lost there.
 */

#include <lh/os/loop.h>
#include <lh/assert/runtime.h>
#include <lh/attribute/unused.h>
#include <lh/cast/const.h>
#include <lh/math.h>
#include <lh/os/tick.h>
#include <lh/runtime/allocator.h>
#include <lh/util/return.h>
#include <stdatomic.h>

struct lh_os_loop
{
    lh_memory_allocator_t allocator;
    lh_os_loop_step_cb step;
    lh_ptr user;
    lh_os_loop_wait_cb wait;
    lh_os_loop_wake_cb wake;
    lh_ptr waiter;
    lh_u32_t period_ms;
    lh_timer_group_t timers;
    lh_os_thread_t thread;
    lh_os_mutex_t mutex;
    lh_os_condition_t condition;
    lh_bool_t started;
    lh_bool_t last_step;
    atomic_bool pending;
    atomic_bool stopping;
};

/* -- Level 1: the fields ------------------------------------------------------ */

lh_os_loop_wait_cb
lh_os_loop_get_wait_fn(const lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wait;
}

lh_void
lh_os_loop_set_wait_fn(lh_os_loop_t *self, lh_os_loop_wait_cb wait)
{
    lh_assert_runtime_ref(self);
    self->wait = wait;
}

lh_os_loop_wake_cb
lh_os_loop_get_wake_fn(const lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wake;
}

lh_void
lh_os_loop_set_wake_fn(lh_os_loop_t *self, lh_os_loop_wake_cb wake)
{
    lh_assert_runtime_ref(self);
    self->wake = wake;
}

lh_ptr
lh_os_loop_get_waiter(const lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->waiter;
}

lh_void
lh_os_loop_set_waiter(lh_os_loop_t *self, lh_ptr waiter)
{
    lh_assert_runtime_ref(self);
    self->waiter = waiter;
}

lh_memory_allocator_t *
lh_os_loop_get_allocator_mut(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_os_loop_step_cb
lh_os_loop_get_step_fn(const lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

lh_void
lh_os_loop_set_step_fn(lh_os_loop_t *self, lh_os_loop_step_cb step)
{
    lh_assert_runtime_ref(self);
    self->step = step;
}

lh_ptr
lh_os_loop_get_user(const lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_os_loop_set_user(lh_os_loop_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

lh_u32_t
lh_os_loop_get_period_ms(const lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->period_ms;
}

lh_void
lh_os_loop_set_period_ms(lh_os_loop_t *self, lh_u32_t period_ms)
{
    lh_assert_runtime_ref(self);
    self->period_ms = period_ms;
}

lh_timer_group_t *
lh_os_loop_get_timers_mut(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->timers);
}

lh_os_thread_t *
lh_os_loop_get_thread_mut(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->thread);
}

lh_os_mutex_t *
lh_os_loop_get_mutex_mut(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->mutex);
}

lh_os_condition_t *
lh_os_loop_get_condition_mut(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->condition);
}

lh_bool_t
lh_os_loop_is_started(const lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->started;
}

lh_void
lh_os_loop_set_started(lh_os_loop_t *self, lh_bool_t started)
{
    lh_assert_runtime_ref(self);
    self->started = started;
}

/* Clear the pending wake and say whether there was one. */
lh_bool_t
lh_os_loop_take_pending(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_exchange_explicit(lh_addr_of(self->pending), 0, memory_order_acq_rel)
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_os_loop_is_pending(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_addr_of(self->pending), memory_order_acquire) ? lh_bool_true
                                                                                 : lh_bool_false;
}

lh_void
lh_os_loop_set_pending(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    atomic_store_explicit(lh_addr_of(self->pending), 1, memory_order_release);
}

lh_bool_t
lh_os_loop_is_last_step(const lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_step;
}

lh_void
lh_os_loop_set_last_step(lh_os_loop_t *self, lh_bool_t last_step)
{
    lh_assert_runtime_ref(self);
    self->last_step = last_step;
}

lh_bool_t
lh_os_loop_is_stopping(const lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_cast_const(atomic_bool *, lh_addr_of(self->stopping)),
                                memory_order_acquire)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_os_loop_set_stopping(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    atomic_store_explicit(lh_addr_of(self->stopping), 1, memory_order_release);
}

lh_void
lh_os_loop_init_flags(lh_os_loop_t *self)
{
    lh_assert_runtime_ref(self);
    atomic_init(lh_addr_of(self->pending), 0);
    atomic_init(lh_addr_of(self->stopping), 0);
}

/* -- Level 2 and up ----------------------------------------------------------- */

/* Sleep at most @p timeout_ms, unless a wake or a stop is already there. */
lh_void
lh_os_loop_sleep(lh_os_loop_t *self, lh_u32_t timeout_ms)
{
    if (lh_null_ne(lh_os_loop_get_wait_fn(self)))
    {
        if (!lh_os_loop_is_pending(self) && !lh_os_loop_is_stopping(self))
        {
            lh_os_loop_get_wait_fn(self)(lh_os_loop_get_waiter(self), timeout_ms);
        }
        return;
    }
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_lock(lh_os_loop_get_mutex_mut(self)));
    if (!lh_os_loop_is_pending(self) && !lh_os_loop_is_stopping(self))
    {
        LH_ATTRIBUTE_UNUSED(lh_os_condition_wait_for(lh_os_loop_get_condition_mut(self),
                                                     lh_os_loop_get_mutex_mut(self), timeout_ms));
    }
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_unlock(lh_os_loop_get_mutex_mut(self)));
}

lh_void
lh_os_loop_run(lh_ptr user)
{
    lh_os_loop_t *self = lh_ptr_rcast(lh_os_loop_t, user);
    lh_timer_group_t *timers = lh_os_loop_get_timers_mut(self);

    for (;;)
    {
        /* Read before the step: the step that sees the stop is the last one. */
        const lh_bool_t stopping = lh_os_loop_is_stopping(self);
        lh_tick_t now;

        LH_ATTRIBUTE_UNUSED(lh_os_loop_take_pending(self));
        lh_os_loop_set_last_step(self, stopping);
        lh_timer_group_handler(timers, lh_os_tick_ms());
        lh_os_loop_get_step_fn(self)(lh_os_loop_get_user(self));
        if (stopping)
        {
            break;
        }
        now = lh_os_tick_ms();
        lh_os_loop_sleep(
            self,
            lh_cast_static(lh_u32_t,
                           lh_math_min(lh_cast_static(lh_tick_t, lh_os_loop_get_period_ms(self)),
                                       lh_timer_group_until_next(timers, now))));
    }
}

lh_bool_t
lh_os_loop_try_start_on(lh_os_loop_t *self, lh_s32_t cpu, lh_u32_t flags)
{
    lh_return_if(lh_os_loop_is_started(self), lh_bool_false);
    lh_os_loop_set_started(self, lh_os_thread_start(lh_os_loop_get_thread_mut(self), lh_os_loop_run,
                                                    self, cpu, flags));
    return lh_os_loop_is_started(self);
}

lh_bool_t
lh_os_loop_try_start(lh_os_loop_t *self)
{
    return lh_os_loop_try_start_on(self, -1, 0U);
}

lh_void
lh_os_loop_wake(lh_os_loop_t *self)
{
    if (lh_null_ne(lh_os_loop_get_wake_fn(self)))
    {
        lh_os_loop_set_pending(self);
        lh_os_loop_get_wake_fn(self)(lh_os_loop_get_waiter(self));
        return;
    }
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_lock(lh_os_loop_get_mutex_mut(self)));
    lh_os_loop_set_pending(self);
    LH_ATTRIBUTE_UNUSED(lh_os_condition_signal(lh_os_loop_get_condition_mut(self)));
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_unlock(lh_os_loop_get_mutex_mut(self)));
}

lh_void
lh_os_loop_hint(lh_os_loop_t *self)
{
    lh_os_loop_set_pending(self);
    if (lh_null_ne(lh_os_loop_get_wake_fn(self)))
    {
        lh_os_loop_get_wake_fn(self)(lh_os_loop_get_waiter(self));
        return;
    }
    LH_ATTRIBUTE_UNUSED(lh_os_condition_signal(lh_os_loop_get_condition_mut(self)));
}

lh_void
lh_os_loop_destroy(lh_os_loop_t *self)
{
    lh_return_if(lh_null_eq(self));
    if (lh_os_loop_is_started(self) && lh_null_ne(lh_os_loop_get_wake_fn(self)))
    {
        lh_os_loop_set_stopping(self);
        lh_os_loop_get_wake_fn(self)(lh_os_loop_get_waiter(self));
        LH_ATTRIBUTE_UNUSED(lh_os_thread_join(lh_os_loop_get_thread_mut(self)));
    }
    else if (lh_os_loop_is_started(self))
    {
        LH_ATTRIBUTE_UNUSED(lh_os_mutex_lock(lh_os_loop_get_mutex_mut(self)));
        lh_os_loop_set_stopping(self);
        LH_ATTRIBUTE_UNUSED(lh_os_condition_signal(lh_os_loop_get_condition_mut(self)));
        LH_ATTRIBUTE_UNUSED(lh_os_mutex_unlock(lh_os_loop_get_mutex_mut(self)));
        LH_ATTRIBUTE_UNUSED(lh_os_thread_join(lh_os_loop_get_thread_mut(self)));
    }
    lh_timer_group_deinit(lh_os_loop_get_timers_mut(self));
    LH_ATTRIBUTE_UNUSED(lh_os_condition_close(lh_os_loop_get_condition_mut(self)));
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_close(lh_os_loop_get_mutex_mut(self)));
    lh_memory_allocator_dealloc(lh_os_loop_get_allocator_mut(self), self);
}

lh_os_loop_t *
lh_os_loop_try_create(lh_u32_t period_ms, lh_os_loop_step_cb step, lh_ptr user)
{
    lh_return_if(period_ms == 0U || lh_null_eq(step), lh_null);
    lh_memory_allocator_t allocator = lh_ptr_deref(lh_runtime_allocator());
    lh_os_loop_t *self =
        lh_ptr_rcast(lh_os_loop_t, lh_memory_allocator_try_alloc_zero(lh_addr_of(allocator), 1U,
                                                                      sizeof(lh_ptr_deref(self))));

    lh_return_if(lh_null_eq(self), lh_null);
    /* Freed with the allocator it was made with, whatever the runtime one is then. */
    lh_memory_allocator_assign(lh_os_loop_get_allocator_mut(self), lh_addr_of(allocator));
    lh_os_loop_set_step_fn(self, step);
    lh_os_loop_set_user(self, user);
    lh_os_loop_set_period_ms(self, period_ms);
    lh_os_loop_set_started(self, lh_bool_false);
    lh_os_loop_set_last_step(self, lh_bool_false);
    lh_os_loop_init_flags(self);
    lh_timer_group_init(lh_os_loop_get_timers_mut(self));
    lh_os_thread_init(lh_os_loop_get_thread_mut(self));
    lh_os_mutex_init(lh_os_loop_get_mutex_mut(self));
    lh_os_condition_init(lh_os_loop_get_condition_mut(self));
    if (!lh_os_mutex_open(lh_os_loop_get_mutex_mut(self)) ||
        !lh_os_condition_open(lh_os_loop_get_condition_mut(self)))
    {
        lh_os_loop_destroy(self);
        return lh_null;
    }
    return self;
}

lh_os_loop_t *
lh_os_loop_try_create_waiting(lh_u32_t period_ms, lh_os_loop_step_cb step, lh_ptr user,
                              lh_os_loop_wait_cb wait, lh_os_loop_wake_cb wake, lh_ptr waiter)
{
    lh_os_loop_t *self;

    lh_return_if(lh_null_eq(wait) || lh_null_eq(wake), lh_null);
    self = lh_os_loop_try_create(period_ms, step, user);
    lh_return_if(lh_null_eq(self), lh_null);
    lh_os_loop_set_wait_fn(self, wait);
    lh_os_loop_set_wake_fn(self, wake);
    lh_os_loop_set_waiter(self, waiter);
    return self;
}
