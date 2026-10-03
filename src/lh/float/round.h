/**
 * @file round.h
 * @brief Library-private: float to integer rounding without libm.
 *
 * `floorf` / `ceilf` are libm calls on targets without an SSE4.1 / ARMv8
 * rounding instruction in use, and a freestanding build has no libm. A cast
 * truncates toward zero; one comparison corrects it for the other side.
 * Valid while the value fits ::lh_int_t, as pixel coordinates do.
 */

#ifndef LH_SRC_FLOAT_ROUND_H
#define LH_SRC_FLOAT_ROUND_H

#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/float.h>
#include <lh/numeric/types.h>

/**
 * @brief The largest integer not above @p x.
 */
LH_ATTRIBUTE_STATIC
lh_int_t
lh_float_floor_to_int(lh_float_t x)
{
    const lh_int_t truncated = lh_cast_static(lh_int_t, x);
    return lh_cast_static(lh_float_t, truncated) > x ? truncated - 1 : truncated;
}

/**
 * @brief The smallest integer not below @p x.
 */
LH_ATTRIBUTE_STATIC
lh_int_t
lh_float_ceil_to_int(lh_float_t x)
{
    const lh_int_t truncated = lh_cast_static(lh_int_t, x);
    return lh_cast_static(lh_float_t, truncated) < x ? truncated + 1 : truncated;
}

#endif /* LH_SRC_FLOAT_ROUND_H */
