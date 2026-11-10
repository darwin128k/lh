/**
 * @file loop.c
 * @brief ::lh_rail_loop_t: a thread, a signal to sleep on and a pending-wake flag.
 *
 * The thread, the lock and the signal are the rail base's (lh/rail/runtime/base.h).
 * The thread clears the pending flag before its step and, under the lock, sleeps
 * only while the flag is clear. A wake sets it under the lock, so it is never
 * lost; a hint sets it without, so one that races the sleep itself waits for the
 * period. A loop with a waiter sleeps in it instead and wakes it for every wake,
 * hint and stop; the waiter keeps the signal pending, so nothing is lost there.
 */

#include <lh/rail/loop.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/cast/const.h>
#include <lh/core/math.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/util/return.h>
#include <lh/core/atomic/bool.h>
#include <lh/rail/runtime/base.h>

struct lh_rail_loop
{
    lh_memory_allocator_t allocator;
    lh_rail_loop_step_cb step;
    lh_ptr user;
    lh_rail_loop_wait_cb wait;
    lh_rail_loop_wake_cb wake;
    lh_ptr waiter;
    lh_u32_t period_ms;
    lh_timer_group_t timers;
    /* Handles of the rail base; lh_null while not made. */
    lh_ptr thread;
    lh_ptr lock;
    lh_ptr signal;
    lh_bool_t started;
    lh_bool_t last_step;
    lh_atomic_bool_t pending;
    lh_atomic_bool_t stopping;
    /* lh_rail_loop_state_*: who frees the step's state at the end. */
    lh_atomic_uint_t state;
    /* Set by the owner before it abandons; read by the thread after it lost. */
    lh_rail_loop_release_cb release;
    lh_ptr release_user;
};

/* -- Level 1: the fields ------------------------------------------------------ */

lh_rail_loop_wait_cb
lh_rail_loop_get_wait_fn(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wait;
}

lh_void
lh_rail_loop_set_wait_fn(lh_rail_loop_t *self, lh_rail_loop_wait_cb wait)
{
    lh_assert_runtime_ref(self);
    self->wait = wait;
}

lh_rail_loop_wake_cb
lh_rail_loop_get_wake_fn(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wake;
}

lh_void
lh_rail_loop_set_wake_fn(lh_rail_loop_t *self, lh_rail_loop_wake_cb wake)
{
    lh_assert_runtime_ref(self);
    self->wake = wake;
}

lh_ptr
lh_rail_loop_get_waiter(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->waiter;
}

lh_void
lh_rail_loop_set_waiter(lh_rail_loop_t *self, lh_ptr waiter)
{
    lh_assert_runtime_ref(self);
    self->waiter = waiter;
}

lh_memory_allocator_t *
lh_rail_loop_get_allocator_mut(lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_rail_loop_step_cb
lh_rail_loop_get_step_fn(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

lh_void
lh_rail_loop_set_step_fn(lh_rail_loop_t *self, lh_rail_loop_step_cb step)
{
    lh_assert_runtime_ref(self);
    self->step = step;
}

lh_ptr
lh_rail_loop_get_user(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_rail_loop_set_user(lh_rail_loop_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

lh_u32_t
lh_rail_loop_get_period_ms(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->period_ms;
}

lh_void
lh_rail_loop_set_period_ms(lh_rail_loop_t *self, lh_u32_t period_ms)
{
    lh_assert_runtime_ref(self);
    self->period_ms = period_ms;
}

lh_timer_group_t *
lh_rail_loop_get_timers_mut(lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->timers);
}

lh_ptr
lh_rail_loop_get_thread(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thread;
}

lh_void
lh_rail_loop_set_thread(lh_rail_loop_t *self, lh_ptr thread)
{
    lh_assert_runtime_ref(self);
    self->thread = thread;
}

lh_ptr
lh_rail_loop_get_lock(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lock;
}

lh_void
lh_rail_loop_set_lock(lh_rail_loop_t *self, lh_ptr lock)
{
    lh_assert_runtime_ref(self);
    self->lock = lock;
}

lh_ptr
lh_rail_loop_get_signal(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->signal;
}

lh_void
lh_rail_loop_set_signal(lh_rail_loop_t *self, lh_ptr signal)
{
    lh_assert_runtime_ref(self);
    self->signal = signal;
}

lh_bool_t
lh_rail_loop_is_started(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->started;
}

lh_void
lh_rail_loop_set_started(lh_rail_loop_t *self, lh_bool_t started)
{
    lh_assert_runtime_ref(self);
    self->started = started;
}

/* Clear the pending wake and say whether there was one. */
lh_bool_t
lh_rail_loop_take_pending(lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_atomic_bool_exchange_acq_rel(lh_addr_of(self->pending), lh_bool_false);
}

lh_bool_t
lh_rail_loop_is_pending(lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_atomic_bool_get_acquire(lh_addr_of(self->pending));
}

lh_void
lh_rail_loop_set_pending(lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    lh_atomic_bool_set_release(lh_addr_of(self->pending), lh_bool_true);
}

lh_bool_t
lh_rail_loop_is_last_step(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_step;
}

lh_void
lh_rail_loop_set_last_step(lh_rail_loop_t *self, lh_bool_t last_step)
{
    lh_assert_runtime_ref(self);
    self->last_step = last_step;
}

lh_bool_t
lh_rail_loop_is_stopping(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_atomic_bool_get_acquire(lh_addr_of(self->stopping));
}

lh_void
lh_rail_loop_set_stopping(lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    lh_atomic_bool_set_release(lh_addr_of(self->stopping), lh_bool_true);
}

lh_atomic_uint_t *
lh_rail_loop_get_state_mut(lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->state);
}

lh_rail_loop_release_cb
lh_rail_loop_get_release_fn(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->release;
}

lh_void
lh_rail_loop_set_release_fn(lh_rail_loop_t *self, lh_rail_loop_release_cb release)
{
    lh_assert_runtime_ref(self);
    self->release = release;
}

lh_ptr
lh_rail_loop_get_release_user(const lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->release_user;
}

lh_void
lh_rail_loop_set_release_user(lh_rail_loop_t *self, lh_ptr release_user)
{
    lh_assert_runtime_ref(self);
    self->release_user = release_user;
}

lh_void
lh_rail_loop_init_flags(lh_rail_loop_t *self)
{
    lh_assert_runtime_ref(self);
    lh_atomic_bool_set_relaxed(lh_addr_of(self->pending), lh_bool_false);
    lh_atomic_bool_set_relaxed(lh_addr_of(self->stopping), lh_bool_false);
    lh_atomic_uint_set_relaxed(lh_addr_of(self->state), lh_rail_loop_state_running);
}

/* -- Level 2 and up ----------------------------------------------------------- */

lh_void
lh_rail_loop_lock(lh_rail_loop_t *self)
{
    LH_ATTRIBUTE_UNUSED(lh_rail_base_lock(lh_rail_runtime_base_get(), lh_rail_loop_get_lock(self)));
}

lh_void
lh_rail_loop_unlock(lh_rail_loop_t *self)
{
    LH_ATTRIBUTE_UNUSED(
        lh_rail_base_unlock(lh_rail_runtime_base_get(), lh_rail_loop_get_lock(self)));
}

lh_void
lh_rail_loop_signal(lh_rail_loop_t *self)
{
    LH_ATTRIBUTE_UNUSED(
        lh_rail_base_signal_wake(lh_rail_runtime_base_get(), lh_rail_loop_get_signal(self)));
}

lh_rail_loop_state_t
lh_rail_loop_load_state(lh_rail_loop_t *self)
{
    return lh_atomic_uint_get_acquire(lh_rail_loop_get_state_mut(self));
}

lh_void
lh_rail_loop_end(lh_rail_loop_t *self)
{
    if (lh_atomic_uint_try_compare_exchange_acq_rel(
            lh_rail_loop_get_state_mut(self), lh_rail_loop_state_running, lh_rail_loop_state_ended))
    {
        return; /* the owner still holds the loop and joins it */
    }
    /* Abandoned: the owner touches neither the loop nor the step's state any more. */
    lh_rail_loop_release_cb release = lh_rail_loop_get_release_fn(self);
    if (lh_null_ne(release))
    {
        release(lh_rail_loop_get_release_user(self));
    }
    lh_atomic_uint_set_release(lh_rail_loop_get_state_mut(self), lh_rail_loop_state_released);
}

lh_void
lh_rail_loop_free(lh_rail_loop_t *self)
{
    const lh_rail_base_t *base = lh_rail_runtime_base_get();
    lh_timer_group_deinit(lh_rail_loop_get_timers_mut(self));
    if (lh_ptr_is_set(lh_rail_loop_get_signal(self)))
    {
        LH_ATTRIBUTE_UNUSED(lh_rail_base_signal_close(base, lh_rail_loop_get_signal(self)));
    }
    if (lh_ptr_is_set(lh_rail_loop_get_lock(self)))
    {
        LH_ATTRIBUTE_UNUSED(lh_rail_base_lock_close(base, lh_rail_loop_get_lock(self)));
    }
    lh_memory_allocator_dealloc(lh_rail_loop_get_allocator_mut(self), self);
}

lh_bool_t
lh_rail_loop_try_abandon(lh_rail_loop_t *self, lh_rail_loop_release_cb release, lh_ptr release_user)
{
    /* Published by the compare-exchange below, read by the thread after it lost it. */
    lh_rail_loop_set_release_fn(self, release);
    lh_rail_loop_set_release_user(self, release_user);
    return lh_atomic_uint_try_compare_exchange_acq_rel(
        lh_rail_loop_get_state_mut(self), lh_rail_loop_state_running, lh_rail_loop_state_abandoned);
}

lh_void
lh_rail_loop_run_step(lh_rail_loop_t *self)
{
    /* Read before the step: the step that sees the stop is the last one. */
    const lh_bool_t stopping = lh_rail_loop_is_stopping(self);
    LH_ATTRIBUTE_UNUSED(lh_rail_loop_take_pending(self));
    lh_rail_loop_set_last_step(self, stopping);
    lh_timer_group_handler(
        lh_rail_loop_get_timers_mut(self),
        lh_cast_static(lh_tick_t, lh_rail_base_now_ms(lh_rail_runtime_base_get())));
    lh_rail_loop_get_step_fn(self)(lh_rail_loop_get_user(self));
}

lh_u32_t
lh_rail_loop_get_next_pass_ms(lh_rail_loop_t *self)
{
    /* A wake or a stop that came during the step: the next pass is due now. */
    lh_return_if(lh_rail_loop_is_pending(self) || lh_rail_loop_is_stopping(self), 0U);
    const lh_tick_t now =
        lh_cast_static(lh_tick_t, lh_rail_base_now_ms(lh_rail_runtime_base_get()));
    return lh_cast_static(
        lh_u32_t, lh_math_min(lh_cast_static(lh_tick_t, lh_rail_loop_get_period_ms(self)),
                              lh_timer_group_until_next(lh_rail_loop_get_timers_mut(self), now)));
}

lh_void
lh_rail_loop_ask_stop(lh_rail_loop_t *self)
{
    if (lh_null_ne(lh_rail_loop_get_wake_fn(self)))
    {
        lh_rail_loop_set_stopping(self);
        lh_rail_loop_get_wake_fn(self)(lh_rail_loop_get_waiter(self));
        return;
    }
    lh_rail_loop_lock(self);
    lh_rail_loop_set_stopping(self);
    lh_rail_loop_signal(self);
    lh_rail_loop_unlock(self);
}

/* Sleep at most @p timeout_ms, unless a wake or a stop is already there. */
lh_void
lh_rail_loop_sleep(lh_rail_loop_t *self, lh_u32_t timeout_ms)
{
    if (lh_null_ne(lh_rail_loop_get_wait_fn(self)))
    {
        if (!lh_rail_loop_is_pending(self) && !lh_rail_loop_is_stopping(self))
        {
            lh_rail_loop_get_wait_fn(self)(lh_rail_loop_get_waiter(self), timeout_ms);
        }
        return;
    }
    lh_rail_loop_lock(self);
    if (!lh_rail_loop_is_pending(self) && !lh_rail_loop_is_stopping(self))
    {
        LH_ATTRIBUTE_UNUSED(lh_rail_base_signal_wait(lh_rail_runtime_base_get(),
                                                     lh_rail_loop_get_signal(self),
                                                     lh_rail_loop_get_lock(self), timeout_ms));
    }
    lh_rail_loop_unlock(self);
}

lh_void
lh_rail_loop_wake(lh_rail_loop_t *self)
{
    if (lh_null_ne(lh_rail_loop_get_wake_fn(self)))
    {
        lh_rail_loop_set_pending(self);
        lh_rail_loop_get_wake_fn(self)(lh_rail_loop_get_waiter(self));
        return;
    }
    lh_rail_loop_lock(self);
    lh_rail_loop_set_pending(self);
    lh_rail_loop_signal(self);
    lh_rail_loop_unlock(self);
}

lh_void
lh_rail_loop_hint(lh_rail_loop_t *self)
{
    lh_rail_loop_set_pending(self);
    if (lh_null_ne(lh_rail_loop_get_wake_fn(self)))
    {
        lh_rail_loop_get_wake_fn(self)(lh_rail_loop_get_waiter(self));
        return;
    }
    lh_rail_loop_signal(self);
}

lh_bool_t
lh_rail_loop_is_ended(lh_rail_loop_t *self)
{
    return lh_math_eq(lh_rail_loop_load_state(self), lh_rail_loop_state_ended);
}

lh_bool_t
lh_rail_loop_try_reap(lh_rail_loop_t *self)
{
    lh_return_if(lh_math_ne(lh_rail_loop_load_state(self), lh_rail_loop_state_released),
                 lh_bool_false);
    /* The thread only returns from its body now: the join waits for no work. */
    LH_ATTRIBUTE_UNUSED(
        lh_rail_base_thread_join(lh_rail_runtime_base_get(), lh_rail_loop_get_thread(self)));
    lh_rail_loop_free(self);
    return lh_bool_true;
}

lh_u32_t
lh_rail_loop_pass(lh_rail_loop_t *self)
{
    lh_rail_loop_run_step(self);
    return lh_rail_loop_get_next_pass_ms(self);
}

lh_void
lh_rail_loop_destroy(lh_rail_loop_t *self)
{
    lh_return_if(lh_null_eq(self));
    if (lh_rail_loop_is_started(self))
    {
        lh_rail_loop_ask_stop(self);
        LH_ATTRIBUTE_UNUSED(
            lh_rail_base_thread_join(lh_rail_runtime_base_get(), lh_rail_loop_get_thread(self)));
    }
    lh_rail_loop_free(self);
}

lh_void
lh_rail_loop_run(lh_ptr user)
{
    lh_rail_loop_t *self = lh_ptr_rcast(lh_rail_loop_t, user);
    for (lh_u32_t next_ms = lh_rail_loop_pass(self); !lh_rail_loop_is_last_step(self);
         next_ms = lh_rail_loop_pass(self))
    {
        lh_rail_loop_sleep(self, next_ms);
    }
    lh_rail_loop_end(self);
}

lh_bool_t
lh_rail_loop_try_stop(lh_rail_loop_t *self, lh_u32_t timeout_ms)
{
    lh_return_if(!lh_rail_loop_is_started(self), lh_bool_true);
    lh_rail_loop_ask_stop(self);
    const lh_rail_base_t *base = lh_rail_runtime_base_get();
    const lh_u64_t start = lh_rail_base_now_ms(base);
    /* The end is one atomic store on the loop's thread, which may sleep in a waiter
       that has no signal to wake: a short poll serves both kinds of loop. */
    while (!lh_rail_loop_is_ended(self) && lh_math_lt(lh_math_sub(lh_rail_base_now_ms(base), start),
                                                      lh_cast_static(lh_u64_t, timeout_ms)))
    {
        LH_ATTRIBUTE_UNUSED(lh_rail_base_sleep(base, 1U));
    }
    return lh_rail_loop_is_ended(self);
}

lh_bool_t
lh_rail_loop_try_start_on(lh_rail_loop_t *self, lh_s32_t cpu, lh_u32_t flags)
{
    const lh_rail_base_t *base = lh_rail_runtime_base_get();
    lh_return_if(lh_rail_loop_is_started(self) || !lh_rail_base_has_thread(base), lh_bool_false);
    lh_rail_loop_set_thread(self,
                            lh_rail_base_thread_start(base, lh_rail_loop_run, self, cpu, flags));
    lh_rail_loop_set_started(self, lh_ptr_is_set(lh_rail_loop_get_thread(self)));
    return lh_rail_loop_is_started(self);
}

lh_rail_loop_t *
lh_rail_loop_try_create(lh_u32_t period_ms, lh_rail_loop_step_cb step, lh_ptr user)
{
    const lh_rail_base_t *base = lh_rail_runtime_base_get();
    lh_return_if(period_ms == 0U || lh_null_eq(step), lh_null);
    lh_return_if(!lh_rail_base_has_clock(base) || !lh_rail_base_has_lock(base) ||
                     !lh_rail_base_has_signal(base),
                 lh_null);
    lh_memory_allocator_t allocator = lh_ptr_deref(lh_runtime_allocator_get());
    lh_rail_loop_t *self = lh_ptr_rcast(
        lh_rail_loop_t,
        lh_memory_allocator_try_alloc_zero(lh_addr_of(allocator), 1U, sizeof(lh_ptr_deref(self))));

    lh_return_if(lh_null_eq(self), lh_null);
    /* Freed with the allocator it was made with, whatever the runtime one is then. */
    lh_memory_allocator_assign(lh_rail_loop_get_allocator_mut(self), lh_addr_of(allocator));
    lh_rail_loop_set_step_fn(self, step);
    lh_rail_loop_set_user(self, user);
    lh_rail_loop_set_period_ms(self, period_ms);
    lh_rail_loop_set_started(self, lh_bool_false);
    lh_rail_loop_set_last_step(self, lh_bool_false);
    lh_rail_loop_set_release_fn(self, lh_null);
    lh_rail_loop_set_release_user(self, lh_null);
    lh_rail_loop_init_flags(self);
    lh_timer_group_init(lh_rail_loop_get_timers_mut(self));
    lh_rail_loop_set_thread(self, lh_null);
    lh_rail_loop_set_lock(self, lh_rail_base_lock_open(base));
    lh_rail_loop_set_signal(self, lh_rail_base_signal_open(base));
    if (lh_null_eq(lh_rail_loop_get_lock(self)) || lh_null_eq(lh_rail_loop_get_signal(self)))
    {
        lh_rail_loop_destroy(self);
        return lh_null;
    }
    return self;
}

lh_bool_t
lh_rail_loop_try_start(lh_rail_loop_t *self)
{
    return lh_rail_loop_try_start_on(self, -1, 0U);
}

lh_rail_loop_t *
lh_rail_loop_try_create_waiting(lh_u32_t period_ms, lh_rail_loop_step_cb step, lh_ptr user,
                                lh_rail_loop_wait_cb wait, lh_rail_loop_wake_cb wake, lh_ptr waiter)
{
    lh_rail_loop_t *self;

    lh_return_if(lh_null_eq(wait) || lh_null_eq(wake), lh_null);
    self = lh_rail_loop_try_create(period_ms, step, user);
    lh_return_if(lh_null_eq(self), lh_null);
    lh_rail_loop_set_wait_fn(self, wait);
    lh_rail_loop_set_wake_fn(self, wake);
    lh_rail_loop_set_waiter(self, waiter);
    return self;
}
