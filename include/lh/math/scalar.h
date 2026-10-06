/**
 * @file scalar.h
 * @brief Continuous 3D / FPU component: ::lh_math_scalar_t (`float`).
 *
 * The scalar of float math: ::lh_math_vec2_t / ::lh_math_vec3_t /
 * ::lh_math_vec4_t, ::lh_math_quat_t, ::lh_math_mat4_t, and the float
 * 2D geometry (::lh_math_fpoint_t, …). Always ::lh_float_t — math
 * does not read ::LH_LIBRARY_OPTION_MATH_FPU here; that option only decides
 * whether these sources are built and what UI aliases through
 * <lh/ui/scalar.h>.
 */

#ifndef LH_MATH_SCALAR_H
#define LH_MATH_SCALAR_H

#include <lh/cast/static.h>
#include <lh/numeric/float.h>

/**
 * @typedef lh_math_scalar_t
 * @brief Continuous component (`lh_float_t`).
 */
typedef lh_float_t lh_math_scalar_t;

/**
 * @def lh_math_scalar(n)
 * @brief One ::lh_math_scalar_t from a numeric literal (`n` as single precision).
 */
#define lh_math_scalar(n) lh_cast_static(lh_math_scalar_t, n##e0f)

#endif /* LH_MATH_SCALAR_H */
