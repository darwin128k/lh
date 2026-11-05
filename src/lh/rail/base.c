#include <lh/rail/base.h>
#include <lh/core/assert/runtime.h>

/* -- Level 1: the fields ------------------------------------------------------ */

lh_rail_base_now_cb
lh_rail_base_get_now_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->now_cb;
}

lh_void
lh_rail_base_set_now_cb(lh_rail_base_t *self, lh_rail_base_now_cb now_cb)
{
    lh_assert_runtime_ref(self);
    self->now_cb = now_cb;
}

lh_rail_base_open_cb
lh_rail_base_get_lock_open_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lock_open_cb;
}

lh_void
lh_rail_base_set_lock_open_cb(lh_rail_base_t *self, lh_rail_base_open_cb lock_open_cb)
{
    lh_assert_runtime_ref(self);
    self->lock_open_cb = lock_open_cb;
}

lh_rail_base_handle_cb
lh_rail_base_get_lock_close_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lock_close_cb;
}

lh_void
lh_rail_base_set_lock_close_cb(lh_rail_base_t *self, lh_rail_base_handle_cb lock_close_cb)
{
    lh_assert_runtime_ref(self);
    self->lock_close_cb = lock_close_cb;
}

lh_rail_base_handle_cb
lh_rail_base_get_lock_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lock_cb;
}

lh_void
lh_rail_base_set_lock_cb(lh_rail_base_t *self, lh_rail_base_handle_cb lock_cb)
{
    lh_assert_runtime_ref(self);
    self->lock_cb = lock_cb;
}

lh_rail_base_handle_cb
lh_rail_base_get_unlock_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->unlock_cb;
}

lh_void
lh_rail_base_set_unlock_cb(lh_rail_base_t *self, lh_rail_base_handle_cb unlock_cb)
{
    lh_assert_runtime_ref(self);
    self->unlock_cb = unlock_cb;
}

lh_rail_base_open_cb
lh_rail_base_get_signal_open_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->signal_open_cb;
}

lh_void
lh_rail_base_set_signal_open_cb(lh_rail_base_t *self, lh_rail_base_open_cb signal_open_cb)
{
    lh_assert_runtime_ref(self);
    self->signal_open_cb = signal_open_cb;
}

lh_rail_base_handle_cb
lh_rail_base_get_signal_close_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->signal_close_cb;
}

lh_void
lh_rail_base_set_signal_close_cb(lh_rail_base_t *self, lh_rail_base_handle_cb signal_close_cb)
{
    lh_assert_runtime_ref(self);
    self->signal_close_cb = signal_close_cb;
}

lh_rail_base_wait_cb
lh_rail_base_get_signal_wait_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->signal_wait_cb;
}

lh_void
lh_rail_base_set_signal_wait_cb(lh_rail_base_t *self, lh_rail_base_wait_cb signal_wait_cb)
{
    lh_assert_runtime_ref(self);
    self->signal_wait_cb = signal_wait_cb;
}

lh_rail_base_handle_cb
lh_rail_base_get_signal_wake_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->signal_wake_cb;
}

lh_void
lh_rail_base_set_signal_wake_cb(lh_rail_base_t *self, lh_rail_base_handle_cb signal_wake_cb)
{
    lh_assert_runtime_ref(self);
    self->signal_wake_cb = signal_wake_cb;
}

lh_rail_base_start_cb
lh_rail_base_get_thread_start_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thread_start_cb;
}

lh_void
lh_rail_base_set_thread_start_cb(lh_rail_base_t *self, lh_rail_base_start_cb thread_start_cb)
{
    lh_assert_runtime_ref(self);
    self->thread_start_cb = thread_start_cb;
}

lh_rail_base_handle_cb
lh_rail_base_get_thread_join_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thread_join_cb;
}

lh_void
lh_rail_base_set_thread_join_cb(lh_rail_base_t *self, lh_rail_base_handle_cb thread_join_cb)
{
    lh_assert_runtime_ref(self);
    self->thread_join_cb = thread_join_cb;
}

lh_rail_base_sleep_cb
lh_rail_base_get_sleep_cb(const lh_rail_base_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sleep_cb;
}

lh_void
lh_rail_base_set_sleep_cb(lh_rail_base_t *self, lh_rail_base_sleep_cb sleep_cb)
{
    lh_assert_runtime_ref(self);
    self->sleep_cb = sleep_cb;
}

/* -- Level 2: groups, copy, calls --------------------------------------------- */

lh_bool_t
lh_rail_base_has_clock(const lh_rail_base_t *self)
{
    return lh_ptr_is_set(lh_rail_base_get_now_cb(self));
}

lh_bool_t
lh_rail_base_has_lock(const lh_rail_base_t *self)
{
    return lh_ptr_is_set(lh_rail_base_get_lock_open_cb(self)) &&
           lh_ptr_is_set(lh_rail_base_get_lock_close_cb(self)) &&
           lh_ptr_is_set(lh_rail_base_get_lock_cb(self)) &&
           lh_ptr_is_set(lh_rail_base_get_unlock_cb(self));
}

lh_bool_t
lh_rail_base_has_signal(const lh_rail_base_t *self)
{
    return lh_ptr_is_set(lh_rail_base_get_signal_open_cb(self)) &&
           lh_ptr_is_set(lh_rail_base_get_signal_close_cb(self)) &&
           lh_ptr_is_set(lh_rail_base_get_signal_wait_cb(self)) &&
           lh_ptr_is_set(lh_rail_base_get_signal_wake_cb(self));
}

lh_bool_t
lh_rail_base_has_thread(const lh_rail_base_t *self)
{
    return lh_ptr_is_set(lh_rail_base_get_thread_start_cb(self)) &&
           lh_ptr_is_set(lh_rail_base_get_thread_join_cb(self)) &&
           lh_ptr_is_set(lh_rail_base_get_sleep_cb(self));
}

lh_void
lh_rail_base_assign(lh_rail_base_t *self, const lh_rail_base_t *other)
{
    lh_rail_base_set_now_cb(self, lh_rail_base_get_now_cb(other));
    lh_rail_base_set_lock_open_cb(self, lh_rail_base_get_lock_open_cb(other));
    lh_rail_base_set_lock_close_cb(self, lh_rail_base_get_lock_close_cb(other));
    lh_rail_base_set_lock_cb(self, lh_rail_base_get_lock_cb(other));
    lh_rail_base_set_unlock_cb(self, lh_rail_base_get_unlock_cb(other));
    lh_rail_base_set_signal_open_cb(self, lh_rail_base_get_signal_open_cb(other));
    lh_rail_base_set_signal_close_cb(self, lh_rail_base_get_signal_close_cb(other));
    lh_rail_base_set_signal_wait_cb(self, lh_rail_base_get_signal_wait_cb(other));
    lh_rail_base_set_signal_wake_cb(self, lh_rail_base_get_signal_wake_cb(other));
    lh_rail_base_set_thread_start_cb(self, lh_rail_base_get_thread_start_cb(other));
    lh_rail_base_set_thread_join_cb(self, lh_rail_base_get_thread_join_cb(other));
    lh_rail_base_set_sleep_cb(self, lh_rail_base_get_sleep_cb(other));
}

lh_u64_t
lh_rail_base_now_ms(const lh_rail_base_t *self)
{
    return lh_rail_base_get_now_cb(self)();
}

lh_ptr
lh_rail_base_lock_open(const lh_rail_base_t *self)
{
    return lh_rail_base_get_lock_open_cb(self)();
}

lh_bool_t
lh_rail_base_lock_close(const lh_rail_base_t *self, lh_ptr lock)
{
    return lh_rail_base_get_lock_close_cb(self)(lock);
}

lh_bool_t
lh_rail_base_lock(const lh_rail_base_t *self, lh_ptr lock)
{
    return lh_rail_base_get_lock_cb(self)(lock);
}

lh_bool_t
lh_rail_base_unlock(const lh_rail_base_t *self, lh_ptr lock)
{
    return lh_rail_base_get_unlock_cb(self)(lock);
}

lh_ptr
lh_rail_base_signal_open(const lh_rail_base_t *self)
{
    return lh_rail_base_get_signal_open_cb(self)();
}

lh_bool_t
lh_rail_base_signal_close(const lh_rail_base_t *self, lh_ptr signal)
{
    return lh_rail_base_get_signal_close_cb(self)(signal);
}

lh_bool_t
lh_rail_base_signal_wait(const lh_rail_base_t *self, lh_ptr signal, lh_ptr lock,
                         lh_u32_t timeout_ms)
{
    return lh_rail_base_get_signal_wait_cb(self)(signal, lock, timeout_ms);
}

lh_bool_t
lh_rail_base_signal_wake(const lh_rail_base_t *self, lh_ptr signal)
{
    return lh_rail_base_get_signal_wake_cb(self)(signal);
}

lh_ptr
lh_rail_base_thread_start(const lh_rail_base_t *self, lh_rail_base_run_cb run, lh_ptr user,
                          lh_s32_t cpu, lh_u32_t flags)
{
    return lh_rail_base_get_thread_start_cb(self)(run, user, cpu, flags);
}

lh_bool_t
lh_rail_base_thread_join(const lh_rail_base_t *self, lh_ptr thread)
{
    return lh_rail_base_get_thread_join_cb(self)(thread);
}

lh_bool_t
lh_rail_base_sleep(const lh_rail_base_t *self, lh_u32_t milliseconds)
{
    return lh_rail_base_get_sleep_cb(self)(milliseconds);
}
