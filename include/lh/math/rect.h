/**
 * @file rect.h
 * @brief An axis-aligned rectangle: ::lh_math_rect_t of an `origin` (top-left)
 *        plus a `size`, half-open.
 *
 * `width = 10` covers columns `[origin.x, origin.x + 10)`, i.e. `origin.x`
 * through `origin.x + 9`.
 *
 * Empty rectangles follow ::lh_math_size_is_empty on the embedded size:
 * ::lh_math_rect_is_empty and ::lh_math_rect_intersection share that
 * definition, so an "empty" output is canonical regardless of which
 * operation produced it.
 *
 * Accessors return pointers to the embedded `origin` and `size`; scalar
 * components live on ::lh_math_point_t / ::lh_math_size_t. The struct is
 * defined here only so those members can be embedded by value.
 */

#ifndef LH_MATH_RECT_H
#define LH_MATH_RECT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/scalar.h>
#include <lh/math/point.h>
#include <lh/math/rect/fields.h>
#include <lh/math/size.h>
#include <lh/void.h>

/**
 * @struct lh_math_rect
 * @typedef lh_math_rect_t
 * @brief An axis-aligned rectangle: `origin` plus `size`, half-open.
 */
struct lh_math_rect
{
    lh_math_rect_fields(lh_math_point_t, lh_math_size_t);
};
typedef struct lh_math_rect lh_math_rect_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── init / from ─────────────────────────────────────────────────────────── */

/**
 * @brief Build a `::lh_math_rect_t` from exclusive extent corners.
 *
 * @p max is exclusive (`origin + size`). Empty size yields an empty rectangle.
 */
lh_math_rect_t
lh_math_rect_from_extent(const lh_math_point_t *min, const lh_math_point_t *max);

/**
 * @brief Build a `::lh_math_rect_t` from min/max scalars (Win32-style:
 *        left, top, right, bottom — right/bottom are exclusive).
 */
lh_math_rect_t
lh_math_rect_from_min_max(lh_math_scalar_t x_min, lh_math_scalar_t y_min,
                          lh_math_scalar_t x_max, lh_math_scalar_t y_max);

/**
 * @brief Fill @p self from origin coordinates and extents.
 */
lh_void
lh_math_rect_init(lh_math_rect_t *self, lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t width,
                  lh_math_scalar_t height);

/**
 * @brief Fill @p self from @p origin and @p size.
 */
lh_void
lh_math_rect_init_origin_size(lh_math_rect_t *self, lh_math_point_t origin, lh_math_size_t size);

/**
 * @brief Fill @p self with the empty rectangle sentinel `(0, 0)` size.
 */
lh_void
lh_math_rect_init_empty(lh_math_rect_t *self);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Pointer to the top-left corner of @p self.
 */
lh_math_point_t *
lh_math_rect_get_origin(lh_math_rect_t *self);

/**
 * @brief Const pointer to the top-left corner of @p self.
 */
const lh_math_point_t *
lh_math_rect_get_origin_as_const(const lh_math_rect_t *self);

/**
 * @brief Pointer to the size of @p self.
 */
lh_math_size_t *
lh_math_rect_get_size(lh_math_rect_t *self);

/**
 * @brief Const pointer to the size of @p self.
 */
const lh_math_size_t *
lh_math_rect_get_size_as_const(const lh_math_rect_t *self);

/**
 * @brief Replace the origin of @p self with @p origin.
 */
lh_void
lh_math_rect_set_origin(lh_math_rect_t *self, lh_math_point_t origin);

/**
 * @brief Replace the size of @p self with @p size.
 */
lh_void
lh_math_rect_set_size(lh_math_rect_t *self, lh_math_size_t size);

/**
 * @brief Exclusive far corner: origin offset by size.
 */
lh_math_point_t
lh_math_rect_far(const lh_math_rect_t *self);

/**
 * @brief Element-wise minimum of the origins of @p a and @p b.
 */
lh_math_point_t
lh_math_rect_origin_min(const lh_math_rect_t *a, const lh_math_rect_t *b);

/**
 * @brief Element-wise maximum of the origins of @p a and @p b.
 */
lh_math_point_t
lh_math_rect_origin_max(const lh_math_rect_t *a, const lh_math_rect_t *b);

/**
 * @brief Element-wise minimum of the exclusive far corners of @p a and @p b.
 */
lh_math_point_t
lh_math_rect_far_min(const lh_math_rect_t *a, const lh_math_rect_t *b);

/**
 * @brief Element-wise maximum of the exclusive far corners of @p a and @p b.
 */
lh_math_point_t
lh_math_rect_far_max(const lh_math_rect_t *a, const lh_math_rect_t *b);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Test whether @p self is empty (::lh_math_size_is_empty on its size).
 */
lh_bool_t
lh_math_rect_is_empty(const lh_math_rect_t *self);

/**
 * @brief Test whether @p self covers @p point (inclusive at top/left,
 *        exclusive at bottom/right).
 */
lh_bool_t
lh_math_rect_contains_point(const lh_math_rect_t *self, lh_math_point_t point);

/**
 * @brief Test whether two rectangles share any non-empty area.
 */
lh_bool_t
lh_math_rect_intersects(const lh_math_rect_t *a, const lh_math_rect_t *b);

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_rect_eq(const lh_math_rect_t *a, const lh_math_rect_t *b);

/**
 * @brief True if @p self and @p other have the same origin and size.
 */
lh_bool_t
lh_math_rect_equals(const lh_math_rect_t *self, const lh_math_rect_t *other);

/**
 * @brief True if @p self is not lexicographically earlier than @p minimum.
 *
 * Order: origin (::lh_math_point_is_at_least), then size (::lh_math_size_is_at_least).
 */
lh_bool_t
lh_math_rect_is_at_least(const lh_math_rect_t *self, const lh_math_rect_t *minimum);

/**
 * @brief True if @p self is strictly lexicographically earlier than @p other.
 *
 * Same field order as ::lh_math_rect_is_at_least.
 */
lh_bool_t
lh_math_rect_is_less(const lh_math_rect_t *self, const lh_math_rect_t *other);

/**
 * @brief True if @p self is strictly lexicographically later than @p other.
 *
 * Same field order as ::lh_math_rect_is_at_least.
 */
lh_bool_t
lh_math_rect_is_greater(const lh_math_rect_t *self, const lh_math_rect_t *other);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Intersection of @p a and @p b. Returns ::lh_math_rect_init_empty if they do
 *        not overlap.
 */
lh_math_rect_t
lh_math_rect_intersection(const lh_math_rect_t *a, const lh_math_rect_t *b);

/**
 * @brief Union of @p a and @p b: the smallest rectangle containing both.
 *        Empty @p a or @p b returns the other.
 */
lh_math_rect_t
lh_math_rect_union(const lh_math_rect_t *a, const lh_math_rect_t *b);

/**
 * @brief Translate @p self by `(@p dx, @p dy)`.
 */
lh_math_rect_t
lh_math_rect_offset(const lh_math_rect_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy);

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side (positive shrinks,
 *        negative grows).
 */
lh_math_rect_t
lh_math_rect_inset(const lh_math_rect_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_RECT_H */
