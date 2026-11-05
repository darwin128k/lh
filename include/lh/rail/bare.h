/**
 * @file bare.h
 * @brief The rail base without an OS: one core, interrupts, no threads.
 *
 * One implementation of the rail base (lh/rail/base.h), for a board with no
 * operating system — a Cortex-M0 under `main`. It stands on the CPU layer
 * (lh/rail/system/cpu.h): interrupt masking and waiting for an interrupt.
 *
 * - The clock counts milliseconds the board's 1 ms timer interrupt hands it:
 *   the host calls ::lh_rail_bare_tick from that handler (SysTick).
 * - A critical section masks interrupts and restores the mask it found, so
 *   sections nest (::lh_rail_bare_critical_get, for the core's slot,
 *   lh/core/runtime/critical.h).
 * - A lock is such a section: with one core, shutting interrupts out is all
 *   the exclusion there is.
 * - A signal is a flag an interrupt handler or the main code raises
 *   (::lh_rail_bare_signal_wake); a wait sleeps for interrupts under the mask
 *   (no wake is lost) until the flag is up or the time is over.
 * - There are no threads: the thread group is empty, and a loop is driven by
 *   passes from `main` (lh_rail_loop_pass).
 *
 * The host fills both slots before it enables the timer interrupt:
 * `lh_critical_assign(lh_runtime_critical_get_mut(), lh_rail_bare_critical_get());`
 * `lh_rail_base_assign(lh_rail_runtime_base_get_mut(), lh_rail_bare_get());`
 * Locks and signals take their few bytes from the runtime allocator.
 */

#ifndef LH_RAIL_BARE_H
#define LH_RAIL_BARE_H

#include <lh/core/critical.h>
#include <lh/rail/base.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The milliseconds counted so far (level 1); read it under the mask.
 * @return The count.
 */
lh_u64_t
lh_rail_bare_get_ms(void);

/**
 * @brief Store the milliseconds counted so far (level 1).
 * @param ms The count.
 */
lh_void
lh_rail_bare_set_ms(lh_u64_t ms);

/**
 * @brief Begin a section: mask interrupts (an ::lh_critical_enter_fn).
 * @return The mask before, for ::lh_rail_bare_critical_leave.
 */
lh_usize_t
lh_rail_bare_critical_enter(void);

/**
 * @brief End a section: restore the mask it found (an ::lh_critical_leave_fn).
 * @param state What ::lh_rail_bare_critical_enter returned.
 */
lh_void
lh_rail_bare_critical_leave(lh_usize_t state);

/**
 * @brief The critical section of this base, for the core's slot.
 * @return A constant record with both calls.
 */
const lh_critical_t *
lh_rail_bare_critical_get(void);

/**
 * @brief Make a lock: room for the mask its holder found (an ::lh_rail_base_open_fn).
 * @return The lock, from the runtime allocator; ::lh_null without memory.
 */
lh_ptr
lh_rail_bare_lock_open(void);

/**
 * @brief Free a lock nobody holds (an ::lh_rail_base_handle_fn).
 * @param lock A lock from ::lh_rail_bare_lock_open.
 * @return ::lh_bool_true.
 */
lh_bool_t
lh_rail_bare_lock_close(lh_ptr lock);

/**
 * @brief Make a signal: a lowered flag (an ::lh_rail_base_open_fn).
 * @return The signal, from the runtime allocator; ::lh_null without memory.
 */
lh_ptr
lh_rail_bare_signal_open(void);

/**
 * @brief Free a signal nobody waits on (an ::lh_rail_base_handle_fn).
 * @param signal A signal from ::lh_rail_bare_signal_open.
 * @return ::lh_bool_true.
 */
lh_bool_t
lh_rail_bare_signal_close(lh_ptr signal);

/**
 * @brief Raise @p signal, from an interrupt handler or the main code (an
 *        ::lh_rail_base_handle_fn).
 * @param signal A signal from ::lh_rail_bare_signal_open.
 * @return ::lh_bool_true.
 */
lh_bool_t
lh_rail_bare_signal_wake(lh_ptr signal);

/**
 * @brief The rail base without an OS: clock, lock and signal; no thread group.
 * @return A constant base; copy it into the slot.
 */
const lh_rail_base_t *
lh_rail_bare_get(void);

/**
 * @brief One millisecond passed: call it from the board's 1 ms timer interrupt.
 *
 * Interrupt context only (or anywhere with interrupts masked).
 */
lh_void
lh_rail_bare_tick(void);

/**
 * @brief The milliseconds counted so far (an ::lh_rail_base_now_fn).
 * @return The count, read under the mask (64 bits are two loads on a 32-bit core).
 */
lh_u64_t
lh_rail_bare_now_ms(void);

/**
 * @brief Take a lock: mask interrupts and keep the mask found in it.
 * @param lock A lock from ::lh_rail_bare_lock_open, not held.
 * @return ::lh_bool_true.
 */
lh_bool_t
lh_rail_bare_lock(lh_ptr lock);

/**
 * @brief Give a lock back: restore the mask it kept.
 * @param lock A held lock.
 * @return ::lh_bool_true.
 */
lh_bool_t
lh_rail_bare_unlock(lh_ptr lock);

/**
 * @brief Sleep for interrupts until @p signal is raised or @p timeout_ms passes
 *        (an ::lh_rail_base_wait_fn).
 *
 * The caller holds @p lock, so interrupts are masked: the flag is checked, the
 * core waits for an interrupt, and the lock is given back and taken again for a
 * moment so the handler runs; then the check repeats. A wake that came before
 * the wait is kept and ends it at once (an early return the contract allows).
 * The flag is lowered on the way out.
 *
 * @param signal     A signal from ::lh_rail_bare_signal_open.
 * @param lock       A lock the caller holds.
 * @param timeout_ms Longest sleep, or ::lh_rail_base_wait_forever.
 * @return ::lh_bool_true with @p lock held.
 */
lh_bool_t
lh_rail_bare_signal_wait(lh_ptr signal, lh_ptr lock, lh_u32_t timeout_ms);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_BARE_H */
