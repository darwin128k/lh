/**
 * @file geom.h
 * @brief OS-portable 2D geometry: point, size, rectangle.
 *
 * Three small value types — `lh_point_t`, `lh_size_t`, `lh_rect_t` —
 * with the operations a UI / scene-graph layer needs: construct, query
 * (empty / contains-point / intersects / equal), and combine (offset,
 * inset, intersection).
 *
 * Plain lh value types for layers above the OS (the `pa` scene graph and
 * the like). The `lh/os/system` backends do not use them: each backend
 * works in its own native types, and any mapping between the two belongs
 * to the lh layer that needs it, not to the backend.
 *
 * No floating point: `lh_coord_t` is signed `int`, matching the screen
 * coordinates every OS exposes (LONG / int / CGFloat-rounded). When
 * `pa` adds sub-pixel rendering later, we add `lh_point_f_t` then — the
 * existing integer types stay ABI-stable.
 *
 * Requires nothing from `lh/os`. Safe in STM/embedded.
 */

#ifndef LH_GEOM_H
#define LH_GEOM_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/geom/point/fields.h>
#include <lh/geom/rect/fields.h>
#include <lh/geom/size/fields.h>
#include <lh/numeric/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_coord_t
 * @brief Signed 2D coordinate / extent, OS-portable.
 *
 * Same width as `lh_int_t`: `INT_MIN..INT_MAX` on every LP64/ILP32/LLP64
 * target, plenty for any realistic window or surface dimension.
 */
typedef lh_int_t lh_coord_t;

/**
 * @struct lh_point
 * @typedef lh_point_t
 * @brief A 2D point: `x` is horizontal, `y` is vertical (top-left origin,
 *        like every OS window coordinate system).
 */
struct lh_point
{
    lh_point_fields(lh_coord_t);
};
typedef struct lh_point lh_point_t;

/**
 * @struct lh_size
 * @typedef lh_size_t
 * @brief A 2D size: positive `width` and `height`.
 *
 * Negative extents are valid bit patterns but interpreted as "empty" by
 * ::lh_rect_is_empty — used to represent "nothing" without a sentinel.
 */
struct lh_size
{
    lh_size_fields(lh_coord_t);
};
typedef struct lh_size lh_size_t;

/**
 * @struct lh_rect
 * @typedef lh_rect_t
 * @brief An axis-aligned rectangle: an `origin` (top-left corner) plus a
 *        `size`, half-open — `width = 10` covers columns
 *        `[origin.x, origin.x + 10)`, i.e. `origin.x` through
 *        `origin.x + 9`.
 */
struct lh_rect
{
    lh_rect_fields(lh_point_t, lh_size_t);
};
typedef struct lh_rect lh_rect_t;

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_point_t` from explicit coordinates.
 */
lh_point_t
lh_point_make(lh_coord_t x, lh_coord_t y);

/**
 * @brief Make a `::lh_size_t` from explicit extents.
 */
lh_size_t
lh_size_make(lh_coord_t width, lh_coord_t height);

/**
 * @brief Make a `::lh_rect_t` from origin coordinates and extents.
 */
lh_rect_t
lh_rect_make(lh_coord_t x, lh_coord_t y, lh_coord_t width, lh_coord_t height);

/**
 * @brief Make a `::lh_rect_t` from min/max corners (Win32-style:
 *        left, top, right, bottom — right/bottom are exclusive).
 */
lh_rect_t
lh_rect_from_min_max(lh_coord_t x_min, lh_coord_t y_min,
                      lh_coord_t x_max, lh_coord_t y_max);

/**
 * @brief The "no rectangle" sentinel: origin `(0, 0)`, size `(0, 0)`.
 *        ::lh_rect_is_empty returns ::lh_bool_true for this value.
 */
lh_rect_t
lh_rect_zero(void);

/* ── Accessors ──────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self as a ::lh_coord_t. Zero for empty rectangles.
 */
lh_coord_t
lh_rect_width(const lh_rect_t *self);

/**
 * @brief Height of @p self as a ::lh_coord_t. Zero for empty rectangles.
 */
lh_coord_t
lh_rect_height(const lh_rect_t *self);

/**
 * @brief Top-left corner as a ::lh_point_t.
 */
lh_point_t
lh_rect_origin(const lh_rect_t *self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Test whether @p self is "empty" (zero or negative extent).
 */
lh_bool_t
lh_rect_is_empty(const lh_rect_t *self);

/**
 * @brief Test whether @p self covers @p point (inclusive at top/left,
 *        exclusive at bottom/right).
 */
lh_bool_t
lh_rect_contains_point(const lh_rect_t *self, lh_point_t point);

/**
 * @brief Test whether two rectangles share any non-empty area.
 */
lh_bool_t
lh_rect_intersects(const lh_rect_t *a, const lh_rect_t *b);

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_rect_eq(const lh_rect_t *a, const lh_rect_t *b);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Intersection of @p a and @p b. Returns ::lh_rect_zero if they do
 *        not overlap.
 */
lh_rect_t
lh_rect_intersection(const lh_rect_t *a, const lh_rect_t *b);

/**
 * @brief Union of @p a and @p b: the smallest rectangle containing both.
 *        Empty @p a or @p b returns the other.
 */
lh_rect_t
lh_rect_union(const lh_rect_t *a, const lh_rect_t *b);

/**
 * @brief Translate @p self by `(@p dx, @p dy)`.
 */
lh_rect_t
lh_rect_offset(const lh_rect_t *self, lh_coord_t dx, lh_coord_t dy);

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side (positive shrinks,
 *        negative grows).
 */
lh_rect_t
lh_rect_inset(const lh_rect_t *self, lh_coord_t dx, lh_coord_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GEOM_H */