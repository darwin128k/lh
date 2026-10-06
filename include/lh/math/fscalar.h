/**
 * @file fscalar.h
 * @brief Continuous 3D / FPU component: ::lh_math_fscalar_t (`float`).
 *
 * The scalar of float math: ::lh_math_vec2_t / ::lh_math_vec3_t /
 * ::lh_math_vec4_t, ::lh_math_quat_t, ::lh_math_mat4_t, and the float
 * 2D geometry (::lh_math_fpoint_t, …). Always ::lh_float_t — math
 * does not read ::LH_LIBRARY_OPTION_MATH_FPU here; that option only decides
 * whether these sources are built and what UI aliases through
 * <lh/ui/scalar.h>.
 */

#ifndef LH_MATH_FSCALAR_H
#define LH_MATH_FSCALAR_H

#include <lh/cast/static.h>
#include <lh/numeric/float.h>

/**
 * @typedef lh_math_fscalar_t
 * @brief Continuous component (`lh_float_t`).
 */
typedef lh_float_t lh_math_fscalar_t;

/**
 * @def lh_math_fscalar(n)
 * @brief One ::lh_math_fscalar_t from a numeric literal (`n` as single precision).
 */
#define lh_math_fscalar(n) lh_cast_static(lh_math_fscalar_t, n##e0f)

#endif /* LH_MATH_FSCALAR_H */
