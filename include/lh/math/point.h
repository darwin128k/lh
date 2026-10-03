/**
 * @file point.h
 * @brief A 2D point: ::lh_math_point_t with ::lh_math_coord_t `x, y`.
 *
 * `x` is horizontal, `y` is vertical (top-left origin, like every OS window
 * coordinate system).
 */

#ifndef LH_MATH_POINT_H
#define LH_MATH_POINT_H

#include <lh/compiler/extern/c.h>
#include <lh/math/coord.h>
#include <lh/math/point/fields.h>

/**
 * @struct lh_math_point
 * @typedef lh_math_point_t
 * @brief A 2D point: `x` is horizontal, `y` is vertical.
 */
struct lh_math_point
{
    lh_math_point_fields(lh_math_coord_t);
};
typedef struct lh_math_point lh_math_point_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Make a `::lh_math_point_t` from explicit coordinates.
 */
lh_math_point_t
lh_math_point_make(lh_math_coord_t x, lh_math_coord_t y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_POINT_H */