/**
 * @file geom.h
 * @brief Integer 2D geometry of the UI layer: point, size, rectangle.
 *
 * Three small value types — `lh_ui_point_t`, `lh_ui_size_t`,
 * `lh_ui_rect_t` — in whole pixels, with the operations a UI / widget
 * layer needs: construct, query (empty / contains-point / intersects /
 * equal), and combine (offset, inset, intersection, union).
 *
 * These are screen / window coordinates, not general math: positions,
 * directions and sub-pixel work elsewhere use ::lh_vec2_t (float). The
 * `lh/os/system` backends do not use these types either: each backend works
 * in its own native types, and any mapping between the two belongs to the lh
 * layer that needs it.
 *
 * `lh_ui_coord_t` is a signed `int`, which is 32-bit on every supported
 * target — the same width as Win32's `LONG` in `POINT` / `RECT` (32-bit on
 * Win64 too) and wide enough for any realistic surface.
 *
 * Requires nothing from `lh/os`. Safe in STM/embedded.
 */

#ifndef LH_UI_GEOM_H
#define LH_UI_GEOM_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/geom/point/fields.h>
#include <lh/ui/geom/rect/fields.h>
#include <lh/ui/geom/size/fields.h>
#include <lh/numeric/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_ui_coord_t
 * @brief Signed 2D coordinate / extent, OS-portable.
 *
 * Same width as `lh_int_t`: `INT_MIN..INT_MAX` on every LP64/ILP32/LLP64
 * target, plenty for any realistic window or surface dimension.
 */
typedef lh_int_t lh_ui_coord_t;

/**
 * @struct lh_ui_point
 * @typedef lh_ui_point_t
 * @brief A 2D point: `x` is horizontal, `y` is vertical (top-left origin,
 *        like every OS window coordinate system).
 */
struct lh_ui_point
{
    lh_ui_point_fields(lh_ui_coord_t);
};
typedef struct lh_ui_point lh_ui_point_t;

/**
 * @struct lh_ui_size
 * @typedef lh_ui_size_t
 * @brief A 2D size: positive `width` and `height`.
 *
 * Negative extents are valid bit patterns but interpreted as "empty" by
 * ::lh_ui_rect_is_empty — used to represent "nothing" without a sentinel.
 */
struct lh_ui_size
{
    lh_ui_size_fields(lh_ui_coord_t);
};
typedef struct lh_ui_size lh_ui_size_t;

/**
 * @struct lh_ui_rect
 * @typedef lh_ui_rect_t
 * @brief An axis-aligned rectangle: an `origin` (top-left corner) plus a
 *        `size`, half-open — `width = 10` covers columns
 *        `[origin.x, origin.x + 10)`, i.e. `origin.x` through
 *        `origin.x + 9`.
 */
struct lh_ui_rect
{
    lh_ui_rect_fields(lh_ui_point_t, lh_ui_size_t);
};
typedef struct lh_ui_rect lh_ui_rect_t;

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_ui_point_t` from explicit coordinates.
 */
lh_ui_point_t
lh_ui_point_make(lh_ui_coord_t x, lh_ui_coord_t y);

/**
 * @brief Make a `::lh_ui_size_t` from explicit extents.
 */
lh_ui_size_t
lh_ui_size_make(lh_ui_coord_t width, lh_ui_coord_t height);

/**
 * @brief Make a `::lh_ui_rect_t` from origin coordinates and extents.
 */
lh_ui_rect_t
lh_ui_rect_make(lh_ui_coord_t x, lh_ui_coord_t y, lh_ui_coord_t width, lh_ui_coord_t height);

/**
 * @brief Make a `::lh_ui_rect_t` from min/max corners (Win32-style:
 *        left, top, right, bottom — right/bottom are exclusive).
 */
lh_ui_rect_t
lh_ui_rect_from_min_max(lh_ui_coord_t x_min, lh_ui_coord_t y_min,
                        lh_ui_coord_t x_max, lh_ui_coord_t y_max);

/**
 * @brief The "no rectangle" sentinel: origin `(0, 0)`, size `(0, 0)`.
 *        ::lh_ui_rect_is_empty returns ::lh_bool_true for this value.
 */
lh_ui_rect_t
lh_ui_rect_zero(void);

/* ── Accessors ──────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self as a ::lh_ui_coord_t. Zero for empty rectangles.
 */
lh_ui_coord_t
lh_ui_rect_width(const lh_ui_rect_t *self);

/**
 * @brief Height of @p self as a ::lh_ui_coord_t. Zero for empty rectangles.
 */
lh_ui_coord_t
lh_ui_rect_height(const lh_ui_rect_t *self);

/**
 * @brief Top-left corner as a ::lh_ui_point_t.
 */
lh_ui_point_t
lh_ui_rect_origin(const lh_ui_rect_t *self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Test whether @p self is "empty" (zero or negative extent).
 */
lh_bool_t
lh_ui_rect_is_empty(const lh_ui_rect_t *self);

/**
 * @brief Test whether @p self covers @p point (inclusive at top/left,
 *        exclusive at bottom/right).
 */
lh_bool_t
lh_ui_rect_contains_point(const lh_ui_rect_t *self, lh_ui_point_t point);

/**
 * @brief Test whether two rectangles share any non-empty area.
 */
lh_bool_t
lh_ui_rect_intersects(const lh_ui_rect_t *a, const lh_ui_rect_t *b);

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_ui_rect_eq(const lh_ui_rect_t *a, const lh_ui_rect_t *b);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Intersection of @p a and @p b. Returns ::lh_ui_rect_zero if they do
 *        not overlap.
 */
lh_ui_rect_t
lh_ui_rect_intersection(const lh_ui_rect_t *a, const lh_ui_rect_t *b);

/**
 * @brief Union of @p a and @p b: the smallest rectangle containing both.
 *        Empty @p a or @p b returns the other.
 */
lh_ui_rect_t
lh_ui_rect_union(const lh_ui_rect_t *a, const lh_ui_rect_t *b);

/**
 * @brief Translate @p self by `(@p dx, @p dy)`.
 */
lh_ui_rect_t
lh_ui_rect_offset(const lh_ui_rect_t *self, lh_ui_coord_t dx, lh_ui_coord_t dy);

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side (positive shrinks,
 *        negative grows).
 */
lh_ui_rect_t
lh_ui_rect_inset(const lh_ui_rect_t *self, lh_ui_coord_t dx, lh_ui_coord_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GEOM_H */