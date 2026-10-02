/**
 * @file vec2.h
 * @brief 2-component float vector: `x, y`.
 *
 * Plain value type of lh's vector math, the same in every engine and API:
 * points, directions, colors. All functions take and return vectors by
 * value and never fail; nothing here knows about coordinate systems (which
 * axis is up, handedness) — those are a matter of the code using it.
 *
 * Layout is 2 consecutive ::lh_float_t (`x, y`), the same as a C
 * `float[2]`.
 */

#ifndef LH_VEC2_H
#define LH_VEC2_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/float.h>
#include <lh/vec2/fields.h>

/**
 * @struct lh_vec2
 * @brief 2-component float vector.
 */
struct lh_vec2
{
    lh_vec2_fields(lh_float_t);
};
typedef struct lh_vec2 lh_vec2_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Vector with the given components.
 */
lh_vec2_t
lh_vec2_make(lh_float_t x, lh_float_t y);

/**
 * @brief Component-wise sum @p a + @p b.
 */
lh_vec2_t
lh_vec2_add(lh_vec2_t a, lh_vec2_t b);

/**
 * @brief Component-wise difference @p a - @p b.
 */
lh_vec2_t
lh_vec2_sub(lh_vec2_t a, lh_vec2_t b);

/**
 * @brief @p v with every component multiplied by @p s.
 */
lh_vec2_t
lh_vec2_scale(lh_vec2_t v, lh_float_t s);

/**
 * @brief @p v with every component negated.
 */
lh_vec2_t
lh_vec2_neg(lh_vec2_t v);

/**
 * @brief Dot product: the sum of the component products.
 *
 * `|a| |b| cos(angle)`: 0 for perpendicular vectors, positive when they point
 * the same way.
 */
lh_float_t
lh_vec2_dot(lh_vec2_t a, lh_vec2_t b);

/**
 * @brief Squared length, `dot(v, v)`.
 *
 * Cheaper than ::lh_vec2_length (no square root) and enough for comparing
 * lengths.
 */
lh_float_t
lh_vec2_length_sq(lh_vec2_t v);

/**
 * @brief Euclidean length.
 */
lh_float_t
lh_vec2_length(lh_vec2_t v);

/**
 * @brief @p v scaled to length 1, same direction.
 *
 * A zero vector has no direction and is returned unchanged (zero), not as
 * NaNs.
 */
lh_vec2_t
lh_vec2_normalize(lh_vec2_t v);

/**
 * @brief Linear interpolation: @p a at @p t = 0, @p b at @p t = 1.
 *
 * @p t is not clamped: values outside `[0, 1]` extrapolate along the line.
 */
lh_vec2_t
lh_vec2_lerp(lh_vec2_t a, lh_vec2_t b, lh_float_t t);

/**
 * @brief Whether every component of @p a and @p b differs by at most @p eps.
 *
 * Floating-point results are rarely bit-identical; compare with a tolerance
 * suited to the magnitudes involved.
 */
lh_bool_t
lh_vec2_near(lh_vec2_t a, lh_vec2_t b, lh_float_t eps);

LH_COMPILER_EXTERN_C_END

#endif /* LH_VEC2_H */
