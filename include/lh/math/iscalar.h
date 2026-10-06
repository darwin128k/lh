/**
 * @file iscalar.h
 * @brief Discrete 2D component: ::lh_math_iscalar_t (`int`).
 *
 * The scalar of screen / window geometry (::lh_math_point_t,
 * ::lh_math_size_t, ::lh_math_rect_t, and ::lh_math_point3_t).
 * Always signed `int` — math does not read ::LH_LIBRARY_OPTION_MATH_FPU here.
 * UI may alias this type through <lh/ui/scalar.h> when FPU is OFF.
 */

#ifndef LH_MATH_ISCALAR_H
#define LH_MATH_ISCALAR_H

#include <lh/cast/static.h>
#include <lh/numeric/types.h>

/**
 * @typedef lh_math_iscalar_t
 * @brief Discrete 2D component (`lh_int_t`).
 */
typedef lh_int_t lh_math_iscalar_t;

/**
 * @def lh_math_iscalar(n)
 * @brief One ::lh_math_iscalar_t from a numeric literal (truncates toward zero).
 */
#define lh_math_iscalar(n) lh_cast_static(lh_math_iscalar_t, (n))

#endif /* LH_MATH_ISCALAR_H */
