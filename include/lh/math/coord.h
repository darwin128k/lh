/**
 * @file coord.h
 * @brief Signed 2D coordinate / extent type used by ::lh_math_point_t,
 *        ::lh_math_size_t and ::lh_math_rect_t.
 *
 * `lh_math_coord_t` is a signed `int`, 32-bit on every supported target —
 * the same width as Win32's `LONG` in `POINT` / `RECT` (32-bit on Win64 too)
 * and wide enough for any realistic surface.
 *
 * Same width as `lh_int_t`: `INT_MIN..INT_MAX` on every LP64/ILP32/LLP64
 * target.
 */

#ifndef LH_MATH_COORD_H
#define LH_MATH_COORD_H

#include <lh/numeric/types.h>

typedef lh_int_t lh_math_coord_t;

#endif /* LH_MATH_COORD_H */