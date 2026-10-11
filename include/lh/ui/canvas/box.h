/**
 * @file box.h
 * @brief ::lh_ui_canvas_fill_box: a box with a radius for each corner and a paint that
 *        may be a gradient.
 */

#ifndef LH_UI_CANVAS_BOX_H
#define LH_UI_CANVAS_BOX_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/canvas.h>
#include <lh/ui/paint.h>
#include <lh/ui/radii.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p rect with @p paint, each corner rounded by its own radius in
 *        @p radii (fitted to the box as CSS fits them, ::lh_ui_radii_clamp).
 *
 * A solid paint with four equal radii is ::lh_ui_canvas_fill_round_rect -- the
 * backend's own rounded fill -- and costs what it always did. A gradient or unequal
 * corners are drawn by the canvas from `fill_rect` alone, so every backend draws them
 * the same: straight parts as spans, corners pixel by pixel at their coverage. An
 * empty paint draws nothing. @p rect is moved by the offset and cut by the clip, like
 * every other fill.
 */
lh_void
lh_ui_canvas_fill_box(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, const lh_ui_radii_t *radii,
                      const lh_ui_paint_t *paint);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_BOX_H */
