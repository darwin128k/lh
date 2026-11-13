/**
 * @file fit.h
 * @brief Aspect-preserving, centred integer viewport and its inverse point map.
 *
 * The same rectangle is used for presenting pixels and interpreting input. No
 * allocation or OS. A point may be outside: a captured drag still needs its
 * coordinates there; use lh_math_rect_contains_point to reject an initial press.
 */

#ifndef LH_MATH_FIT_H
#define LH_MATH_FIT_H

#include <lh/core/math/rect.h>

LH_COMPILER_EXTERN_C_BEGIN

/** Make the largest centred source-aspect rectangle inside the destination.
 * Positive extents are required. Rounds down, with at least one pixel on each
 * axis. False for a null output or nonpositive extent; leaves output untouched.
 * Cross-products are evaluated at 64 bits, not at the width of int.
 */
lh_bool_t
lh_math_fit_try_rect(lh_math_rect_t *out, int src_w, int src_h, int dst_w, int dst_h);

/** Checked lh_math_fit_try_rect: a failed operation terminates via runtime check. */
lh_void
lh_math_fit_rect(lh_math_rect_t *out, int src_w, int src_h, int dst_w, int dst_h);

/** Map a destination-space point through a fitted rectangle to source pixels.
 * Truncates toward zero; outside points are mapped too, for captured drags.
 * False for null pointers, nonpositive extents, or an unrepresentable result;
 * leaves output untouched. The rectangle need not start at zero.
 */
lh_bool_t
lh_math_fit_try_map_point(lh_math_point_t *out, const lh_math_rect_t *fit, int src_w, int src_h,
                          lh_math_point_t point);

/** Checked lh_math_fit_try_map_point; terminates when mapping is invalid. */
lh_void
lh_math_fit_map_point(lh_math_point_t *out, const lh_math_rect_t *fit, int src_w, int src_h,
                      lh_math_point_t point);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FIT_H */
