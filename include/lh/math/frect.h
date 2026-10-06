/**
 * @file frect.h
 * @brief An axis-aligned rectangle whose components are ::lh_math_scalar_t.
 *
 * Same shape as ::lh_math_rect_t: a ::lh_math_fpoint_t origin plus a
 * ::lh_math_fsize_t, half-open. The screen rectangle stays
 * ::lh_math_rect_t. ::lh_math_frect_to_rect narrows by cast (toward
 * zero). ::lh_math_rect_to_frect widens the other way.
 *
 * Empty rectangles are `size.width <= 0` or `size.height <= 0`, the same
 * rule as ::lh_math_rect_is_empty.
 *
 * Built only when ::LH_LIBRARY_OPTION_MATH_FPU is ON. UI picks this type or
 * the integer ::lh_math_rect_t through <lh/ui/rect.h>.
 *
 * Fields are not part of the public API: read and mutate them through the
 * accessors. The struct is defined here only so `origin` and `size` can be
 * embedded by value.
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
 * @brief An axis-aligned rectangle of ::lh_math_scalar_t components.
 */
struct lh_math_frect
{
    lh_math_rect_fields(lh_math_fpoint_t, lh_math_fsize_t);
};
typedef struct lh_math_frect lh_math_frect_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_frect_t` from origin and extents.
 */
lh_math_frect_t
lh_math_frect_make(lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t width, lh_math_scalar_t height);

/**
 * @brief The empty rectangle: origin `(0, 0)`, size `(0, 0)`.
 */
lh_math_frect_t
lh_math_frect_make_empty(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Top-left corner of @p self.
 */
lh_math_fpoint_t
lh_math_frect_get_origin(const lh_math_frect_t *self);

/**
 * @brief Size of @p self.
 */
lh_math_fsize_t
lh_math_frect_get_size(const lh_math_frect_t *self);

/**
 * @brief X coordinate of the origin of @p self.
 */
lh_math_scalar_t
lh_math_frect_get_x(const lh_math_frect_t *self);

/**
 * @brief Y coordinate of the origin of @p self.
 */
lh_math_scalar_t
lh_math_frect_get_y(const lh_math_frect_t *self);

/**
 * @brief Width component of the size of @p self.
 */
lh_math_scalar_t
lh_math_frect_get_size_width(const lh_math_frect_t *self);

/**
 * @brief Height component of the size of @p self.
 */
lh_math_scalar_t
lh_math_frect_get_size_height(const lh_math_frect_t *self);

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
 * @brief Width of @p self. Zero when @p self is empty.
 */
lh_math_scalar_t
lh_math_frect_get_width(const lh_math_frect_t *self);

/**
 * @brief Height of @p self. Zero when @p self is empty.
 */
lh_math_scalar_t
lh_math_frect_get_height(const lh_math_frect_t *self);

/**
 * @brief Test whether @p self is empty (zero or negative extent).
 */
lh_bool_t
lh_math_frect_is_empty(const lh_math_frect_t *self);

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_frect_eq(const lh_math_frect_t *a, const lh_math_frect_t *b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FRECT_H */
