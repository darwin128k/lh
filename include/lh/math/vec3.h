/**
 * @file vec3.h
 * @brief 3-component float vector: `x, y, z`.
 *
 * Plain value type of lh's vector math, the same in every engine and API:
 * points, directions, colors. All functions take and return vectors by
 * value and never fail; nothing here knows about coordinate systems (which
 * axis is up, handedness) — those are a matter of the code using it.
 *
 * Layout is 3 consecutive ::lh_float_t (`x, y, z`), the same as a C
 * `float[3]`, e.g. GoldSrc's `vec3_t`.
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_vec3_get_*` / `lh_math_vec3_set_*` accessors. The struct is
 * defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_VEC3_H
#define LH_MATH_VEC3_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/float.h>
#include <lh/math/vec3/fields.h>
#include <lh/void.h>

/**
 * @struct lh_math_vec3
 * @typedef lh_math_vec3_t
 * @brief 3-component float vector.
 */
struct lh_math_vec3
{
    lh_math_vec3_fields(lh_float_t);
};
typedef struct lh_math_vec3 lh_math_vec3_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Vector with the given components.
 */
lh_math_vec3_t
lh_math_vec3_make(lh_float_t x, lh_float_t y, lh_float_t z);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X component of @p self.
 */
lh_float_t
lh_math_vec3_get_x(const lh_math_vec3_t *self);

/**
 * @brief Y component of @p self.
 */
lh_float_t
lh_math_vec3_get_y(const lh_math_vec3_t *self);

/**
 * @brief Z component of @p self.
 */
lh_float_t
lh_math_vec3_get_z(const lh_math_vec3_t *self);

/**
 * @brief Set the X component of @p self.
 */
lh_void
lh_math_vec3_set_x(lh_math_vec3_t *self, lh_float_t x);

/**
 * @brief Set the Y component of @p self.
 */
lh_void
lh_math_vec3_set_y(lh_math_vec3_t *self, lh_float_t y);

/**
 * @brief Set the Z component of @p self.
 */
lh_void
lh_math_vec3_set_z(lh_math_vec3_t *self, lh_float_t z);

/**
 * @brief Component-wise sum @p a + @p b.
 */
lh_math_vec3_t
lh_math_vec3_add(lh_math_vec3_t a, lh_math_vec3_t b);

/**
 * @brief Component-wise difference @p a - @p b.
 */
lh_math_vec3_t
lh_math_vec3_sub(lh_math_vec3_t a, lh_math_vec3_t b);

/**
 * @brief @p v with every component multiplied by @p s.
 */
lh_math_vec3_t
lh_math_vec3_scale(lh_math_vec3_t v, lh_float_t s);

/**
 * @brief @p v with every component negated.
 */
lh_math_vec3_t
lh_math_vec3_neg(lh_math_vec3_t v);

/**
 * @brief Dot product: the sum of the component products.
 *
 * `|a| |b| cos(angle)`: 0 for perpendicular vectors, positive when they point
 * the same way.
 */
lh_float_t
lh_math_vec3_dot(lh_math_vec3_t a, lh_math_vec3_t b);

/**
 * @brief Squared length, `dot(v, v)`.
 *
 * Cheaper than ::lh_math_vec3_length (no square root) and enough for comparing
 * lengths.
 */
lh_float_t
lh_math_vec3_length_sq(lh_math_vec3_t v);

/**
 * @brief Euclidean length.
 */
lh_float_t
lh_math_vec3_length(lh_math_vec3_t v);

/**
 * @brief @p v scaled to length 1, same direction.
 *
 * A zero vector has no direction and is returned unchanged (zero), not as
 * NaNs.
 */
lh_math_vec3_t
lh_math_vec3_normalize(lh_math_vec3_t v);

/**
 * @brief Linear interpolation: @p a at @p t = 0, @p b at @p t = 1.
 *
 * @p t is not clamped: values outside `[0, 1]` extrapolate along the line.
 */
lh_math_vec3_t
lh_math_vec3_lerp(lh_math_vec3_t a, lh_math_vec3_t b, lh_float_t t);

/**
 * @brief Whether every component of @p a and @p b differs by at most @p eps.
 *
 * Floating-point results are rarely bit-identical; compare with a tolerance
 * suited to the magnitudes involved.
 */
lh_bool_t
lh_math_vec3_near(lh_math_vec3_t a, lh_math_vec3_t b, lh_float_t eps);

/**
 * @brief Cross product @p a × @p b: perpendicular to both, of length
 *        `|a| |b| sin(angle)`.
 *
 * The standard component formula: the result follows the right-hand rule in
 * a right-handed coordinate system (and the left-hand rule in a left-handed
 * one). Anticommutative: `cross(b, a) == -cross(a, b)`.
 */
lh_math_vec3_t
lh_math_vec3_cross(lh_math_vec3_t a, lh_math_vec3_t b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_VEC3_H */
