#include <lh/os/rail/base.h>
#include <lh/os/condition.h>
#include <lh/os/thread.h>
#include <lh/os/tick.h>
#include <lh/rail/base/initializer.h>
#include <lh/rail/base/wait/forever.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/util/return.h>

lh_u64_t
lh_os_rail_base_now_ms(lh_void)
{
    return lh_os_tick_ms64();
}

lh_ptr
lh_os_rail_base_lock_open(lh_void)
{
    lh_os_mutex_t *mutex =
        lh_ptr_rcast(lh_os_mutex_t, lh_runtime_allocator_try_alloc(sizeof(lh_ptr_deref(mutex))));
    lh_return_if(lh_null_eq(mutex), lh_null);
    lh_os_mutex_init(mutex);
    if (!lh_os_mutex_open(mutex))
    {
        lh_runtime_allocator_free(mutex);
        return lh_null;
    }
    return mutex;
}

lh_bool_t
lh_os_rail_base_lock_close(lh_ptr lock)
{
    lh_return_if(!lh_os_mutex_close(lh_ptr_rcast(lh_os_mutex_t, lock)), lh_bool_false);
    lh_runtime_allocator_free(lock);
    return lh_bool_true;
}

lh_bool_t
lh_os_rail_base_lock(lh_ptr lock)
{
    return lh_os_mutex_lock(lh_ptr_rcast(lh_os_mutex_t, lock));
}

lh_bool_t
lh_os_rail_base_unlock(lh_ptr lock)
{
    return lh_os_mutex_unlock(lh_ptr_rcast(lh_os_mutex_t, lock));
}

lh_ptr
lh_os_rail_base_signal_open(lh_void)
{
    lh_os_condition_t *condition = lh_ptr_rcast(
        lh_os_condition_t, lh_runtime_allocator_try_alloc(sizeof(lh_ptr_deref(condition))));
    lh_return_if(lh_null_eq(condition), lh_null);
    lh_os_condition_init(condition);
    if (!lh_os_condition_open(condition))
    {
        lh_runtime_allocator_free(condition);
        return lh_null;
    }
    return condition;
}

lh_bool_t
lh_os_rail_base_signal_close(lh_ptr signal)
{
    lh_return_if(!lh_os_condition_close(lh_ptr_rcast(lh_os_condition_t, signal)), lh_bool_false);
    lh_runtime_allocator_free(signal);
    return lh_bool_true;
}

lh_bool_t
lh_os_rail_base_signal_wait(lh_ptr signal, lh_ptr lock, lh_u32_t timeout_ms)
{
    lh_os_condition_t *condition = lh_ptr_rcast(lh_os_condition_t, signal);
    lh_os_mutex_t *mutex = lh_ptr_rcast(lh_os_mutex_t, lock);
    if (lh_math_eq(timeout_ms, lh_rail_base_wait_forever))
    {
        return lh_os_condition_wait(condition, mutex);
    }
    return lh_os_condition_wait_for(condition, mutex, timeout_ms);
}

lh_bool_t
lh_os_rail_base_signal_wake(lh_ptr signal)
{
    return lh_os_condition_broadcast(lh_ptr_rcast(lh_os_condition_t, signal));
}

lh_ptr
lh_os_rail_base_thread_start(lh_rail_base_run_cb run, lh_ptr user, lh_s32_t cpu,
                             lh_u32_t flags)
{
    lh_os_thread_t *thread =
        lh_ptr_rcast(lh_os_thread_t, lh_runtime_allocator_try_alloc(sizeof(lh_ptr_deref(thread))));
    lh_return_if(lh_null_eq(thread), lh_null);
    lh_os_thread_init(thread);
    if (!lh_os_thread_start(thread, run, user, cpu, flags))
    {
        lh_runtime_allocator_free(thread);
        return lh_null;
    }
    return thread;
}

lh_bool_t
lh_os_rail_base_thread_join(lh_ptr thread)
{
    lh_return_if(!lh_os_thread_join(lh_ptr_rcast(lh_os_thread_t, thread)), lh_bool_false);
    lh_runtime_allocator_free(thread);
    return lh_bool_true;
}

lh_bool_t
lh_os_rail_base_sleep(lh_u32_t milliseconds)
{
    return lh_os_thread_sleep(milliseconds);
}

/* The one OS base; the host copies it into the rail slot. */
const lh_rail_base_t m_os_rail_base = lh_rail_base_initializer(
    lh_os_rail_base_now_ms, lh_os_rail_base_lock_open, lh_os_rail_base_lock_close,
    lh_os_rail_base_lock, lh_os_rail_base_unlock, lh_os_rail_base_signal_open,
    lh_os_rail_base_signal_close, lh_os_rail_base_signal_wait, lh_os_rail_base_signal_wake,
    lh_os_rail_base_thread_start, lh_os_rail_base_thread_join, lh_os_rail_base_sleep);

const lh_rail_base_t *
lh_os_rail_base_get(lh_void)
{
    return lh_addr_of(m_os_rail_base);
}
