/**
 * @file pen.h
 * @brief Outline: ::lh_ui_pen_t — the same type as ::lh_ui_paint_t (a color).
 */

#ifndef LH_UI_PEN_H
#define LH_UI_PEN_H

#include <lh/ui/paint.h>

/**
 * @typedef lh_ui_pen_t
 * @brief An outline. Alias of ::lh_ui_paint_t.
 */
typedef lh_ui_paint_t lh_ui_pen_t;

/**
 * @def lh_ui_pen_make_empty
 * @brief The empty pen. Same as ::lh_ui_paint_make_empty.
 */
#define lh_ui_pen_make_empty lh_ui_paint_make_empty

/**
 * @def lh_ui_pen_init
 * @brief Fill @p self with the empty pen. Same as ::lh_ui_paint_init.
 */
#define lh_ui_pen_init lh_ui_paint_init

#endif /* LH_UI_PEN_H */
