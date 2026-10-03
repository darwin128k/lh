/**
 * @file size.h
 * @brief A 2D size: ::lh_math_size_t with ::lh_math_coord_t `width`, `height`.
 *
 * Negative extents are valid bit patterns but interpreted as "empty" by
 * ::lh_math_rect_is_empty — used to represent "nothing" without a sentinel.
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_size_get_*` / `lh_math_size_set_*` accessors. The struct is
 * defined here only so it can be embedded by value (e.g. as
 * `lh_math_rect_t::size`).
 */

#ifndef LH_MATH_SIZE_H
#define LH_MATH_SIZE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/coord.h>
#include <lh/math/size/fields.h>
#include <lh/void.h>

/**
 * @struct lh_math_size
 * @typedef lh_math_size_t
 * @brief A 2D size: positive `width` and `height`.
 */
struct lh_math_size
{
    lh_math_size_fields(lh_math_coord_t);
};
typedef struct lh_math_size lh_math_size_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_size_t` from explicit extents.
 */
lh_math_size_t
lh_math_size_make(lh_math_coord_t width, lh_math_coord_t height);

/**
 * @brief The empty size: `(0, 0)`. ::lh_math_rect_is_empty treats this as empty.
 */
lh_math_size_t
lh_math_size_zero(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self.
 */
lh_math_coord_t
lh_math_size_get_width(const lh_math_size_t *self);

/**
 * @brief Height of @p self.
 */
lh_math_coord_t
lh_math_size_get_height(const lh_math_size_t *self);

/**
 * @brief Set the width of @p self.
 */
lh_void
lh_math_size_set_width(lh_math_size_t *self, lh_math_coord_t width);

/**
 * @brief Set the height of @p self.
 */
lh_void
lh_math_size_set_height(lh_math_size_t *self, lh_math_coord_t height);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_size_eq(const lh_math_size_t *a, const lh_math_size_t *b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_SIZE_H */