/**
 * @file round.h
 * @brief How much of a pixel a rounded rectangle covers, in fixed point.
 *
 * The anti-aliased edge of a rounded corner, as a coverage 0..255 per pixel.
 * Everything is in fixed point — ::LH_PIXEL_ROUND_SUBPIXEL units per pixel — so
 * it needs no FPU and gives the same answer for an integer and a float scalar
 * above it. A pixel whose distance from the corner's centre is wholly inside or
 * outside the one-pixel ramp of the edge is decided without a square root; only
 * the pixels on the ramp take one (::lh_math_isqrt_u64).
 *
 * The rectangle is given by its edges in fixed point (`lo` and `hi` on each
 * axis), the radius likewise; a renderer — gl's canvas — converts its own scalar
 * once and asks here.
 */

#ifndef LH_PIXEL_ROUND_H
#define LH_PIXEL_ROUND_H

#include <lh/core/byte.h>
#include <lh/core/compiler/extern/c.h>

/**
 * @def LH_PIXEL_ROUND_SUBPIXEL
 * @brief Fixed-point units in one pixel.
 */
#define LH_PIXEL_ROUND_SUBPIXEL 256

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The centre of pixel @p i, in fixed point.
 *
 * @param i A pixel column or row.
 * @return `i * SUBPIXEL + SUBPIXEL / 2`.
 */
lh_s64_t
lh_pixel_round_pixel_center(lh_s32_t i);

/**
 * @brief How far @p p lies into a corner zone of the span `lo .. hi` with radius
 *        @p r: `0` in the straight middle, growing towards an end, `-1` outside.
 *
 * @param p  A point on the axis, fixed point.
 * @param lo The span's start, fixed point.
 * @param hi The span's end, fixed point.
 * @param r  The radius, fixed point.
 * @return The distance into the nearer corner zone, or `-1` when @p p is outside.
 */
lh_s64_t
lh_pixel_round_axis_distance(lh_s64_t p, lh_s64_t lo, lh_s64_t hi, lh_s64_t r);

/**
 * @brief The coverage of a pixel whose centre is @p d from the corner's centre,
 *        on a circle of radius @p r: a one-pixel ramp across the edge.
 *
 * @param r The radius, fixed point.
 * @param d The distance, fixed point.
 * @return 0..255.
 */
lh_byte_t
lh_pixel_round_cover_from_distance(lh_s64_t r, lh_s64_t d);

/**
 * @brief ::lh_pixel_round_cover_from_distance from the square of the distance,
 *        taking a square root only on the ramp.
 *
 * @param r  The radius, fixed point.
 * @param d2 The squared distance, fixed point squared.
 * @return 0..255.
 */
lh_byte_t
lh_pixel_round_cover_from_square(lh_s64_t r, lh_s64_t d2);

/**
 * @brief How much of pixel (@p x, @p y) the rounded rectangle covers.
 *
 * @param lo_x Left edge, fixed point.
 * @param lo_y Top edge, fixed point.
 * @param hi_x Right edge, fixed point.
 * @param hi_y Bottom edge, fixed point.
 * @param r    The radius, fixed point; at most half the shorter side.
 * @param x    Pixel column.
 * @param y    Pixel row.
 * @return 0 outside, 255 inside, the ramp between on the rounded edge.
 */
lh_byte_t
lh_pixel_round_coverage(lh_s64_t lo_x, lh_s64_t lo_y, lh_s64_t hi_x, lh_s64_t hi_y, lh_s64_t r,
                        lh_s32_t x, lh_s32_t y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_PIXEL_ROUND_H */
