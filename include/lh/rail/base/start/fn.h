/**
 * @file fn.h
 * @brief ::lh_rail_base_start_fn — start a thread.
 */

#ifndef LH_RAIL_BASE_START_FN_H
#define LH_RAIL_BASE_START_FN_H

#include <lh/rail/base/run/cb.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Start a thread that runs @p run with @p user, and return its handle.
 *
 * @p cpu and @p flags are hints the base may ignore: a base without such a choice
 * (one core, no flags) starts the thread all the same.
 *
 * @param run   The body; never ::lh_null.
 * @param user  The value @p run is given, the caller's until the join.
 * @param cpu   The logical CPU to run on, or -1 for any.
 * @param flags The base's own options (for the OS base, lh/os/thread.h), 0 for
 *              none.
 * @return The handle a join takes; ::lh_null when no thread started (@p run is
 *         not called).
 */
typedef lh_ptr(lh_rail_base_start_fn)(lh_rail_base_run_cb run, lh_ptr user, lh_s32_t cpu,
                                       lh_u32_t flags);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_BASE_START_FN_H */
