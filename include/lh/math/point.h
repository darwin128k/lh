/**
 * @file point.h
 * @brief A 2D point whose components are ::lh_math_scalar_t.
 *
 * Same shape as ::lh_math_ipoint_t (`x`, `y`), with continuous coordinates.
 * The screen point stays ::lh_math_ipoint_t.
 * ::lh_math_point_to_ipoint narrows by cast (truncates toward zero).
 * ::lh_math_ipoint_to_point widens the other way.
 *
 * Built only when ::LH_LIBRARY_OPTION_MATH_FPU is ON. UI picks this type or
 * the integer ::lh_math_ipoint_t through <lh/ui/point.h>.
 *
 * Fields are not part of the public API: read and mutate them through the
 * accessors. The struct is defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_POINT_H
#define LH_MATH_POINT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/ipoint.h>
#include <lh/math/ipoint/fields.h>
#include <lh/math/scalar.h>
#include <lh/void.h>

/**
 * @struct lh_math_point
 * @typedef lh_math_point_t
 * @brief A 2D point of ::lh_math_scalar_t components.
 */
struct lh_math_point
{
    lh_math_ipoint_fields(lh_math_scalar_t);
};
typedef struct lh_math_point lh_math_point_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_point_t` from explicit components.
 */
lh_math_point_t
lh_math_point_make(lh_math_scalar_t x, lh_math_scalar_t y);

/**
 * @brief The origin: `(0, 0)`.
 */
lh_math_point_t
lh_math_point_make_empty(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X component of @p self.
 */
lh_math_scalar_t
lh_math_point_get_x(const lh_math_point_t *self);

/**
 * @brief Y component of @p self.
 */
lh_math_scalar_t
lh_math_point_get_y(const lh_math_point_t *self);

/**
 * @brief Set the X component of @p self.
 */
lh_void
lh_math_point_set_x(lh_math_point_t *self, lh_math_scalar_t x);

/**
 * @brief Set the Y component of @p self.
 */
lh_void
lh_math_point_set_y(lh_math_point_t *self, lh_math_scalar_t y);

/* ── Conversions ─────────────────────────────────────────────────────────── */

/**
 * @brief Narrow @p self to a screen point.
 *
 * Casts each component to ::lh_math_iscalar_t (truncates toward zero).
 */
lh_math_ipoint_t
lh_math_point_to_ipoint(lh_math_point_t self);

/**
 * @brief Widen @p self to a continuous point.
 *
 * Casts each ::lh_math_iscalar_t to ::lh_math_scalar_t.
 */
lh_math_point_t
lh_math_ipoint_to_point(lh_math_ipoint_t self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_point_eq(const lh_math_point_t *a, const lh_math_point_t *b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_POINT_H */
