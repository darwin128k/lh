/**
 * @file fn.h
 * @brief ::lh_rail_base_now_fn — the base's clock.
 */

#ifndef LH_RAIL_BASE_NOW_FN_H
#define LH_RAIL_BASE_NOW_FN_H

#include <lh/core/numeric/fixed/types.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Milliseconds of a monotonic clock.
 *
 * The value never goes back; where it starts is the base's own (boot, first
 * call). rail measures periods, timers and stop timeouts with it, nothing else.
 *
 * @return The milliseconds now.
 */
typedef lh_u64_t(lh_rail_base_now_fn)(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_BASE_NOW_FN_H */
