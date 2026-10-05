/**
 * @file scalar.h
 * @brief A 2D size whose components are ::lh_math_scalar_t.
 *
 * Same shape as ::lh_math_size_t (`width`, `height`). Each component follows
 * ::LH_LIBRARY_OPTION_MATH_FPU. The screen size stays ::lh_math_size_t.
 * ::lh_math_size_scalar_to_size narrows by cast (toward zero when the scalar
 * is a float). ::lh_math_size_to_size_scalar widens the other way.
 *
 * Fields are not part of the public API: read and mutate them through the
 * accessors. The struct is defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_SIZE_SCALAR_H
#define LH_MATH_SIZE_SCALAR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/scalar.h>
#include <lh/math/size.h>
#include <lh/math/size/fields.h>
#include <lh/void.h>

/**
 * @struct lh_math_size_scalar
 * @typedef lh_math_size_scalar_t
 * @brief A 2D size of ::lh_math_scalar_t components.
 */
struct lh_math_size_scalar
{
    lh_math_size_fields(lh_math_scalar_t);
};
typedef struct lh_math_size_scalar lh_math_size_scalar_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_size_scalar_t` from explicit extents.
 */
lh_math_size_scalar_t
lh_math_size_scalar_make(lh_math_scalar_t width, lh_math_scalar_t height);

/**
 * @brief The empty size: `(0, 0)`.
 */
lh_math_size_scalar_t
lh_math_size_scalar_make_empty(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self.
 */
lh_math_scalar_t
lh_math_size_scalar_get_width(const lh_math_size_scalar_t *self);

/**
 * @brief Height of @p self.
 */
lh_math_scalar_t
lh_math_size_scalar_get_height(const lh_math_size_scalar_t *self);

/**
 * @brief Set the width of @p self.
 */
lh_void
lh_math_size_scalar_set_width(lh_math_size_scalar_t *self, lh_math_scalar_t width);

/**
 * @brief Set the height of @p self.
 */
lh_void
lh_math_size_scalar_set_height(lh_math_size_scalar_t *self, lh_math_scalar_t height);

/* ── Conversions ─────────────────────────────────────────────────────────── */

/**
 * @brief Narrow @p self to a screen size.
 *
 * Casts each component to ::lh_math_coord_t. When the scalar is
 * ::lh_int_t the value is unchanged. When it is ::lh_float_t the cast
 * truncates toward zero.
 */
lh_math_size_t
lh_math_size_scalar_to_size(lh_math_size_scalar_t self);

/**
 * @brief Widen @p self to a scalar size.
 *
 * Casts each ::lh_math_coord_t to ::lh_math_scalar_t.
 */
lh_math_size_scalar_t
lh_math_size_to_size_scalar(lh_math_size_t self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_size_scalar_eq(const lh_math_size_scalar_t *a, const lh_math_size_scalar_t *b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_SIZE_SCALAR_H */
