/**
 * @file scalar.h
 * @brief One numeric component: ::lh_math_scalar_t.
 *
 * ::LH_LIBRARY_OPTION_MATH_FPU selects the underlying type. OFF keeps the
 * component on ::lh_int_t. ON switches it to ::lh_float_t.
 * ::lh_math_point_scalar_t is the point built from two of these.
 *
 * ::lh_math_scalar writes one number and lets the option finish it. The
 * argument is a numeric literal, not an expression: `lh_math_scalar(3)`,
 * `lh_math_scalar(1.5)`. ON makes that literal single precision, so a
 * `double` is never written. OFF casts to ::lh_int_t; a fraction truncates
 * toward zero.
 */

#ifndef LH_MATH_SCALAR_H
#define LH_MATH_SCALAR_H

#include <lh/cast/static.h>
#include <lh/config.h>
#include <lh/numeric/float.h>
#include <lh/numeric/types.h>

#if LH_LIBRARY_OPTION_MATH_FPU
typedef lh_float_t lh_math_scalar_t;
/**
 * @brief One ::lh_math_scalar_t from a numeric literal. ON: `n` with `f` pasted on.
 */
#    define lh_math_scalar(n) lh_cast_static(lh_math_scalar_t, n##e0f)
#else
typedef lh_int_t lh_math_scalar_t;
/**
 * @brief One ::lh_math_scalar_t from a numeric literal. OFF: cast to ::lh_int_t.
 */
#    define lh_math_scalar(n) lh_cast_static(lh_math_scalar_t, (n))
#endif

#endif /* LH_MATH_SCALAR_H */
