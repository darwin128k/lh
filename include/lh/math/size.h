/**
 * @file size.h
 * @brief A 2D size: ::lh_math_size_t with ::lh_math_scalar_t `width`, `height`.
 *
 * Negative extents are valid bit patterns but ::lh_math_size_is_empty treats
 * zero or negative extent as empty — used to represent "nothing" without a
 * sentinel (shared with ::lh_math_rect_is_empty).
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_size_get_*` / `lh_math_size_set_*` accessors. The struct is
 * defined here only so it can be embedded by value (e.g. as
 * `lh_math_rect_t::size`).
 */

#ifndef LH_MATH_SIZE_H
#define LH_MATH_SIZE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/scalar.h>
#include <lh/math/point.h>
#include <lh/math/size/fields.h>
#include <lh/void.h>

/**
 * @struct lh_math_size
 * @typedef lh_math_size_t
 * @brief A 2D size: positive `width` and `height`.
 */
struct lh_math_size
{
    lh_math_size_fields(lh_math_scalar_t);
};
typedef struct lh_math_size lh_math_size_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Fill @p self from explicit extents.
 */
lh_void
lh_math_size_init(lh_math_size_t *self, lh_math_scalar_t width, lh_math_scalar_t height);

/**
 * @brief Fill @p self with the empty size `(0, 0)`.
 */
lh_void
lh_math_size_init_empty(lh_math_size_t *self);

/**
 * @brief Size spanning from @p min (inclusive) to @p max (exclusive).
 */
lh_math_size_t
lh_math_size_from_extent(const lh_math_point_t *min, const lh_math_point_t *max);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self.
 */
lh_math_scalar_t
lh_math_size_get_width(const lh_math_size_t *self);

/**
 * @brief Height of @p self.
 */
lh_math_scalar_t
lh_math_size_get_height(const lh_math_size_t *self);

/**
 * @brief Set the width of @p self.
 */
lh_void
lh_math_size_set_width(lh_math_size_t *self, lh_math_scalar_t width);

/**
 * @brief Set the height of @p self.
 */
lh_void
lh_math_size_set_height(lh_math_size_t *self, lh_math_scalar_t height);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_size_eq(const lh_math_size_t *a, const lh_math_size_t *b);

/**
 * @brief True if @p self and @p other have the same `width` and `height`.
 */
lh_bool_t
lh_math_size_equals(const lh_math_size_t *self, const lh_math_size_t *other);

/**
 * @brief True if @p self is not lexicographically earlier than @p minimum.
 *
 * Order: `width`, then `height`.
 */
lh_bool_t
lh_math_size_is_at_least(const lh_math_size_t *self, const lh_math_size_t *minimum);

/**
 * @brief True if @p self is strictly lexicographically earlier than @p other.
 *
 * Same field order as ::lh_math_size_is_at_least.
 */
lh_bool_t
lh_math_size_is_less(const lh_math_size_t *self, const lh_math_size_t *other);

/**
 * @brief True if @p self is strictly lexicographically later than @p other.
 *
 * Same field order as ::lh_math_size_is_at_least.
 */
lh_bool_t
lh_math_size_is_greater(const lh_math_size_t *self, const lh_math_size_t *other);

/**
 * @brief Test whether @p self is empty (zero or negative extent).
 */
lh_bool_t
lh_math_size_is_empty(const lh_math_size_t *self);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side (positive shrinks,
 *        negative grows): width `-= 2 * dx`, height `-= 2 * dy`.
 */
lh_math_size_t
lh_math_size_inset(const lh_math_size_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_SIZE_H */
