/**
 * @file scalar.h
 * @brief The screen coordinate of `lh/gl`: ::lh_gl_scalar_t.
 *
 * One integer for `x`, `y`, width and height. ::lh_gl_point_t, ::lh_gl_size_t
 * and ::lh_gl_rect_t are this value twice or four times. Pen thickness, a scroll
 * offset and a renderer band are the same number.
 *
 * The type is ::lh_math_scalar_t, a signed `int`. It does not follow
 * ::LH_LIBRARY_OPTION_MATH_FPU. Matrices and 3D vectors stay on
 * ::lh_math_fscalar_t and are not this scalar.
 */

#ifndef LH_GL_SCALAR_H
#define LH_GL_SCALAR_H

#include <lh/core/math/scalar.h>

/**
 * @typedef lh_gl_scalar_t
 * @brief One integer screen coordinate.
 */
typedef lh_math_scalar_t lh_gl_scalar_t;

/**
 * @def lh_gl_scalar(n)
 * @brief One ::lh_gl_scalar_t from a numeric literal (truncates toward zero).
 */
#define lh_gl_scalar(n) lh_math_scalar(n)

#endif /* LH_GL_SCALAR_H */
