/**
 * @file scalar.h
 * @brief A 2D point whose components are ::lh_math_scalar_t.
 *
 * Same shape as ::lh_math_point_t (`x`, `y`), but each component follows
 * ::LH_LIBRARY_OPTION_MATH_FPU: ::lh_int_t when the option is off,
 * ::lh_float_t when it is on. The screen point stays ::lh_math_point_t.
 * ::lh_math_point_scalar_to_point narrows by cast (toward zero when the
 * scalar is a float). ::lh_math_point_to_point_scalar widens the other way.
 *
 * Fields are not part of the public API: read and mutate them through the
 * accessors. The struct is defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_POINT_SCALAR_H
#define LH_MATH_POINT_SCALAR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/point.h>
#include <lh/math/point/fields.h>
#include <lh/math/scalar.h>
#include <lh/void.h>

/**
 * @struct lh_math_point_scalar
 * @typedef lh_math_point_scalar_t
 * @brief A 2D point of ::lh_math_scalar_t components.
 */
struct lh_math_point_scalar
{
    lh_math_point_fields(lh_math_scalar_t);
};
typedef struct lh_math_point_scalar lh_math_point_scalar_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_point_scalar_t` from explicit components.
 */
lh_math_point_scalar_t
lh_math_point_scalar_make(lh_math_scalar_t x, lh_math_scalar_t y);

/**
 * @brief The origin: `(0, 0)`.
 */
lh_math_point_scalar_t
lh_math_point_scalar_make_empty(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X component of @p self.
 */
lh_math_scalar_t
lh_math_point_scalar_get_x(const lh_math_point_scalar_t *self);

/**
 * @brief Y component of @p self.
 */
lh_math_scalar_t
lh_math_point_scalar_get_y(const lh_math_point_scalar_t *self);

/**
 * @brief Set the X component of @p self.
 */
lh_void
lh_math_point_scalar_set_x(lh_math_point_scalar_t *self, lh_math_scalar_t x);

/**
 * @brief Set the Y component of @p self.
 */
lh_void
lh_math_point_scalar_set_y(lh_math_point_scalar_t *self, lh_math_scalar_t y);

/* ── Conversions ─────────────────────────────────────────────────────────── */

/**
 * @brief Narrow @p self to a screen point.
 *
 * Casts each component to ::lh_math_coord_t. When the scalar is
 * ::lh_int_t the value is unchanged. When it is ::lh_float_t the cast
 * truncates toward zero: `3.9` becomes `3`, `-3.9` becomes `-3`.
 */
lh_math_point_t
lh_math_point_scalar_to_point(lh_math_point_scalar_t self);

/**
 * @brief Widen @p self to a scalar point.
 *
 * Casts each ::lh_math_coord_t to ::lh_math_scalar_t.
 */
lh_math_point_scalar_t
lh_math_point_to_point_scalar(lh_math_point_t self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_point_scalar_eq(const lh_math_point_scalar_t *a, const lh_math_point_scalar_t *b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_POINT_SCALAR_H */
