/**
 * @file math.h
 * @brief Integer 2D geometry primitives: point, size, rectangle.
 *
 * Three small value types — `lh_math_point_t`, `lh_math_size_t`,
 * `lh_math_rect_t` — in whole pixels, with the operations a 2D layer needs:
 * construct, query (empty / contains-point / intersects / equal), and
 * combine (offset, inset, intersection, union).
 *
 * These are screen / window coordinates, not floating-point math: positions,
 * directions and sub-pixel work elsewhere use ::lh_vec2_t (float). The
 * `lh/os/system` backends do not use these types either: each backend works
 * in its own native types, and any mapping between the two belongs to the lh
 * layer that needs it.
 *
 * ::lh_math_coord_t is a signed `int`, which is 32-bit on every supported
 * target — the same width as Win32's `LONG` in `POINT` / `RECT` (32-bit on
 * Win64 too) and wide enough for any realistic surface.
 *
 * Requires nothing from `lh/os`. Safe in STM/embedded.
 *
 * The type-specific headers (`<lh/math/coord.h>`, `<lh/math/point.h>`,
 * `<lh/math/size.h>`, `<lh/math/rect.h>`) declare one thing each — include
 * this file when you need several.
 */

#ifndef LH_MATH_H
#define LH_MATH_H

#include <lh/math/coord.h>
#include <lh/math/point.h>
#include <lh/math/rect.h>
#include <lh/math/size.h>

#endif /* LH_MATH_H */