/**
 * @file gradient.h
 * @brief gl's side of a gradient: its colours and its boxes in gl's own types.
 *
 * The gradient itself — the stops, the shape, where a pixel falls and what colour
 * that is — lives in the core (`lh/core/pixel/gradient.h`, ::lh_pixel_gradient_t),
 * in whole numbers and `0xAARRGGBB` colours, and gl uses it as it is. What gl
 * adds here is only the step from its own types: a colour is an
 * ::lh_gl_color_t, and a box an ::lh_gl_rect_t that may be fractional, laid
 * over whole pixels from its floor (::lh_gl_gradient_get_box_px).
 */

#ifndef LH_GL_GRADIENT_H
#define LH_GL_GRADIENT_H

#include <lh/core/pixel/gradient.h>
#include <lh/gl/color.h>
#include <lh/gl/rect.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The whole pixels a gradient over @p rect is laid on: the floor of its
 *        origin and of its size.
 *
 * @param rect The box; must not be null.
 * @param out  The box in whole pixels; must not be null.
 */
lh_void
lh_gl_gradient_get_box_px(const lh_gl_rect_t *rect, lh_math_rect_t *out);

/**
 * @brief ::lh_pixel_gradient_init_stops with gl's colours: each is taken as
 *        `0xAARRGGBB` (::lh_gl_color_get_argb).
 *
 * @param self      The gradient; checked by ::lh_pixel_gradient_init_stops.
 * @param colors    @p num_stops colours; checked by ::lh_assert_runtime_ref. Copied.
 * @param fracs     The positions, in order, or ::lh_null to spread them evenly.
 * @param num_stops `1 ..` ::LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS; checked by
 *                  ::lh_pixel_gradient_init_stops.
 */
lh_void
lh_gl_gradient_init_stops(lh_pixel_gradient_t *self, const lh_gl_color_t *colors,
                          lh_byte_cptr fracs, lh_pixel_gradient_stop_count_t num_stops);

/**
 * @brief ::lh_pixel_gradient_get_frac_at over gl's @p rect, laid on its whole
 *        pixels (::lh_gl_gradient_get_box_px).
 *
 * @param self The gradient; must not be null.
 * @param rect The box; must not be null.
 * @param x    The pixel's column.
 * @param y    The pixel's row.
 * @return The position, `0 .. 255`.
 */
lh_byte_t
lh_gl_gradient_frac_at(const lh_pixel_gradient_t *self, const lh_gl_rect_t *rect, lh_s32_t x,
                       lh_s32_t y);

/**
 * @brief ::lh_pixel_gradient_get_color_at as gl's colour.
 *
 * @param self The gradient; must not be null.
 * @param frac A position, `0 .. 255`.
 * @param out  The colour; must not be null.
 */
lh_void
lh_gl_gradient_color_at(const lh_pixel_gradient_t *self, lh_byte_t frac, lh_gl_color_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_GRADIENT_H */
