/**
 * @file gradient.h
 * @brief A two-stop gradient: ::lh_ui_gradient_t.
 *
 * Linear runs from the first stop to the second across the rectangle, on
 * ::lh_ui_gradient_horizontal or ::lh_ui_gradient_vertical. Radial runs from
 * the center to the corner. Angular runs clockwise from the right, one full
 * turn.
 */

#ifndef LH_UI_GRADIENT_H
#define LH_UI_GRADIENT_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/color.h>
#include <lh/void.h>
#include <lh/ui/gradient/fields.h>

/**
 * @enum lh_ui_gradient_kind
 * @brief Linear, radial, or angular.
 */
typedef enum lh_ui_gradient_kind
{
    lh_ui_gradient_linear = 0,
    lh_ui_gradient_radial,
    lh_ui_gradient_angular
} lh_ui_gradient_kind_t;

/**
 * @enum lh_ui_gradient_axis
 * @brief Which way a linear gradient runs.
 */
typedef enum lh_ui_gradient_axis
{
    lh_ui_gradient_horizontal = 0,
    lh_ui_gradient_vertical
} lh_ui_gradient_axis_t;

/**
 * @struct lh_ui_gradient
 * @typedef lh_ui_gradient_t
 * @brief From one color to another.
 */
struct lh_ui_gradient
{
    lh_ui_gradient_fields(lh_ui_color_t, lh_ui_gradient_kind_t, lh_ui_gradient_axis_t);
};
typedef struct lh_ui_gradient lh_ui_gradient_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with a linear gradient from @p from to @p to along @p axis.
 */
lh_void
lh_ui_gradient_init_linear(lh_ui_gradient_t *self, const lh_ui_color_t *from, const lh_ui_color_t *to,
                           lh_ui_gradient_axis_t axis);

/**
 * @brief Fill @p self with a radial gradient from @p from at the center to @p to at the corner.
 */
lh_void
lh_ui_gradient_init_radial(lh_ui_gradient_t *self, const lh_ui_color_t *from, const lh_ui_color_t *to);

/**
 * @brief Fill @p self with an angular gradient from @p from to @p to, clockwise from the right.
 */
lh_void
lh_ui_gradient_init_angular(lh_ui_gradient_t *self, const lh_ui_color_t *from, const lh_ui_color_t *to);

/**
 * @brief First stop of @p self.
 */
const lh_ui_color_t *
lh_ui_gradient_get_from(const lh_ui_gradient_t *self);

/**
 * @brief Second stop of @p self.
 */
const lh_ui_color_t *
lh_ui_gradient_get_to(const lh_ui_gradient_t *self);

/**
 * @brief Kind of @p self.
 */
lh_ui_gradient_kind_t
lh_ui_gradient_get_kind(const lh_ui_gradient_t *self);

/**
 * @brief Axis of a linear @p self.
 */
lh_ui_gradient_axis_t
lh_ui_gradient_get_axis(const lh_ui_gradient_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GRADIENT_H */
