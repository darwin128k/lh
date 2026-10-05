/**
 * @file scalar.h
 * @brief One numeric component: ::lh_math_scalar_t.
 *
 * ::LH_LIBRARY_OPTION_MATH_FPU selects the underlying type. OFF keeps the
 * component on ::lh_int_t. ON switches it to ::lh_float_t.
 */

#ifndef LH_MATH_SCALAR_H
#define LH_MATH_SCALAR_H

#include <lh/config.h>
#include <lh/numeric/float.h>
#include <lh/numeric/types.h>

#if LH_LIBRARY_OPTION_MATH_FPU
typedef lh_float_t lh_math_scalar_t;
#else
typedef lh_int_t lh_math_scalar_t;
#endif

#endif /* LH_MATH_SCALAR_H */
