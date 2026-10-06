/**
 * @file fpoint.h
 * @brief A 2D point whose components are ::lh_math_fscalar_t.
 *
 * Same shape as ::lh_math_point_t (`x`, `y`), with continuous coordinates.
 * The screen point stays ::lh_math_point_t.
 * ::lh_math_fpoint_to_point narrows by cast (truncates toward zero).
 * ::lh_math_point_to_fpoint widens the other way.
 *
 * Built only when ::LH_LIBRARY_OPTION_MATH_FPU is ON. UI picks this type or
 * the integer ::lh_math_point_t through <lh/ui/point.h>.
 *
 * Fields are not part of the public API: read and mutate them through the
 * accessors. The struct is defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_FPOINT_H
#define LH_MATH_FPOINT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/point.h>
#include <lh/math/point/fields.h>
#include <lh/math/fscalar.h>
#include <lh/void.h>

struct lh_math_fsize;

/**
 * @struct lh_math_fpoint
 * @typedef lh_math_fpoint_t
 * @brief A 2D point of ::lh_math_fscalar_t components.
 */
struct lh_math_fpoint
{
    lh_math_point_fields(lh_math_fscalar_t);
};
typedef struct lh_math_fpoint lh_math_fpoint_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self from components.
 */
lh_void
lh_math_fpoint_init(lh_math_fpoint_t *self, lh_math_fscalar_t x, lh_math_fscalar_t y);

/**
 * @brief Fill @p self with zero components.
 */
lh_void
lh_math_fpoint_init_empty(lh_math_fpoint_t *self);


/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X component of @p self.
 */
lh_math_fscalar_t
lh_math_fpoint_get_x(const lh_math_fpoint_t *self);

/**
 * @brief Y component of @p self.
 */
lh_math_fscalar_t
lh_math_fpoint_get_y(const lh_math_fpoint_t *self);

/**
 * @brief Set the X component of @p self.
 */
lh_void
lh_math_fpoint_set_x(lh_math_fpoint_t *self, lh_math_fscalar_t x);

/**
 * @brief Set the Y component of @p self.
 */
lh_void
lh_math_fpoint_set_y(lh_math_fpoint_t *self, lh_math_fscalar_t y);

/* ── Conversions ─────────────────────────────────────────────────────────── */

/**
 * @brief Narrow @p self to a screen point.
 *
 * Casts each component to ::lh_math_scalar_t (truncates toward zero).
 */
lh_math_point_t
lh_math_fpoint_to_point(lh_math_fpoint_t self);

/**
 * @brief Widen @p self to a continuous point.
 *
 * Casts each ::lh_math_scalar_t to ::lh_math_fscalar_t.
 */
lh_math_fpoint_t
lh_math_point_to_fpoint(lh_math_point_t self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_fpoint_eq(const lh_math_fpoint_t *a, const lh_math_fpoint_t *b);

/**
 * @brief True if @p self and @p other have the same `x` and `y`.
 */
lh_bool_t
lh_math_fpoint_equals(const lh_math_fpoint_t *self, const lh_math_fpoint_t *other);

/**
 * @brief True if @p self is not lexicographically earlier than @p minimum.
 *
 * Order: `x`, then `y`.
 */
lh_bool_t
lh_math_fpoint_is_at_least(const lh_math_fpoint_t *self, const lh_math_fpoint_t *minimum);

/**
 * @brief True if @p self is strictly lexicographically earlier than @p other.
 *
 * Same field order as ::lh_math_fpoint_is_at_least.
 */
lh_bool_t
lh_math_fpoint_is_less(const lh_math_fpoint_t *self, const lh_math_fpoint_t *other);

/**
 * @brief True if @p self is strictly lexicographically later than @p other.
 *
 * Same field order as ::lh_math_fpoint_is_at_least.
 */
lh_bool_t
lh_math_fpoint_is_greater(const lh_math_fpoint_t *self, const lh_math_fpoint_t *other);

/**
 * @brief Element-wise minimum of @p a and @p b.
 */
lh_math_fpoint_t
lh_math_fpoint_min(const lh_math_fpoint_t *a, const lh_math_fpoint_t *b);

/**
 * @brief Element-wise maximum of @p a and @p b.
 */
lh_math_fpoint_t
lh_math_fpoint_max(const lh_math_fpoint_t *a, const lh_math_fpoint_t *b);

/**
 * @brief Test whether @p self lies in the half-open extent `[@p min, @p max)`.
 */
lh_bool_t
lh_math_fpoint_in_extent(const lh_math_fpoint_t *self, const lh_math_fpoint_t *min,
                         const lh_math_fpoint_t *max);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Translate @p self by `(@p dx, @p dy)`.
 */
lh_math_fpoint_t
lh_math_fpoint_offset(const lh_math_fpoint_t *self, lh_math_fscalar_t dx, lh_math_fscalar_t dy);

/**
 * @brief Exclusive far corner: @p self offset by @p size (`x + width`, `y + height`).
 */
lh_math_fpoint_t
lh_math_fpoint_offset_size(const lh_math_fpoint_t *self, const struct lh_math_fsize *size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FPOINT_H */
