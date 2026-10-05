/**
 * @file brush.h
 * @brief The fill of one draw call: ::lh_ui_brush_t, one ::lh_ui_paint_t.
 *
 * The paint is a solid color or a gradient. A solid color of alpha 0
 * paints nothing.
 */

#ifndef LH_UI_BRUSH_H
#define LH_UI_BRUSH_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/brush/fields.h>
#include <lh/void.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient.h>
#include <lh/ui/paint.h>

/**
 * @struct lh_ui_brush
 * @typedef lh_ui_brush_t
 * @brief A fill: a solid color or a gradient.
 */
struct lh_ui_brush
{
    lh_ui_brush_fields(lh_ui_paint_t);
};
typedef struct lh_ui_brush lh_ui_brush_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the solid color @p color.
 */
lh_void
lh_ui_brush_init(lh_ui_brush_t *self, const lh_ui_color_t *color);

/**
 * @brief Fill @p self with @p gradient.
 */
lh_void
lh_ui_brush_init_gradient(lh_ui_brush_t *self, const lh_ui_gradient_t *gradient);

/**
 * @brief Paint of @p self.
 */
const lh_ui_paint_t *
lh_ui_brush_get_paint(const lh_ui_brush_t *self);

/**
 * @brief Solid color of @p self. The paint must be ::lh_ui_paint_solid.
 */
const lh_ui_color_t *
lh_ui_brush_get_color(const lh_ui_brush_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_BRUSH_H */
