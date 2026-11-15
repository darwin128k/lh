/**
 * @file cover.h
 * @brief Whether a pixel's center lies in a shape: ::lh_gl_cover_round_rect,
 *        ::lh_gl_cover_circle, ::lh_gl_cover_triangle.
 *
 * The answer is `255` when the center is inside and `0` when it is outside.
 * A rounded rectangle with radius `0` is the rectangle. A radius of half the
 * shorter side is a pill, and a square of that radius is a circle. The edge
 * is the pixel center, in whole numbers, with no partial coverage.
 */

#ifndef LH_GL_COVER_H
#define LH_GL_COVER_H

#include <lh/gl/color.h>
#include <lh/gl/rect.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p radius clamped the way a rounded rectangle uses it.
 *
 * A negative radius is 0. A radius larger than half the shorter side is that
 * half. The three rounded-rectangle tests share this, so a fill and a coverage
 * query cannot disagree about where the arc starts.
 */
lh_gl_scalar_t
lh_gl_cover_round_radius(lh_gl_scalar_t width, lh_gl_scalar_t height, lh_gl_scalar_t radius);

/**
 * @brief `255` when the center of pixel (@p x, @p y) lies in @p rect with
 *        corners of @p radius. `0` otherwise.
 *
 * @p radius is limited to half the shorter side. A negative radius is zero.
 * Outside @p rect is `0`.
 */
lh_gl_color_channel_t
lh_gl_cover_round_rect(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_gl_scalar_t x,
                       lh_gl_scalar_t y);

/**
 * @brief Coverage of pixel (@p x, @p y) in a rounded rectangle, 0 through 255.
 *
 * The flat interior is 255. Outside the rectangle is 0. The arc fades across
 * one pixel. A radius of 0 is 255 inside the rectangle.
 */
lh_gl_color_channel_t
lh_gl_cover_round_rect_band(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_gl_scalar_t x,
                            lh_gl_scalar_t y);

/**
 * @brief True when (@p x, @p y) is inside @p rect grown by @p slop, following
 *        a corner of @p radius also grown by @p slop.
 *
 * @p slop below 0 is 0. A radius of 0 is the rectangle.
 */
lh_bool_t
lh_gl_cover_hits(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_gl_scalar_t slop,
                 lh_gl_scalar_t x, lh_gl_scalar_t y);

/**
 * @brief Half-open horizontal run of pixels on row @p y that
 *        ::lh_gl_cover_round_rect covers.
 *
 * Writes [@p x0, @p x1). Returns false when the row misses the shape, and then
 * leaves @p x0 and @p x1 untouched.
 */
lh_bool_t
lh_gl_cover_round_rect_span(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_gl_scalar_t y,
                            lh_gl_scalar_t *x0, lh_gl_scalar_t *x1);

/**
 * @brief `255` when the center of pixel (@p x, @p y) lies in the circle.
 *        A radius below 1 covers nothing.
 */
lh_gl_color_channel_t
lh_gl_cover_circle(const lh_gl_point_t *center, lh_gl_scalar_t radius, lh_gl_scalar_t x,
                   lh_gl_scalar_t y);

/**
 * @brief `255` when the center of pixel (@p x, @p y) lies in triangle
 *        @p a @p b @p c, either winding. A triangle with no area covers nothing.
 *        The edge counts as inside.
 */
lh_gl_color_channel_t
lh_gl_cover_triangle(const lh_gl_point_t *a, const lh_gl_point_t *b, const lh_gl_point_t *c,
                     lh_gl_scalar_t x, lh_gl_scalar_t y);

/**
 * @brief `255` when pixel (@p x, @p y) lies in a sector of the circle.
 *
 * @p start is degrees from the right, toward +y. A positive @p sweep turns
 * that way. Both edges count as inside. A sweep of 0 covers nothing. A sweep
 * of 360 or more, either way, is the whole circle. A radius below 1 covers
 * nothing. The coordinates stay integers.
 */
lh_gl_color_channel_t
lh_gl_cover_sector(const lh_gl_point_t *center, lh_gl_scalar_t radius, lh_gl_scalar_t start,
                   lh_gl_scalar_t sweep, lh_gl_scalar_t x, lh_gl_scalar_t y);

/**
 * @brief Unit direction of @p degree, scaled by 1024.
 *
 * 0 is +x. 90 is +y. The values are written to @p x and @p y. Both must not
 * be null. The scale is the quarter-degree sine table, so 90 is exactly 1024.
 */
lh_void
lh_gl_cover_degree_direction(lh_gl_scalar_t degree, lh_s32_t *x, lh_s32_t *y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_COVER_H */
