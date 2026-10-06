/**
 * @file size.h
 * @brief A 2D size whose components are ::lh_math_scalar_t.
 *
 * Same shape as ::lh_math_isize_t (`width`, `height`), with continuous extents.
 * The screen size stays ::lh_math_isize_t.
 * ::lh_math_size_to_isize narrows by cast (truncates toward zero).
 * ::lh_math_isize_to_size widens the other way.
 *
 * Built only when ::LH_LIBRARY_OPTION_MATH_FPU is ON. UI picks this type or
 * the integer ::lh_math_isize_t through <lh/ui/size.h>.
 *
 * Fields are not part of the public API: read and mutate them through the
 * accessors. The struct is defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_SIZE_H
#define LH_MATH_SIZE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/isize.h>
#include <lh/math/isize/fields.h>
#include <lh/math/scalar.h>
#include <lh/void.h>

/**
 * @struct lh_math_size
 * @typedef lh_math_size_t
 * @brief A 2D size of ::lh_math_scalar_t components.
 */
struct lh_math_size
{
    lh_math_isize_fields(lh_math_scalar_t);
};
typedef struct lh_math_size lh_math_size_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_size_t` from explicit extents.
 */
lh_math_size_t
lh_math_size_make(lh_math_scalar_t width, lh_math_scalar_t height);

/**
 * @brief The empty size: `(0, 0)`.
 */
lh_math_size_t
lh_math_size_make_empty(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self.
 */
lh_math_scalar_t
lh_math_size_get_width(const lh_math_size_t *self);

/**
 * @brief Height of @p self.
 */
lh_math_scalar_t
lh_math_size_get_height(const lh_math_size_t *self);

/**
 * @brief Set the width of @p self.
 */
lh_void
lh_math_size_set_width(lh_math_size_t *self, lh_math_scalar_t width);

/**
 * @brief Set the height of @p self.
 */
lh_void
lh_math_size_set_height(lh_math_size_t *self, lh_math_scalar_t height);

/* ── Conversions ─────────────────────────────────────────────────────────── */

/**
 * @brief Narrow @p self to a screen size.
 *
 * Casts each component to ::lh_math_iscalar_t (truncates toward zero).
 */
lh_math_isize_t
lh_math_size_to_isize(lh_math_size_t self);

/**
 * @brief Widen @p self to a continuous size.
 *
 * Casts each ::lh_math_iscalar_t to ::lh_math_scalar_t.
 */
lh_math_size_t
lh_math_isize_to_size(lh_math_isize_t self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_size_eq(const lh_math_size_t *a, const lh_math_size_t *b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_SIZE_H */
