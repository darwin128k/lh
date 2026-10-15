/**
 * @file round.c
 * @brief Implementation of `lh/math/float/round.h`.
 */

#include <lh/cast/static.h>
#include <lh/math/float/round.h>

lh_int_t
lh_math_float_floor_to_int(lh_float_t x)
{
    const lh_int_t truncated = lh_cast_static(lh_int_t, x);
    return lh_cast_static(lh_float_t, truncated) > x ? truncated - 1 : truncated;
}

lh_int_t
lh_math_float_ceil_to_int(lh_float_t x)
{
    const lh_int_t truncated = lh_cast_static(lh_int_t, x);
    return lh_cast_static(lh_float_t, truncated) < x ? truncated + 1 : truncated;
}
