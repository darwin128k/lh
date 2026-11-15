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

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_COVER_H */
