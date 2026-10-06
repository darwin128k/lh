/**
 * @file pen.h
 * @brief Outline: ::lh_ui_pen_t, a paint plus a line width.
 *
 * A pen is its own type, not an alias of ::lh_ui_paint_t, so a fill paint
 * cannot be passed where an outline is expected. Dash and join come later.
 */

#ifndef LH_UI_PEN_H
#define LH_UI_PEN_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/paint.h>
#include <lh/ui/pen/fields.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

/**
 * @struct lh_ui_pen
 * @typedef lh_ui_pen_t
 * @brief An outline: paint and width.
 */
struct lh_ui_pen
{
    lh_ui_pen_fields(lh_ui_paint_t, lh_ui_scalar_t);
};
typedef struct lh_ui_pen lh_ui_pen_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the empty pen: no paint, width `1`.
 */
lh_void
lh_ui_pen_init(lh_ui_pen_t *self);

/**
 * @brief Fill @p self with a copy of @p paint and @p width.
 *
 * ::lh_null @p paint gives the empty paint. @p width must not be negative.
 */
lh_void
lh_ui_pen_init_paint(lh_ui_pen_t *self, const lh_ui_paint_t *paint, lh_ui_scalar_t width);

/**
 * @brief Paint of @p self. Never ::lh_null; may be empty.
 */
const lh_ui_paint_t *
lh_ui_pen_get_paint(const lh_ui_pen_t *self);

/**
 * @brief Replace the paint of @p self with a copy of @p paint.
 *
 * ::lh_null clears it to the empty paint.
 */
lh_void
lh_ui_pen_set_paint(lh_ui_pen_t *self, const lh_ui_paint_t *paint);

/**
 * @brief Line width of @p self.
 */
lh_ui_scalar_t
lh_ui_pen_get_width(const lh_ui_pen_t *self);

/**
 * @brief Replace the line width of @p self. @p width must not be negative.
 */
lh_void
lh_ui_pen_set_width(lh_ui_pen_t *self, lh_ui_scalar_t width);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_PEN_H */
