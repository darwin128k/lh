#include <lh/rail/bare.h>
#include <lh/core/atomic/bool.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/critical/initializer.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/util/return.h>
#include <lh/rail/base/initializer.h>
#include <lh/rail/base/wait/forever.h>
#include <lh/rail/system/cpu.h>

/* Milliseconds counted by the board's timer interrupt. */
lh_u64_t m_rail_bare_ms = 0U;
lh_u64_t
lh_rail_bare_get_ms(void)
{
    return m_rail_bare_ms;
}

lh_void
lh_rail_bare_set_ms(lh_u64_t ms)
{
    m_rail_bare_ms = ms;
}

lh_usize_t
lh_rail_bare_critical_enter(void)
{
    return lh_rail_system_cpu_mask_interrupts();
}

lh_void
lh_rail_bare_critical_leave(lh_usize_t state)
{
    lh_rail_system_cpu_restore_interrupts(state);
}

const lh_critical_t m_rail_bare_critical =
    lh_critical_initializer(lh_rail_bare_critical_enter, lh_rail_bare_critical_leave);

const lh_critical_t *
lh_rail_bare_critical_get(void)
{
    return lh_addr_of(m_rail_bare_critical);
}

lh_ptr
lh_rail_bare_lock_open(void)
{
    lh_usize_t *lock = lh_ptr_rcast(lh_usize_t, lh_runtime_allocator_try_alloc(sizeof(lh_usize_t)));
    lh_return_if(lh_null_eq(lock), lh_null);
    lh_ptr_deref(lock) = 0U;
    return lock;
}

lh_bool_t
lh_rail_bare_lock_close(lh_ptr lock)
{
    lh_runtime_allocator_free(lock);
    return lh_bool_true;
}

lh_ptr
lh_rail_bare_signal_open(void)
{
    lh_atomic_bool_t *flag =
        lh_ptr_rcast(lh_atomic_bool_t, lh_runtime_allocator_try_alloc(sizeof(lh_atomic_bool_t)));
    lh_return_if(lh_null_eq(flag), lh_null);
    lh_atomic_bool_set_relaxed(flag, lh_bool_false);
    return flag;
}

lh_bool_t
lh_rail_bare_signal_close(lh_ptr signal)
{
    lh_runtime_allocator_free(signal);
    return lh_bool_true;
}

lh_bool_t
lh_rail_bare_signal_wake(lh_ptr signal)
{
    lh_atomic_bool_set_release(lh_ptr_rcast(lh_atomic_bool_t, signal), lh_bool_true);
    return lh_bool_true;
}

/* The one base without an OS; the host copies it into the rail slot. No threads. */
const lh_rail_base_t m_rail_bare_base = lh_rail_base_initializer(
    lh_rail_bare_now_ms, lh_rail_bare_lock_open, lh_rail_bare_lock_close, lh_rail_bare_lock,
    lh_rail_bare_unlock, lh_rail_bare_signal_open, lh_rail_bare_signal_close,
    lh_rail_bare_signal_wait, lh_rail_bare_signal_wake, lh_null, lh_null, lh_null);

const lh_rail_base_t *
lh_rail_bare_get(void)
{
    return lh_addr_of(m_rail_bare_base);
}

lh_void
lh_rail_bare_tick(void)
{
    lh_rail_bare_set_ms(lh_math_add_one(lh_rail_bare_get_ms()));
}

lh_u64_t
lh_rail_bare_now_ms(void)
{
    const lh_usize_t state = lh_rail_bare_critical_enter();
    const lh_u64_t ms = lh_rail_bare_get_ms();
    lh_rail_bare_critical_leave(state);
    return ms;
}

lh_bool_t
lh_rail_bare_lock(lh_ptr lock)
{
    const lh_usize_t state = lh_rail_bare_critical_enter();
    lh_ptr_deref(lh_ptr_rcast(lh_usize_t, lock)) = state;
    return lh_bool_true;
}

lh_bool_t
lh_rail_bare_unlock(lh_ptr lock)
{
    lh_rail_bare_critical_leave(lh_ptr_deref(lh_ptr_rcast(lh_usize_t, lock)));
    return lh_bool_true;
}

lh_bool_t
lh_rail_bare_signal_wait(lh_ptr signal, lh_ptr lock, lh_u32_t timeout_ms)
{
    lh_atomic_bool_t *flag = lh_ptr_rcast(lh_atomic_bool_t, signal);
    const lh_u64_t start = lh_rail_bare_now_ms();
    while (!lh_atomic_bool_get_acquire(flag))
    {
        if (lh_math_ne(timeout_ms, lh_rail_base_wait_forever) &&
            lh_math_ge(lh_math_sub(lh_rail_bare_now_ms(), start), timeout_ms))
        {
            break;
        }
        /* Masked: a pending interrupt wakes the core but waits for the unmask. */
        lh_rail_system_cpu_wait_for_interrupt();
        LH_ATTRIBUTE_UNUSED(lh_rail_bare_unlock(lock));
        LH_ATTRIBUTE_UNUSED(lh_rail_bare_lock(lock));
    }
    LH_ATTRIBUTE_UNUSED(lh_atomic_bool_exchange_acq_rel(flag, lh_bool_false));
    return lh_bool_true;
}
