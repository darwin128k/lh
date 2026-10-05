/**
 * @file pen.h
 * @brief The outline of one draw call: ::lh_ui_pen_t, a paint and a width.
 *
 * The paint is a solid color or a gradient. The width is in pixels,
 * inside the rect. A width of 0 paints nothing.
 */

#ifndef LH_UI_PEN_H
#define LH_UI_PEN_H

#include <lh/compiler/extern/c.h>
#include <lh/math/coord.h>
#include <lh/void.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient.h>
#include <lh/ui/paint.h>
#include <lh/ui/pen/fields.h>

/**
 * @struct lh_ui_pen
 * @typedef lh_ui_pen_t
 * @brief An outline: paint and width.
 */
struct lh_ui_pen
{
    lh_ui_pen_fields(lh_ui_paint_t, lh_math_coord_t);
};
typedef struct lh_ui_pen lh_ui_pen_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the solid color @p color and @p width pixels.
 */
lh_void
lh_ui_pen_init(lh_ui_pen_t *self, const lh_ui_color_t *color, lh_math_coord_t width);

/**
 * @brief Fill @p self with @p gradient and @p width pixels.
 */
lh_void
lh_ui_pen_init_gradient(lh_ui_pen_t *self, const lh_ui_gradient_t *gradient, lh_math_coord_t width);

/**
 * @brief Paint of @p self.
 */
const lh_ui_paint_t *
lh_ui_pen_get_paint(const lh_ui_pen_t *self);

/**
 * @brief Solid color of @p self. The paint must be ::lh_ui_paint_solid.
 */
const lh_ui_color_t *
lh_ui_pen_get_color(const lh_ui_pen_t *self);

/**
 * @brief Width of @p self in pixels.
 */
lh_math_coord_t
lh_ui_pen_get_width(const lh_ui_pen_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_PEN_H */
