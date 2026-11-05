/**
 * @file fn.h
 * @brief ::lh_rail_base_open_fn — make a lock or a signal.
 */

#ifndef LH_RAIL_BASE_OPEN_FN_H
#define LH_RAIL_BASE_OPEN_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Make one lock or one signal and return its handle.
 *
 * The handle names the object in every later call of the same group; the base
 * owns what is behind it until the matching close.
 *
 * @return The handle; ::lh_null when the object cannot be made (no memory, no
 *         system object).
 */
typedef lh_ptr(lh_rail_base_open_fn)(lh_void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_BASE_OPEN_FN_H */
