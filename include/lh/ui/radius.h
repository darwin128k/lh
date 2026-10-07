/**
 * @file radius.h
 * @brief Corner radius: clamp and anti-aliased coverage, once for everyone.
 *
 * Every rounded fill goes through these two functions. ::lh_ui_canvas_t
 * clamps before a backend sees the radius, and its fallback uses the
 * coverage, so no backend repeats the math. Works without an FPU: both are
 * fixed point, whatever ::lh_ui_scalar_t is.
 */

#ifndef LH_UI_RADIUS_H
#define LH_UI_RADIUS_H

#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>

/**
 * @def LH_UI_RADIUS_CIRCLE
 * @brief The largest radius: ::lh_ui_radius_clamp turns it into half the
 *        short side (a pill, or a circle for a square). Same idea as LVGL
 *        `LV_RADIUS_CIRCLE`.
 */
#define LH_UI_RADIUS_CIRCLE lh_ui_scalar(0x7FFF)

/**
 * @def LH_UI_RADIUS_SUBPIXEL
 * @brief Fixed-point units per pixel in the coverage math. 256 keeps squared
 *        distances of screens up to ~65k pixels inside 64 bits and gives a
 *        256-step ramp along the arc.
 */
#define LH_UI_RADIUS_SUBPIXEL 256

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p v (pixels, either scalar) in ::LH_UI_RADIUS_SUBPIXEL units.
 */
lh_s64_t
lh_ui_radius_to_fixed(lh_ui_scalar_t v);

/**
 * @brief Center of pixel @p i along one axis, in fixed-point units.
 */
lh_s64_t
lh_ui_radius_pixel_center(lh_s32_t i);

/**
 * @brief Largest radius @p rect can take: half its short side (`0` if empty).
 */
lh_ui_scalar_t
lh_ui_radius_get_max(const lh_ui_rect_t *rect);

/**
 * @brief How deep fixed-point @p p lies in a corner zone along one axis.
 *
 * The axis runs from @p start for @p length pixels; @p r is the fixed-point
 * radius. Returns `-1` outside the span, `0` on the straight part between the
 * corners, otherwise the distance from the corner circle center.
 */
lh_s64_t
lh_ui_radius_axis_distance(lh_s64_t p, lh_ui_scalar_t start, lh_ui_scalar_t length, lh_s64_t r);

/**
 * @brief Coverage `0..255` of a pixel whose center is @p d from the corner
 *        circle center (fixed point): a one-pixel ramp centered on @p r.
 */
lh_byte_t
lh_ui_radius_cover_from_distance(lh_s64_t r, lh_s64_t d);

/**
 * @def LH_UI_RADIUS_HIT_COVERAGE
 * @brief Least coverage of the pixel under a point that counts as inside a
 *        rounded rect for hit testing: half (the edge pixel is more shape
 *        than background).
 */
#define LH_UI_RADIUS_HIT_COVERAGE 128

/**
 * @brief True when @p point lies in @p rect and, with @p radius (unclamped)
 *        rounding its corners, on a pixel at least ::LH_UI_RADIUS_HIT_COVERAGE
 *        covered: the shape a rounded fill paints, for hit testing.
 */
lh_bool_t
lh_ui_radius_contains(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_ui_point_t point);

/**
 * @brief Two coverages `0..255` combined: `a * b / 255`, rounded (what two
 *        stacked masks let through).
 */
lh_byte_t
lh_ui_radius_scale(lh_byte_t a, lh_byte_t b);

/**
 * @brief ::lh_ui_radius_cover_from_distance for the squared distance @p d2:
 *        `255` / `0` without a square root when the pixel lies wholly inside
 *        / outside the one-pixel ramp (the same answer), the root otherwise.
 */
lh_byte_t
lh_ui_radius_cover_from_square(lh_s64_t r, lh_s64_t d2);

/**
 * @brief @p radius limited to `0 .. min(width, height) / 2` of @p rect.
 *
 * Negative gives `0`. ::LH_UI_RADIUS_CIRCLE gives half the short side.
 */
lh_ui_scalar_t
lh_ui_radius_clamp(const lh_ui_rect_t *rect, lh_ui_scalar_t radius);

/**
 * @brief How much of pixel (@p x, @p y) a rounded @p rect covers: `0..255`.
 *
 * @p radius must already be clamped (::lh_ui_radius_clamp). The pixel is the
 * unit square at (@p x, @p y); its center is sampled against each corner
 * circle with a one-pixel ramp, so the arc is anti-aliased. Pixels inside
 * the rect and outside the corner zones are `255`; outside the rect `0`.
 */
lh_byte_t
lh_ui_radius_coverage(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x, lh_s32_t y);

/**
 * @brief The tightest span of row @p y that the rounded @p rect covers wholly,
 *        written to @p out_x0 / @p out_x1.
 *
 * Every pixel in the returned span has coverage exactly `255` and every pixel
 * outside it (that the rect reaches at all) has less, so a row can be split
 * once here and the two ends sent through the per-pixel coverage path while the
 * span itself is a plain store.
 *
 * This is one square root per row, against the obvious alternative of cutting
 * the corner as a `radius x radius` box and walking all of it per pixel — most
 * of which has coverage `255` and does not need to. Measured on a 240x160 r8
 * rectangle, 16 corner rows cost 350 ns each against 127 ns for a row with no
 * corner at all, and that difference was almost entirely the square's inside.
 *
 * The span is empty (both ends equal, at the left edge) for a row the rect does
 * not reach, and the whole width for a row between the corners.
 */
lh_void
lh_ui_radius_full_span(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y, lh_s32_t *out_x0,
                       lh_s32_t *out_x1);

/**
 * @brief Coverage `0..255` of pixels `x0 .. x1 - 1` of row @p y, written to
 *        @p out `x1 - x0` bytes. The same values as calling
 *        ::lh_ui_radius_coverage per pixel, with the parts that do not change
 *        along a row (the rect's fixed-point edges, the radius, and the vertical
 *        distance) taken once instead of per pixel.
 *
 * @param out Receives `x1 - x0` bytes.
 */
lh_void
lh_ui_radius_coverage_run(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                          lh_byte_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_RADIUS_H */
