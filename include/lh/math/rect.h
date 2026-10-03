/**
 * @file rect.h
 * @brief An axis-aligned rectangle: ::lh_math_rect_t of an `origin` (top-left)
 *        plus a `size`, half-open.
 *
 * `width = 10` covers columns `[origin.x, origin.x + 10)`, i.e. `origin.x`
 * through `origin.x + 9`.
 *
 * Empty rectangles are represented as `size.width <= 0 || size.height <= 0`:
 * ::lh_math_rect_is_empty, ::lh_math_rect_intersection, and
 * ::lh_math_rect_width / ::lh_math_rect_height all share that definition, so
 * an "empty" output is canonical regardless of which operation produced it.
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_rect_get_*` / `lh_math_rect_set_*` accessors. The struct is
 * defined here only so `origin` and `size` can be embedded by value.
 */

#ifndef LH_MATH_RECT_H
#define LH_MATH_RECT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/coord.h>
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

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_rect_t` from origin coordinates and extents.
 */
lh_math_rect_t
lh_math_rect_make(lh_math_coord_t x, lh_math_coord_t y, lh_math_coord_t width, lh_math_coord_t height);

/**
 * @brief Make a `::lh_math_rect_t` from min/max corners (Win32-style:
 *        left, top, right, bottom — right/bottom are exclusive).
 */
lh_math_rect_t
lh_math_rect_from_min_max(lh_math_coord_t x_min, lh_math_coord_t y_min,
                          lh_math_coord_t x_max, lh_math_coord_t y_max);

/**
 * @brief The "no rectangle" sentinel: origin `(0, 0)`, size `(0, 0)`.
 *        ::lh_math_rect_is_empty returns ::lh_bool_true for this value.
 */
lh_math_rect_t
lh_math_rect_zero(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Top-left corner of @p self as a ::lh_math_point_t.
 */
lh_math_point_t
lh_math_rect_get_origin(const lh_math_rect_t *self);

/**
 * @brief Size of @p self as a ::lh_math_size_t.
 */
lh_math_size_t
lh_math_rect_get_size(const lh_math_rect_t *self);

/**
 * @brief Top-left corner of @p self as a ::lh_math_point_t. Handles @c NULL.
 */
lh_math_point_t
lh_math_rect_origin(const lh_math_rect_t *self);

/**
 * @brief X coordinate of the origin of @p self.
 */
lh_math_coord_t
lh_math_rect_get_x(const lh_math_rect_t *self);

/**
 * @brief Y coordinate of the origin of @p self.
 */
lh_math_coord_t
lh_math_rect_get_y(const lh_math_rect_t *self);

/**
 * @brief Width component of the size of @p self. Zero for empty rectangles.
 */
lh_math_coord_t
lh_math_rect_get_size_width(const lh_math_rect_t *self);

/**
 * @brief Height component of the size of @p self. Zero for empty rectangles.
 */
lh_math_coord_t
lh_math_rect_get_size_height(const lh_math_rect_t *self);

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

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self as a ::lh_math_coord_t. Zero for empty rectangles.
 */
lh_math_coord_t
lh_math_rect_width(const lh_math_rect_t *self);

/**
 * @brief Height of @p self as a ::lh_math_coord_t. Zero for empty rectangles.
 */
lh_math_coord_t
lh_math_rect_height(const lh_math_rect_t *self);

/**
 * @brief Test whether @p self is "empty" (zero or negative extent).
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

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Intersection of @p a and @p b. Returns ::lh_math_rect_zero if they do
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
lh_math_rect_offset(const lh_math_rect_t *self, lh_math_coord_t dx, lh_math_coord_t dy);

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side (positive shrinks,
 *        negative grows).
 */
lh_math_rect_t
lh_math_rect_inset(const lh_math_rect_t *self, lh_math_coord_t dx, lh_math_coord_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_RECT_H */