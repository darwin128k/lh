/**
 * @file fn.h
 * @brief ::lh_rail_base_handle_fn — one action on an object the base made.
 */

#ifndef LH_RAIL_BASE_HANDLE_FN_H
#define LH_RAIL_BASE_HANDLE_FN_H

#include <lh/core/bool.h>
#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Act on one object the base made: close it, take or give back a lock,
 *        wake a signal's waiters, join a thread.
 *
 * Which action it is follows from the slot the call sits in (lh/rail/base.h).
 * A close and a join also free the object: the handle names nothing afterwards.
 *
 * @param handle The handle an open or a start returned; never ::lh_null.
 * @return ::lh_bool_true when the action was done; ::lh_bool_false on a failure
 *         (the object is left as it was).
 */
typedef lh_bool_t(lh_rail_base_handle_fn)(lh_ptr handle);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_BASE_HANDLE_FN_H */
