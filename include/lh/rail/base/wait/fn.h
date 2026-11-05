/**
 * @file fn.h
 * @brief ::lh_rail_base_wait_fn — sleep on a signal.
 */

#ifndef LH_RAIL_BASE_WAIT_FN_H
#define LH_RAIL_BASE_WAIT_FN_H

#include <lh/core/bool.h>
#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Give @p lock back, sleep until @p signal is woken or @p timeout_ms
 *        passes, take @p lock again.
 *
 * The caller holds @p lock exactly once. A wake that comes while nobody sleeps on
 * @p signal is not kept: the caller guards what it waits for with @p lock and
 * checks it again after every return, which may also come early for no reason.
 * A timeout is not a failure. ::lh_rail_base_wait_forever waits without a bound.
 *
 * @param signal     A signal's handle.
 * @param lock       The lock every waiter on @p signal uses, held by the caller.
 * @param timeout_ms Longest sleep, in milliseconds.
 * @return ::lh_bool_true with @p lock held again; ::lh_bool_false on a failure.
 */
typedef lh_bool_t(lh_rail_base_wait_fn)(lh_ptr signal, lh_ptr lock, lh_u32_t timeout_ms);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_BASE_WAIT_FN_H */
