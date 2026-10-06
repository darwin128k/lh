/**
 * @file fsize.h
 * @brief A 2D size whose components are ::lh_math_fscalar_t.
 *
 * Same shape as ::lh_math_size_t (`width`, `height`), with continuous extents.
 * The screen size stays ::lh_math_size_t.
 * ::lh_math_fsize_to_size narrows by cast (truncates toward zero).
 * ::lh_math_size_to_fsize widens the other way.
 *
 * Built only when ::LH_LIBRARY_OPTION_MATH_FPU is ON. UI picks this type or
 * the integer ::lh_math_size_t through <lh/ui/size.h>.
 *
 * Fields are not part of the public API: read and mutate them through the
 * accessors. The struct is defined here only so it can be embedded by value.
 */

#ifndef LH_MATH_FSIZE_H
#define LH_MATH_FSIZE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/size.h>
#include <lh/math/size/fields.h>
#include <lh/math/fpoint.h>
#include <lh/math/fscalar.h>
#include <lh/void.h>

/**
 * @struct lh_math_fsize
 * @typedef lh_math_fsize_t
 * @brief A 2D size of ::lh_math_fscalar_t components.
 */
struct lh_math_fsize
{
    lh_math_size_fields(lh_math_fscalar_t);
};
typedef struct lh_math_fsize lh_math_fsize_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_fsize_t` from explicit extents.
 */
lh_math_fsize_t
lh_math_fsize_make(lh_math_fscalar_t width, lh_math_fscalar_t height);

/**
 * @brief The empty size: `(0, 0)`.
 */
lh_math_fsize_t
lh_math_fsize_make_empty(void);

/**
 * @brief Size spanning from @p min (inclusive) to @p max (exclusive).
 */
lh_math_fsize_t
lh_math_fsize_from_extent(const lh_math_fpoint_t *min, const lh_math_fpoint_t *max);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self.
 */
lh_math_fscalar_t
lh_math_fsize_get_width(const lh_math_fsize_t *self);

/**
 * @brief Height of @p self.
 */
lh_math_fscalar_t
lh_math_fsize_get_height(const lh_math_fsize_t *self);

/**
 * @brief Set the width of @p self.
 */
lh_void
lh_math_fsize_set_width(lh_math_fsize_t *self, lh_math_fscalar_t width);

/**
 * @brief Set the height of @p self.
 */
lh_void
lh_math_fsize_set_height(lh_math_fsize_t *self, lh_math_fscalar_t height);

/* ── Conversions ─────────────────────────────────────────────────────────── */

/**
 * @brief Narrow @p self to a screen size.
 *
 * Casts each component to ::lh_math_scalar_t (truncates toward zero).
 */
lh_math_size_t
lh_math_fsize_to_size(lh_math_fsize_t self);

/**
 * @brief Widen @p self to a continuous size.
 *
 * Casts each ::lh_math_scalar_t to ::lh_math_fscalar_t.
 */
lh_math_fsize_t
lh_math_size_to_fsize(lh_math_size_t self);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_fsize_eq(const lh_math_fsize_t *a, const lh_math_fsize_t *b);

/**
 * @brief Test whether @p self is empty (zero or negative extent).
 */
lh_bool_t
lh_math_fsize_is_empty(const lh_math_fsize_t *self);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Inset @p self by `(@p dx, @p dy)` on each side (positive shrinks,
 *        negative grows).
 */
lh_math_fsize_t
lh_math_fsize_inset(const lh_math_fsize_t *self, lh_math_fscalar_t dx, lh_math_fscalar_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FSIZE_H */
