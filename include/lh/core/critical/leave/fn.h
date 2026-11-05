/**
 * @file fn.h
 * @brief ::lh_critical_leave_fn — end a critical section.
 */

#ifndef LH_CRITICAL_LEAVE_FN_H
#define LH_CRITICAL_LEAVE_FN_H

#include <lh/core/size.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Let the others in again, restoring what the matching enter found.
 *
 * Leaving is a full memory barrier: everything written inside is seen by the
 * next enter, on any thread.
 *
 * @param state The token the matching ::lh_critical_enter_fn returned.
 */
typedef lh_void(lh_critical_leave_fn)(lh_usize_t state);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRITICAL_LEAVE_FN_H */
