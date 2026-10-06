/**
 * @file vec4.h
 * @brief 4-component float vector: `x, y, z, w`.
 *
 * Plain value type of lh's vector math, the same in every engine and API:
 * points, directions, colors with alpha. All functions take and return vectors by
 * value and never fail; nothing here knows about coordinate systems (which
 * axis is up, handedness) — those are a matter of the code using it.
 *
 * Layout is 4 consecutive ::lh_math_scalar_t (`x, y, z, w`), the same as a C
 * `float[4]`.
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_vec4_get_*` / `lh_math_vec4_set_*` accessors. The struct is
 * defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_VEC4_H
#define LH_MATH_VEC4_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/float.h>
#include <lh/math/scalar.h>
#include <lh/math/vec3.h>
#include <lh/math/vec4/fields.h>
#include <lh/void.h>

/**
 * @struct lh_math_vec4
 * @typedef lh_math_vec4_t
 * @brief 4-component float vector.
 */
struct lh_math_vec4
{
    lh_math_vec4_fields(lh_math_scalar_t);
};
typedef struct lh_math_vec4 lh_math_vec4_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Vector with the given components.
 */
lh_math_vec4_t
lh_math_vec4_make(lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t z, lh_math_scalar_t w);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X component of @p self.
 */
lh_math_scalar_t
lh_math_vec4_get_x(const lh_math_vec4_t *self);

/**
 * @brief Y component of @p self.
 */
lh_math_scalar_t
lh_math_vec4_get_y(const lh_math_vec4_t *self);

/**
 * @brief Z component of @p self.
 */
lh_math_scalar_t
lh_math_vec4_get_z(const lh_math_vec4_t *self);

/**
 * @brief W component of @p self.
 */
lh_math_scalar_t
lh_math_vec4_get_w(const lh_math_vec4_t *self);

/**
 * @brief Set the X component of @p self.
 */
lh_void
lh_math_vec4_set_x(lh_math_vec4_t *self, lh_math_scalar_t x);

/**
 * @brief Set the Y component of @p self.
 */
lh_void
lh_math_vec4_set_y(lh_math_vec4_t *self, lh_math_scalar_t y);

/**
 * @brief Set the Z component of @p self.
 */
lh_void
lh_math_vec4_set_z(lh_math_vec4_t *self, lh_math_scalar_t z);

/**
 * @brief Set the W component of @p self.
 */
lh_void
lh_math_vec4_set_w(lh_math_vec4_t *self, lh_math_scalar_t w);

/* ── Conversions ─────────────────────────────────────────────────────────── */

/**
 * @brief Widen @p v to homogeneous space with `w = @p w`.
 *
 * @param v Vector to widen.
 * @param w Homogeneous component; `1.0f` for a point, `0.0f` for a direction.
 * @return `(v.x, v.y, v.z, w)`.
 */
lh_math_vec4_t
lh_math_vec3_to_vec4(lh_math_vec3_t v, lh_math_scalar_t w);

/**
 * @brief Narrow @p v from homogeneous space, dropping `w`.
 *
 * Assumes a point, not a direction: for a projective transform the result
 * must be divided by `w` first.
 *
 * @param v Vector to narrow.
 * @return `(v.x, v.y, v.z)`.
 */
lh_math_vec3_t
lh_math_vec4_to_vec3(lh_math_vec4_t v);

/**
 * @brief Component-wise sum @p a + @p b.
 */
lh_math_vec4_t
lh_math_vec4_add(lh_math_vec4_t a, lh_math_vec4_t b);

/**
 * @brief Component-wise difference @p a - @p b.
 */
lh_math_vec4_t
lh_math_vec4_sub(lh_math_vec4_t a, lh_math_vec4_t b);

/**
 * @brief @p v with every component multiplied by @p s.
 */
lh_math_vec4_t
lh_math_vec4_scale(lh_math_vec4_t v, lh_math_scalar_t s);

/**
 * @brief @p v with every component negated.
 */
lh_math_vec4_t
lh_math_vec4_neg(lh_math_vec4_t v);

/**
 * @brief Dot product: the sum of the component products.
 *
 * `|a| |b| cos(angle)`: 0 for perpendicular vectors, positive when they point
 * the same way.
 */
lh_math_scalar_t
lh_math_vec4_dot(lh_math_vec4_t a, lh_math_vec4_t b);

/**
 * @brief Squared length, `dot(v, v)`.
 *
 * Cheaper than ::lh_math_vec4_length (no square root) and enough for comparing
 * lengths.
 */
lh_math_scalar_t
lh_math_vec4_length_sq(lh_math_vec4_t v);

/**
 * @brief Euclidean length.
 */
lh_math_scalar_t
lh_math_vec4_length(lh_math_vec4_t v);

/**
 * @brief @p v scaled to length 1, same direction.
 *
 * A zero vector has no direction and is returned unchanged (zero), not as
 * NaNs.
 */
lh_math_vec4_t
lh_math_vec4_normalize(lh_math_vec4_t v);

/**
 * @brief Linear interpolation: @p a at @p t = 0, @p b at @p t = 1.
 *
 * @p t is not clamped: values outside `[0, 1]` extrapolate along the line.
 */
lh_math_vec4_t
lh_math_vec4_lerp(lh_math_vec4_t a, lh_math_vec4_t b, lh_math_scalar_t t);

/**
 * @brief Whether every component of @p a and @p b differs by at most @p eps.
 *
 * Floating-point results are rarely bit-identical; compare with a tolerance
 * suited to the magnitudes involved.
 */
lh_bool_t
lh_math_vec4_near(lh_math_vec4_t a, lh_math_vec4_t b, lh_math_scalar_t eps);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_VEC4_H */
