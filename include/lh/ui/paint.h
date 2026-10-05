/**
 * @file paint.h
 * @brief The color of a brush or a pen: ::lh_ui_paint_t.
 *
 * It is either one ::lh_ui_color_t or one ::lh_ui_gradient_t.
 */

#ifndef LH_UI_PAINT_H
#define LH_UI_PAINT_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/color.h>
#include <lh/void.h>
#include <lh/ui/gradient.h>
#include <lh/ui/paint/fields.h>

/**
 * @enum lh_ui_paint_kind
 * @brief Which value a ::lh_ui_paint_t holds.
 */
typedef enum lh_ui_paint_kind
{
    lh_ui_paint_solid = 0,
    lh_ui_paint_gradient
} lh_ui_paint_kind_t;

/**
 * @struct lh_ui_paint
 * @typedef lh_ui_paint_t
 * @brief A solid color or a gradient.
 */
struct lh_ui_paint
{
    lh_ui_paint_fields(lh_ui_paint_kind_t, lh_ui_color_t, lh_ui_gradient_t);
};
typedef struct lh_ui_paint lh_ui_paint_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the solid @p color.
 */
lh_void
lh_ui_paint_init(lh_ui_paint_t *self, const lh_ui_color_t *color);

/**
 * @brief Fill @p self with @p gradient.
 */
lh_void
lh_ui_paint_init_gradient(lh_ui_paint_t *self, const lh_ui_gradient_t *gradient);

/**
 * @brief Which value @p self holds.
 */
lh_ui_paint_kind_t
lh_ui_paint_get_kind(const lh_ui_paint_t *self);

/**
 * @brief Solid color of @p self. @p self must be ::lh_ui_paint_solid.
 */
const lh_ui_color_t *
lh_ui_paint_get_color(const lh_ui_paint_t *self);

/**
 * @brief Gradient of @p self. @p self must be ::lh_ui_paint_gradient.
 */
const lh_ui_gradient_t *
lh_ui_paint_get_gradient(const lh_ui_paint_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_PAINT_H */
