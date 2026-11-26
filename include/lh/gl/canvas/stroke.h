/**
 * @file stroke.h
 * @brief An inset rounded border painted without covering the box's interior.
 *
 * Composes the existing radius coverage and canvas colour-run primitives. The
 * caller supplies the shape once; the inner contour is derived from it here.
 * Offsets and clipping follow the same contract as canvas_fill_box.
 */

#ifndef LH_GL_CANVAS_STROKE_H
#define LH_GL_CANVAS_STROKE_H

#include <lh/gl/canvas/round.h>
#include <lh/gl/radii.h>

struct lh_gl_radius_run;

LH_COMPILER_EXTERN_C_BEGIN

/** Draw a target-space row of a border from prepared outer/inner corner pairs.
 * Each pair holds left and right runs for the row's half of the shape. The
 * inner pair may be null for a box consumed by its border. Pixels outside
 * x0..x1 are untouched; middle is the shape's horizontal midpoint.
 */
lh_void
lh_gl_canvas_stroke_run(struct lh_gl_canvas *self, const struct lh_gl_radius_run *outer,
                        const struct lh_gl_radius_run *inner, const lh_gl_color_t *color,
                        lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_s32_t middle);

/** The part of row @p y a border leaves to its @p hole, cut to `x0..x1`, into
 * @p inside0 and @p inside1. A row the hole does not reach leaves nothing (both
 * @p x1). With @p exact edges (::lh_gl_canvas_round_is_exact) the span is the
 * pixels the hole covers wholly at the corner radius @p maximum
 * (::lh_gl_radius_full_span); without, the hole's sides less @p maximum.
 */
lh_void
lh_gl_canvas_stroke_hole_span(const lh_gl_rect_t *hole, lh_gl_scalar_t maximum, lh_bool_t exact,
                              lh_s32_t y, lh_s32_t x0, lh_s32_t x1, lh_s32_t *inside0,
                              lh_s32_t *inside1);

/** Paint a uniform inset border with independently rounded corners.
 * Width <= 0 or empty geometry paints nothing. Radii are clamped to the box;
 * the inner radii are max(outer-width,0). A width consuming the box fills its
 * entire rounded shape. The colour's alpha and the canvas clip are respected.
 * Geometry is in the caller's coordinate space. No allocation or background
 * repaint is performed, so transparent interiors stay untouched.
 */
lh_void
lh_gl_canvas_stroke_box(struct lh_gl_canvas *self, const lh_gl_rect_t *rect,
                        const lh_gl_radii_t *radii, lh_gl_scalar_t width,
                        const lh_gl_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_CANVAS_STROKE_H */
