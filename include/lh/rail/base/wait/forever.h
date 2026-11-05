/**
 * @file forever.h
 * @brief ::lh_rail_base_wait_forever — the timeout of a wait without a bound.
 */

#ifndef LH_RAIL_BASE_WAIT_FOREVER_H
#define LH_RAIL_BASE_WAIT_FOREVER_H

#include <lh/core/numeric/fixed/types.h>
#include <lh/core/util/numeric.h>

/**
 * @def lh_rail_base_wait_forever
 * @brief The `timeout_ms` of an ::lh_rail_base_wait_fn that sleeps until a wake.
 *
 * The largest ::lh_u32_t: a base must not read it as some 49 days.
 */
#define lh_rail_base_wait_forever lh_numeric_limit_umax(lh_u32_t)

#endif /* LH_RAIL_BASE_WAIT_FOREVER_H */
