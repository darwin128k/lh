/**
 * @file quat.h
 * @brief Rotation quaternion: `x, y, z` (vector part), `w` (scalar part).
 *
 * A unit quaternion is a rotation in 3D: about a unit axis `a` by `angle`
 * radians it is `(a * sin(angle/2), cos(angle/2))`. Unlike Euler angles it
 * has no gimbal lock, composes with one ::lh_math_quat_mul and interpolates
 * smoothly (::lh_math_quat_nlerp), which is why engines keep orientations as
 * quaternions and turn them into a matrix (::lh_math_mat4_from_quat) only to draw.
 *
 * Rotations follow the right-hand rule in a right-handed coordinate system
 * (counterclockwise looking from the axis' tip toward the origin) and the
 * left-hand rule in a left-handed one; nothing here picks either.
 *
 * Layout is 4 consecutive ::lh_math_scalar_t in `x, y, z, w` order (GLM's and
 * most engines' storage order). All functions take and return by value.
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_quat_get_*` / `lh_math_quat_set_*` accessors. The struct is
 * defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_QUAT_H
#define LH_MATH_QUAT_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/float.h>
#include <lh/math/scalar.h>
#include <lh/math/quat/fields.h>
#include <lh/math/vec3.h>
#include <lh/math/vec4.h>
#include <lh/void.h>

/**
 * @struct lh_math_quat
 * @typedef lh_math_quat_t
 * @brief Rotation quaternion. Fields via ::lh_math_quat_fields.
 */
struct lh_math_quat
{
    lh_math_quat_fields(lh_math_scalar_t);
};
typedef struct lh_math_quat lh_math_quat_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Quaternion with the given components.
 */
lh_math_quat_t
lh_math_quat_make(lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t z, lh_math_scalar_t w);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X component (vector part) of @p self.
 */
lh_math_scalar_t
lh_math_quat_get_x(const lh_math_quat_t *self);

/**
 * @brief Y component (vector part) of @p self.
 */
lh_math_scalar_t
lh_math_quat_get_y(const lh_math_quat_t *self);

/**
 * @brief Z component (vector part) of @p self.
 */
lh_math_scalar_t
lh_math_quat_get_z(const lh_math_quat_t *self);

/**
 * @brief W component (scalar part) of @p self.
 */
lh_math_scalar_t
lh_math_quat_get_w(const lh_math_quat_t *self);

/**
 * @brief Set the X component of @p self.
 */
lh_void
lh_math_quat_set_x(lh_math_quat_t *self, lh_math_scalar_t x);

/**
 * @brief Set the Y component of @p self.
 */
lh_void
lh_math_quat_set_y(lh_math_quat_t *self, lh_math_scalar_t y);

/**
 * @brief Set the Z component of @p self.
 */
lh_void
lh_math_quat_set_z(lh_math_quat_t *self, lh_math_scalar_t z);

/**
 * @brief Set the W component of @p self.
 */
lh_void
lh_math_quat_set_w(lh_math_quat_t *self, lh_math_scalar_t w);

/**
 * @brief The same 4 numbers as a ::lh_math_vec4_t (`x, y, z, w`).
 *
 * For code that treats a quaternion as plain data: a shader uniform, a
 * vertex attribute, a keyframe array of 4-float tuples.
 */
lh_math_vec4_t
lh_math_quat_to_vec4(lh_math_quat_t q);

/**
 * @brief The quaternion with @p v's 4 numbers; see ::lh_math_quat_to_vec4.
 */
lh_math_quat_t
lh_math_quat_from_vec4(lh_math_vec4_t v);

/**
 * @brief No rotation: `(0, 0, 0, 1)`.
 */
lh_math_quat_t
lh_math_quat_identity(void);

/**
 * @brief Rotation about @p axis by @p angle radians.
 *
 * @param axis  Rotation axis; must be unit length (see ::lh_math_vec3_normalize).
 * @param angle Angle in radians.
 */
lh_math_quat_t
lh_math_quat_from_axis_angle(lh_math_vec3_t axis, lh_math_scalar_t angle);

/**
 * @brief Product @p a * @p b: the rotation @p b followed by @p a.
 *
 * Not commutative: `mul(a, b)` and `mul(b, a)` are different rotations in
 * general.
 */
lh_math_quat_t
lh_math_quat_mul(lh_math_quat_t a, lh_math_quat_t b);

/**
 * @brief Conjugate `(-x, -y, -z, w)`: for a unit quaternion, the inverse
 *        rotation.
 */
lh_math_quat_t
lh_math_quat_conjugate(lh_math_quat_t q);

/**
 * @brief 4D dot product. For unit quaternions, `|dot|` near 1 means nearly
 *        the same rotation.
 */
lh_math_scalar_t
lh_math_quat_dot(lh_math_quat_t a, lh_math_quat_t b);

/**
 * @brief @p q scaled to length 1.
 *
 * Products of many unit quaternions drift from length 1 through rounding;
 * normalizing now and then keeps them rotations. A zero quaternion is
 * returned unchanged.
 */
lh_math_quat_t
lh_math_quat_normalize(lh_math_quat_t q);

/**
 * @brief @p v rotated by the unit quaternion @p q.
 */
lh_math_vec3_t
lh_math_quat_rotate(lh_math_quat_t q, lh_math_vec3_t v);

/**
 * @brief Interpolate between the rotations @p a (at @p t = 0) and @p b
 *        (at @p t = 1) the short way round, normalized.
 *
 * `q` and `-q` are the same rotation; @p b is flipped when needed so the
 * result does not take the long way. The speed along the arc is not
 * constant (unlike slerp), which is invisible for the small steps between
 * animation frames.
 */
lh_math_quat_t
lh_math_quat_nlerp(lh_math_quat_t a, lh_math_quat_t b, lh_math_scalar_t t);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_QUAT_H */
