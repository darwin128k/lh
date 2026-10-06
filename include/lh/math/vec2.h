/**
 * @file vec2.h
 * @brief 2-component float vector: `x, y`.
 *
 * Plain value type of lh's vector math, the same in every engine and API:
 * points, directions, colors. All functions take and return vectors by
 * value and never fail; nothing here knows about coordinate systems (which
 * axis is up, handedness) — those are a matter of the code using it.
 *
 * Layout is 2 consecutive ::lh_math_fscalar_t (`x, y`), the same as a C
 * `float[2]`.
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_vec2_get_*` / `lh_math_vec2_set_*` accessors. The struct is
 * defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_VEC2_H
#define LH_MATH_VEC2_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/float.h>
#include <lh/math/fscalar.h>
#include <lh/math/vec2/fields.h>
#include <lh/void.h>

/**
 * @struct lh_math_vec2
 * @typedef lh_math_vec2_t
 * @brief 2-component float vector.
 */
struct lh_math_vec2
{
    lh_math_vec2_fields(lh_math_fscalar_t);
};
typedef struct lh_math_vec2 lh_math_vec2_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the given components.
 */
lh_void
lh_math_vec2_init(lh_math_vec2_t *self, lh_math_fscalar_t x, lh_math_fscalar_t y);

/**
 * @brief Fill @p self with zero components.
 */
lh_void
lh_math_vec2_init_empty(lh_math_vec2_t *self);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X component of @p self.
 */
lh_math_fscalar_t
lh_math_vec2_get_x(const lh_math_vec2_t *self);

/**
 * @brief Y component of @p self.
 */
lh_math_fscalar_t
lh_math_vec2_get_y(const lh_math_vec2_t *self);

/**
 * @brief Set the X component of @p self.
 */
lh_void
lh_math_vec2_set_x(lh_math_vec2_t *self, lh_math_fscalar_t x);

/**
 * @brief Set the Y component of @p self.
 */
lh_void
lh_math_vec2_set_y(lh_math_vec2_t *self, lh_math_fscalar_t y);

/* ── Operations ─────────────────────────────────────────────────────────── */

/**
 * @brief Component-wise sum @p a + @p b.
 */
lh_math_vec2_t
lh_math_vec2_add(lh_math_vec2_t a, lh_math_vec2_t b);

/**
 * @brief Component-wise difference @p a - @p b.
 */
lh_math_vec2_t
lh_math_vec2_sub(lh_math_vec2_t a, lh_math_vec2_t b);

/**
 * @brief @p v with every component multiplied by @p s.
 */
lh_math_vec2_t
lh_math_vec2_scale(lh_math_vec2_t v, lh_math_fscalar_t s);

/**
 * @brief @p v with every component negated.
 */
lh_math_vec2_t
lh_math_vec2_neg(lh_math_vec2_t v);

/**
 * @brief Dot product: the sum of the component products.
 *
 * `|a| |b| cos(angle)`: 0 for perpendicular vectors, positive when they point
 * the same way.
 */
lh_math_fscalar_t
lh_math_vec2_dot(lh_math_vec2_t a, lh_math_vec2_t b);

/**
 * @brief Squared length, `dot(v, v)`.
 *
 * Cheaper than ::lh_math_vec2_length (no square root) and enough for comparing
 * lengths.
 */
lh_math_fscalar_t
lh_math_vec2_length_sq(lh_math_vec2_t v);

/**
 * @brief Euclidean length.
 */
lh_math_fscalar_t
lh_math_vec2_length(lh_math_vec2_t v);

/**
 * @brief @p v scaled to length 1, same direction.
 *
 * A zero vector has no direction and is returned unchanged (zero), not as
 * NaNs.
 */
lh_math_vec2_t
lh_math_vec2_normalize(lh_math_vec2_t v);

/**
 * @brief Linear interpolation: @p a at @p t = 0, @p b at @p t = 1.
 *
 * @p t is not clamped: values outside `[0, 1]` extrapolate along the line.
 */
lh_math_vec2_t
lh_math_vec2_lerp(lh_math_vec2_t a, lh_math_vec2_t b, lh_math_fscalar_t t);

/**
 * @brief Whether every component of @p a and @p b differs by at most @p eps.
 *
 * Floating-point results are rarely bit-identical; compare with a tolerance
 * suited to the magnitudes involved.
 */
lh_bool_t
lh_math_vec2_near(lh_math_vec2_t a, lh_math_vec2_t b, lh_math_fscalar_t eps);

/**
 * @brief Exact component-wise equality (bit-identical floats).
 */
lh_bool_t
lh_math_vec2_equals(lh_math_vec2_t self, lh_math_vec2_t other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_VEC2_H */