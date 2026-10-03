/**
 * @file mat4.h
 * @brief 4x4 float matrix: affine and projective transforms of 3D points.
 *
 * Column vectors: a matrix transforms `v` as `M * v`, and `mul(a, b)` is
 * "apply @p b, then @p a" (the same order as ::lh_math_quat_mul). A transform
 * that scales, then rotates, then moves is `mul(T, mul(R, S))`.
 *
 * Column-major layout: four ::lh_math_vec4_t columns, i.e. 16 consecutive
 * ::lh_float_t column by column. That is what OpenGL / GLM / Vulkan take as
 * is; Direct3D's row-vector convention reads the same bytes as the
 * transpose, which is the same transform written its way.
 *
 * Like the vectors, nothing here knows about handedness or which axis is
 * up.
 */

#ifndef LH_MATH_MAT4_H
#define LH_MATH_MAT4_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/mat4/fields.h>
#include <lh/math/quat.h>
#include <lh/math/vec3.h>
#include <lh/math/vec4.h>

/**
 * @struct lh_mat4
 * @brief 4x4 float matrix. Fields via ::lh_math_mat4_fields.
 */
struct lh_mat4
{
    lh_math_mat4_fields(lh_math_vec4_t);
};
typedef struct lh_mat4 lh_math_mat4_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The identity: transforms every vector to itself.
 */
lh_math_mat4_t
lh_math_mat4_identity(void);

/**
 * @brief Matrix with the given columns.
 */
lh_math_mat4_t
lh_math_mat4_from_columns(lh_math_vec4_t c0, lh_math_vec4_t c1, lh_math_vec4_t c2, lh_math_vec4_t c3);

/**
 * @brief Moves points by @p offset; directions are unaffected.
 */
lh_math_mat4_t
lh_math_mat4_from_translation(lh_math_vec3_t offset);

/**
 * @brief Scales each axis by the matching component of @p factors.
 */
lh_math_mat4_t
lh_math_mat4_from_scale(lh_math_vec3_t factors);

/**
 * @brief The rotation of the unit quaternion @p q.
 */
lh_math_mat4_t
lh_math_mat4_from_quat(lh_math_quat_t q);

/**
 * @brief Product @p a * @p b: the transform @p b followed by @p a.
 */
lh_math_mat4_t
lh_math_mat4_mul(lh_math_mat4_t a, lh_math_mat4_t b);

/**
 * @brief @p m * @p v.
 */
lh_math_vec4_t
lh_math_mat4_mul_vec4(lh_math_mat4_t m, lh_math_vec4_t v);

/**
 * @brief Transform the point @p p (`w = 1`): translation applies.
 *
 * The resulting `w` is dropped without a perspective divide, which is
 * exact for affine matrices (bottom row `0 0 0 1`); for a projection use
 * ::lh_math_mat4_mul_vec4 and divide by `w` yourself.
 */
lh_math_vec3_t
lh_math_mat4_transform_point(lh_math_mat4_t m, lh_math_vec3_t p);

/**
 * @brief Transform the direction @p d (`w = 0`): translation does not
 *        apply.
 */
lh_math_vec3_t
lh_math_mat4_transform_dir(lh_math_mat4_t m, lh_math_vec3_t d);

/**
 * @brief Rows and columns swapped.
 *
 * For a pure rotation this is also its inverse, much cheaper than
 * ::lh_math_mat4_inverse.
 */
lh_math_mat4_t
lh_math_mat4_transpose(lh_math_mat4_t m);

/**
 * @brief Inverse of @p m: the transform that undoes it.
 *
 * @param m   Matrix to invert.
 * @param out Receives the inverse; left untouched when @p m is singular.
 * @return False when @p m has no inverse (determinant 0: it flattens space
 *         onto a plane, line or point).
 */
lh_bool_t
lh_math_mat4_inverse(lh_math_mat4_t m, lh_math_mat4_t *out);

/**
 * @brief Whether every element of @p a and @p b differs by at most @p eps.
 */
lh_bool_t
lh_math_mat4_near(lh_math_mat4_t a, lh_math_mat4_t b, lh_float_t eps);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_MAT4_H */
