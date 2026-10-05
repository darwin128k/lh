/**
 * @file scalar.h
 * @brief An axis-aligned rectangle whose components are ::lh_math_scalar_t.
 *
 * Same shape as ::lh_math_rect_t: a ::lh_math_point_scalar_t origin plus a
 * ::lh_math_size_scalar_t, half-open. The screen rectangle stays
 * ::lh_math_rect_t. ::lh_math_rect_scalar_to_rect narrows by cast (toward
 * zero when the scalar is a float). ::lh_math_rect_to_rect_scalar widens
 * the other way.
 *
 * Empty rectangles are `size.width <= 0` or `size.height <= 0`, the same
 * rule as ::lh_math_rect_is_empty.
 *
 * Fields are not part of the public API: read and mutate them through the
 * accessors. The struct is defined here only so `origin` and `size` can be
 * embedded by value.
 */

#ifndef LH_MATH_RECT_SCALAR_H
#define LH_MATH_RECT_SCALAR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/point/scalar.h>
#include <lh/math/rect.h>
#include <lh/math/rect/fields.h>
#include <lh/math/size/scalar.h>
#include <lh/void.h>

/**
 * @struct lh_math_rect_scalar
 * @typedef lh_math_rect_scalar_t
 * @brief An axis-aligned rectangle of ::lh_math_scalar_t components.
 */
struct lh_math_rect_scalar
{
    lh_math_rect_fields(lh_math_point_scalar_t, lh_math_size_scalar_t);
};
typedef struct lh_math_rect_scalar lh_math_rect_scalar_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_rect_scalar_t` from origin and extents.
 */
lh_math_rect_scalar_t
lh_math_rect_scalar_make(lh_math_scalar_t x, lh_math_scalar_t y,
                         lh_math_scalar_t width, lh_math_scalar_t height);

/**
 * @brief The empty rectangle: origin `(0, 0)`, size `(0, 0)`.
 */
lh_math_rect_scalar_t
lh_math_rect_scalar_make_empty(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Top-left corner of @p self.
 */
lh_math_point_scalar_t
lh_math_rect_scalar_get_origin(const lh_math_rect_scalar_t *self);

/**
 * @brief Size of @p self.
 */
lh_math_size_scalar_t
lh_math_rect_scalar_get_size(const lh_math_rect_scalar_t *self);

/**
 * @brief X coordinate of the origin of @p self.
 */
lh_math_scalar_t
lh_math_rect_scalar_get_x(const lh_math_rect_scalar_t *self);

/**
 * @brief Y coordinate of the origin of @p self.
 */
lh_math_scalar_t
lh_math_rect_scalar_get_y(const lh_math_rect_scalar_t *self);

/**
 * @brief Width component of the size of @p self.
 */
lh_math_scalar_t
lh_math_rect_scalar_get_size_width(const lh_math_rect_scalar_t *self);

/**
 * @brief Height component of the size of @p self.
 */
lh_math_scalar_t
lh_math_rect_scalar_get_size_height(const lh_math_rect_scalar_t *self);

/**
 * @brief Replace the origin of @p self with @p origin.
 */
lh_void
lh_math_rect_scalar_set_origin(lh_math_rect_scalar_t *self, lh_math_point_scalar_t origin);

/**
 * @brief Replace the size of @p self with @p size.
 */
lh_void
lh_math_rect_scalar_set_size(lh_math_rect_scalar_t *self, lh_math_size_scalar_t size);

/* ── Conversions ─────────────────────────────────────────────────────────── */

/**
 * @brief Narrow @p self to a screen rectangle.
 *
 * Casts the origin and the size. When the scalar is ::lh_float_t each
 * component truncates toward zero.
 */
lh_math_rect_t
lh_math_rect_scalar_to_rect(lh_math_rect_scalar_t self);

/**
 * @brief Widen @p self to a scalar rectangle.
 */
lh_math_rect_scalar_t
lh_math_rect_to_rect_scalar(lh_math_rect_t self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self. Zero when @p self is empty.
 */
lh_math_scalar_t
lh_math_rect_scalar_width(const lh_math_rect_scalar_t *self);

/**
 * @brief Height of @p self. Zero when @p self is empty.
 */
lh_math_scalar_t
lh_math_rect_scalar_height(const lh_math_rect_scalar_t *self);

/**
 * @brief Test whether @p self is empty (zero or negative extent).
 */
lh_bool_t
lh_math_rect_scalar_is_empty(const lh_math_rect_scalar_t *self);

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_rect_scalar_eq(const lh_math_rect_scalar_t *a, const lh_math_rect_scalar_t *b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_RECT_SCALAR_H */
