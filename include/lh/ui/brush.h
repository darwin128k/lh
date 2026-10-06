/**
 * @file brush.h
 * @brief Fill: ::lh_ui_brush_t — the same type as ::lh_ui_paint_t.
 *
 * A fill needs nothing beyond the paint, so the brush stays an alias. An
 * outline needs a width and is its own type, ::lh_ui_pen_t.
 */

#ifndef LH_UI_BRUSH_H
#define LH_UI_BRUSH_H

#include <lh/ui/paint.h>

/**
 * @typedef lh_ui_brush_t
 * @brief A fill. Alias of ::lh_ui_paint_t.
 */
typedef lh_ui_paint_t lh_ui_brush_t;

/**
 * @def lh_ui_brush_init
 * @brief Fill @p self with the empty brush. Same as ::lh_ui_paint_init.
 */
#define lh_ui_brush_init lh_ui_paint_init

#endif /* LH_UI_BRUSH_H */
