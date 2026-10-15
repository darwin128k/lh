/**
 * @file float.h
 * @brief Single-precision floating point: ::lh_float_t.
 *
 * A numeric type, next to the integers in `lh/numeric/types.h`. ::lh_float_t
 * is `float`. There is no `double` alias: on a single-precision FPU
 * (Cortex-M4F) a `double` is emulated in software, and a stray `1.0` literal
 * or `sqrt` call is enough to pull that path in. lh writes `1.0f` and uses
 * the single-precision builtins throughout.
 *
 * Also provides the limit macros ::LH_FLOAT_T_MIN, ::LH_FLOAT_T_MAX and
 * ::LH_FLOAT_T_EPS, taken from the freestanding C11 header `<float.h>`.
 */

#ifndef LH_NUMERIC_FLOAT_H
#define LH_NUMERIC_FLOAT_H

#include <float.h>

/**
 * @typedef lh_float_t
 * @brief Single-precision floating point (`float`).
 *
 * Alias for: `float`
 */
typedef float lh_float_t;

#ifndef LH_FLOAT_T_MIN
/**
 * @def LH_FLOAT_T_MIN
 * @brief Smallest positive normalized value of ::lh_float_t.
 *
 * Expands to `FLT_MIN`.
 *
 * @note This is the smallest positive number, not the minimum of the range:
 *       the most negative value of ::lh_float_t is `-LH_FLOAT_T_MAX`.
 */
#    define LH_FLOAT_T_MIN FLT_MIN
#endif /* LH_FLOAT_T_MIN */

#ifndef LH_FLOAT_T_MAX
/**
 * @def LH_FLOAT_T_MAX
 * @brief Maximum finite value of ::lh_float_t.
 *
 * Expands to `FLT_MAX`.
 */
#    define LH_FLOAT_T_MAX FLT_MAX
#endif /* LH_FLOAT_T_MAX */

#ifndef LH_FLOAT_T_EPS
/**
 * @def LH_FLOAT_T_EPS
 * @brief Machine epsilon of ::lh_float_t.
 *
 * Difference between `1.0f` and the next representable ::lh_float_t value.
 * Expands to `FLT_EPSILON`.
 */
#    define LH_FLOAT_T_EPS FLT_EPSILON
#endif /* LH_FLOAT_T_EPS */

#endif /* LH_NUMERIC_FLOAT_H */
