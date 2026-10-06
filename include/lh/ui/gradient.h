/**
 * @file gradient.h
 * @brief Color gradient: ::lh_ui_gradient_t, a fixed table of stops.
 *
 * Capacity is ::LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS (default `2`, same
 * idea as LVGL `LV_GRADIENT_MAX_STOPS`). Alpha lives on each
 * ::lh_ui_color_t — there is no separate opacity array. Direction / sampling
 * for a renderer is not here yet; this is the stop map only.
 */

#ifndef LH_UI_GRADIENT_H
#define LH_UI_GRADIENT_H

#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient/fields.h>
#include <lh/ui/gradient/stop.h>
#include <lh/ui/gradient/stop/count.h>
#include <lh/void.h>

/**
 * @struct lh_ui_gradient
 * @typedef lh_ui_gradient_t
 * @brief Ordered color stops on a `0..255` axis.
 */
struct lh_ui_gradient
{
    lh_ui_gradient_fields(lh_ui_gradient_stop_t, lh_ui_gradient_stop_count_t);
};
typedef struct lh_ui_gradient lh_ui_gradient_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty gradient: zero stops.
 */
lh_ui_gradient_t
lh_ui_gradient_make_empty(void);

/**
 * @brief Clear @p self to zero stops.
 */
lh_void
lh_ui_gradient_init(lh_ui_gradient_t *self);

/**
 * @brief Fill @p self from @p colors and optional @p fracs.
 *
 * Copies @p num_stops colors into the fixed table. When @p fracs is
 * ::lh_null, positions are spaced evenly (`0` … `255`). @p num_stops must be
 * in `1..LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS`.
 */
lh_void
lh_ui_gradient_init_stops(lh_ui_gradient_t *self, const lh_ui_color_t *colors, const lh_byte_t *fracs,
                          lh_ui_gradient_stop_count_t num_stops);

/**
 * @brief Number of used stops in @p self.
 */
lh_ui_gradient_stop_count_t
lh_ui_gradient_get_stop_count(const lh_ui_gradient_t *self);

/**
 * @brief Const pointer to stop @p index (`0 .. count - 1`).
 */
const lh_ui_gradient_stop_t *
lh_ui_gradient_get_stop_as_const(const lh_ui_gradient_t *self, lh_ui_gradient_stop_count_t index);

/**
 * @brief Mutable pointer to stop @p index (`0 .. count - 1`).
 */
lh_ui_gradient_stop_t *
lh_ui_gradient_get_stop(lh_ui_gradient_t *self, lh_ui_gradient_stop_count_t index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GRADIENT_H */
