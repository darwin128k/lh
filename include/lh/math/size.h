/**
 * @file size.h
 * @brief A 2D size: ::lh_math_size_t with ::lh_math_coord_t `width`, `height`.
 *
 * Negative extents are valid bit patterns but interpreted as "empty" by
 * ::lh_math_rect_is_empty — used to represent "nothing" without a sentinel.
 */

#ifndef LH_MATH_SIZE_H
#define LH_MATH_SIZE_H

#include <lh/compiler/extern/c.h>
#include <lh/math/coord.h>
#include <lh/math/size/fields.h>

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

/**
 * @brief Make a `::lh_math_size_t` from explicit extents.
 */
lh_math_size_t
lh_math_size_make(lh_math_coord_t width, lh_math_coord_t height);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_SIZE_H */