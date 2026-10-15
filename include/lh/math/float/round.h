/**
 * @file round.h
 * @brief Float to integer rounding without libm: ::lh_math_float_floor_to_int,
 *        ::lh_math_float_ceil_to_int.
 *
 * `floorf` / `ceilf` are libm calls on targets without an SSE4.1 / ARMv8
 * rounding instruction in use, and a freestanding build has no libm. A cast
 * truncates toward zero; one comparison corrects it for the other side.
 * Valid while the value fits ::lh_int_t, as pixel coordinates do.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FPU.
 */

#ifndef LH_MATH_FLOAT_ROUND_H
#define LH_MATH_FLOAT_ROUND_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/float.h>
#include <lh/numeric/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The largest integer not above @p x.
 *
 * @param x Value; must fit ::lh_int_t.
 */
lh_int_t
lh_math_float_floor_to_int(lh_float_t x);

/**
 * @brief The smallest integer not below @p x.
 *
 * @param x Value; must fit ::lh_int_t.
 */
lh_int_t
lh_math_float_ceil_to_int(lh_float_t x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_ROUND_H */
