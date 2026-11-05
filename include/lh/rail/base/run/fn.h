/**
 * @file fn.h
 * @brief ::lh_rail_base_run_fn — the body of a thread the base starts.
 */

#ifndef LH_RAIL_BASE_RUN_FN_H
#define LH_RAIL_BASE_RUN_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What a thread does; when it returns the thread ends.
 *
 * @param user The value the start was given.
 */
typedef lh_void(lh_rail_base_run_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_BASE_RUN_FN_H */
