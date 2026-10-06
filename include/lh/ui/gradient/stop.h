/**
 * @file stop.h
 * @brief One gradient stop: ::lh_ui_gradient_stop_t (color + frac `0..255`).
 */

#ifndef LH_UI_GRADIENT_STOP_H
#define LH_UI_GRADIENT_STOP_H

#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient/stop/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_gradient_stop
 * @typedef lh_ui_gradient_stop_t
 * @brief A color on the virtual gradient axis `0..255`.
 */
struct lh_ui_gradient_stop
{
    lh_ui_gradient_stop_fields(lh_ui_color_t, lh_byte_t);
};
typedef struct lh_ui_gradient_stop lh_ui_gradient_stop_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Make a stop from @p color at position @p frac (`0..255`).
 */
lh_ui_gradient_stop_t
lh_ui_gradient_stop_make(lh_ui_color_t color, lh_byte_t frac);

/**
 * @brief Color of @p self.
 */
const lh_ui_color_t *
lh_ui_gradient_stop_get_color_as_const(const lh_ui_gradient_stop_t *self);

/**
 * @brief Mutable color of @p self.
 */
lh_ui_color_t *
lh_ui_gradient_stop_get_color(lh_ui_gradient_stop_t *self);

/**
 * @brief Position of @p self on the `0..255` axis.
 */
lh_byte_t
lh_ui_gradient_stop_get_frac(const lh_ui_gradient_stop_t *self);

/**
 * @brief Replace the color of @p self.
 */
lh_void
lh_ui_gradient_stop_set_color(lh_ui_gradient_stop_t *self, lh_ui_color_t color);

/**
 * @brief Replace the frac of @p self.
 */
lh_void
lh_ui_gradient_stop_set_frac(lh_ui_gradient_stop_t *self, lh_byte_t frac);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GRADIENT_STOP_H */
