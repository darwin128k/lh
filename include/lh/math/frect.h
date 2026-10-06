/**
 * @file frect.h
 * @brief An axis-aligned rectangle whose components are ::lh_math_fscalar_t.
 *
 * Same shape as ::lh_math_rect_t: a ::lh_math_fpoint_t origin plus a
 * ::lh_math_fsize_t, half-open. The screen rectangle stays
 * ::lh_math_rect_t. ::lh_math_frect_to_rect narrows by cast (toward
 * zero). ::lh_math_rect_to_frect widens the other way.
 *
 * Empty rectangles follow ::lh_math_fsize_is_empty on the embedded size.
 *
 * Built only when ::LH_LIBRARY_OPTION_MATH_FPU is ON. UI picks this type or
 * the integer ::lh_math_rect_t through <lh/ui/rect.h>.
 *
 * Accessors return pointers to the embedded `origin` and `size`; scalar
 * components live on ::lh_math_fpoint_t / ::lh_math_fsize_t. The struct is
 * defined here only so those members can be embedded by value.
 */

#ifndef LH_MATH_FRECT_H
#define LH_MATH_FRECT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/rect.h>
#include <lh/math/rect/fields.h>
#include <lh/math/fpoint.h>
#include <lh/math/fsize.h>
#include <lh/void.h>

/**
 * @struct lh_math_frect
 * @typedef lh_math_frect_t
 * @brief An axis-aligned rectangle of ::lh_math_fscalar_t components.
 */
struct lh_math_frect
{
    lh_math_rect_fields(lh_math_fpoint_t, lh_math_fsize_t);
};
typedef struct lh_math_frect lh_math_frect_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_frect_t` from origin coordinates and extents.
 */
lh_math_frect_t
lh_math_frect_make(lh_math_fscalar_t x, lh_math_fscalar_t y, lh_math_fscalar_t width, lh_math_fscalar_t height);

/**
 * @brief Make a `::lh_math_frect_t` from @p origin and @p size.
 */
lh_math_frect_t
lh_math_frect_make_origin_size(lh_math_fpoint_t origin, lh_math_fsize_t size);

/**
 * @brief Make a `::lh_math_frect_t` from exclusive extent corners.
 *
 * @p max is exclusive (`origin + size`). Empty size yields
 * ::lh_math_frect_make_empty.
 */
lh_math_frect_t
lh_math_frect_from_extent(const lh_math_fpoint_t *min, const lh_math_fpoint_t *max);

/**
 * @brief The empty rectangle: origin `(0, 0)`, size `(0, 0)`.
 */
lh_math_frect_t
lh_math_frect_make_empty(void);

/**
 * @brief Fill @p self from origin coordinates and extents.
 */
lh_void
lh_math_frect_init(lh_math_frect_t *self, lh_math_fscalar_t x, lh_math_fscalar_t y, lh_math_fscalar_t width,
                   lh_math_fscalar_t height);

/**
 * @brief Fill @p self from @p origin and @p size.
 */
lh_void
lh_math_frect_init_origin_size(lh_math_frect_t *self, lh_math_fpoint_t origin, lh_math_fsize_t size);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Pointer to the top-left corner of @p self.
 */
lh_math_fpoint_t *
lh_math_frect_get_origin(lh_math_frect_t *self);

/**
 * @brief Const pointer to the top-left corner of @p self.
 */
const lh_math_fpoint_t *
lh_math_frect_get_origin_as_const(const lh_math_frect_t *self);

/**
 * @brief Pointer to the size of @p self.
 */
lh_math_fsize_t *
lh_math_frect_get_size(lh_math_frect_t *self);

/**
 * @brief Const pointer to the size of @p self.
 */
const lh_math_fsize_t *
lh_math_frect_get_size_as_const(const lh_math_frect_t *self);

/**
 * @brief Replace the origin of @p self with @p origin.
 */
lh_void
lh_math_frect_set_origin(lh_math_frect_t *self, lh_math_fpoint_t origin);

/**
 * @brief Replace the size of @p self with @p size.
 */
lh_void
lh_math_frect_set_size(lh_math_frect_t *self, lh_math_fsize_t size);

/**
 * @brief Exclusive far corner: origin offset by size.
 */
lh_math_fpoint_t
lh_math_frect_far(const lh_math_frect_t *self);

/**
 * @brief Element-wise minimum of the origins of @p a and @p b.
 */
lh_math_fpoint_t
lh_math_frect_origin_min(const lh_math_frect_t *a, const lh_math_frect_t *b);

/**
 * @brief Element-wise maximum of the origins of @p a and @p b.
 */
lh_math_fpoint_t
lh_math_frect_origin_max(const lh_math_frect_t *a, const lh_math_frect_t *b);

/**
 * @brief Element-wise minimum of the exclusive far corners of @p a and @p b.
 */
lh_math_fpoint_t
lh_math_frect_far_min(const lh_math_frect_t *a, const lh_math_frect_t *b);

/**
 * @brief Element-wise maximum of the exclusive far corners of @p a and @p b.
 */
lh_math_fpoint_t
lh_math_frect_far_max(const lh_math_frect_t *a, const lh_math_frect_t *b);

/* ── Conversions ─────────────────────────────────────────────────────────── */

/**
 * @brief Narrow @p self to a screen rectangle.
 *
 * Casts the origin and the size (truncates toward zero).
 */
lh_math_rect_t
lh_math_frect_to_rect(lh_math_frect_t self);

/**
 * @brief Widen @p self to a continuous rectangle.
 */
lh_math_frect_t
lh_math_rect_to_frect(lh_math_rect_t self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Test whether @p self is empty (zero or negative extent).
 */
lh_bool_t
lh_math_frect_is_empty(const lh_math_frect_t *self);

/**
 * @brief Test whether @p self covers @p point (inclusive at top/left,
 *        exclusive at bottom/right).
 */
lh_bool_t
lh_math_frect_contains_point(const lh_math_frect_t *self, lh_math_fpoint_t point);

/**
 * @brief Test whether two rectangles share any non-empty area.
 */
lh_bool_t
lh_math_frect_intersects(const lh_math_frect_t *a, const lh_math_frect_t *b);

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_frect_eq(const lh_math_frect_t *a, const lh_math_frect_t *b);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Intersection of @p a and @p b. Returns ::lh_math_frect_make_empty if they
 *        do not overlap.
 */
lh_math_frect_t
lh_math_frect_intersection(const lh_math_frect_t *a, const lh_math_frect_t *b);

/**
 * @brief Union of @p a and @p b: the smallest rectangle containing both.
 *        Empty @p a or @p b returns the other.
 */
lh_math_frect_t
lh_math_frect_union(const lh_math_frect_t *a, const lh_math_frect_t *b);

/**
 * @brief Translate @p self by `(@p dx, @p dy)`.
 */
lh_math_frect_t
lh_math_frect_offset(const lh_math_frect_t *self, lh_math_fscalar_t dx, lh_math_fscalar_t dy);

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side (positive shrinks,
 *        negative grows).
 */
lh_math_frect_t
lh_math_frect_inset(const lh_math_frect_t *self, lh_math_fscalar_t dx, lh_math_fscalar_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FRECT_H */
