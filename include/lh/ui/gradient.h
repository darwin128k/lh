/**
 * @file gradient.h
 * @brief Color gradient: ::lh_ui_gradient_t, a fixed table of stops.
 *
 * Capacity is ::LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS (default `2`, same
 * idea as LVGL `LV_GRADIENT_MAX_STOPS`). Alpha lives on each
 * ::lh_ui_color_t — there is no separate opacity array.
 *
 * How the stops lie over a box is the gradient's too, the way CSS has it:
 * along a line at an angle through the middle (`linear-gradient(180deg, ...)`, 0 up
 * and 90 to the right), as long as `|w sin a| + |h cos a|` so the corners are the
 * ends; or out from the middle to the farthest corner on an ellipse the shape of the
 * box (`radial-gradient(...)`). ::lh_ui_gradient_frac_at says where on the stop axis
 * a pixel is and ::lh_ui_gradient_color_at what colour that is -- in whole numbers,
 * so a build with no FPU draws the same gradient.
 */

#ifndef LH_UI_GRADIENT_H
#define LH_UI_GRADIENT_H

#include <lh/byte/ptr.h>
#include <lh/config.h>
#include <lh/ui/gradient/fields.h>
#include <lh/ui/gradient/stop.h>
#include <lh/ui/gradient/stop/count.h>
#include <lh/ui/rect.h>
#include <lh/ui/gradient/shape.h>

/**
 * @struct lh_ui_gradient
 * @brief Ordered color stops on a `0..255` axis.
 */
struct lh_ui_gradient
{
    /** Members: ::lh_ui_gradient_fields. */
    lh_ui_gradient_fields(lh_ui_gradient_stop_t, lh_ui_gradient_stop_count_t,
                          lh_ui_gradient_shape_t, lh_s16_t);
};
/** @copybrief lh_ui_gradient */
typedef struct lh_ui_gradient lh_ui_gradient_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Clear @p self to zero stops, linear at 180 degrees (top to bottom), which is
 *        what CSS draws when a gradient names no direction.
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
lh_ui_gradient_init_stops(lh_ui_gradient_t *self, const lh_ui_color_t *colors,
                          const lh_byte_ptr fracs, lh_ui_gradient_stop_count_t num_stops);

/**
 * @brief Number of used stops in @p self.
 */
lh_ui_gradient_stop_count_t
lh_ui_gradient_get_stop_count(const lh_ui_gradient_t *self);

/**
 * @brief Const pointer to stop @p index (`0 .. count - 1`).
 */
const lh_ui_gradient_stop_t *
lh_ui_gradient_get_stop(const lh_ui_gradient_t *self, lh_ui_gradient_stop_count_t index);

/**
 * @brief Mutable pointer to stop @p index (`0 .. count - 1`).
 */
lh_ui_gradient_stop_t *
lh_ui_gradient_get_stop_mut(lh_ui_gradient_t *self, lh_ui_gradient_stop_count_t index);

/**
 * @brief Lay @p self along a line at @p degrees: 0 up, 90 right, 180 down, as CSS.
 */
lh_void
lh_ui_gradient_set_linear(lh_ui_gradient_t *self, lh_s32_t degrees);

/**
 * @brief Lay @p self out from the middle of a box to its farthest corner.
 */
lh_void
lh_ui_gradient_set_radial(lh_ui_gradient_t *self);

/**
 * @brief How @p self lies over a box.
 */
lh_ui_gradient_shape_t
lh_ui_gradient_get_shape(const lh_ui_gradient_t *self);

/**
 * @brief The angle of a linear @p self, in degrees `0 .. 359`.
 */
lh_s32_t
lh_ui_gradient_get_angle(const lh_ui_gradient_t *self);

/**
 * @brief Where the pixel (@p x, @p y) is on the stop axis of @p self laid over
 *        @p rect, `0 .. 255`: the first stop's end of the box is 0.
 */
lh_byte_t
lh_ui_gradient_frac_at(const lh_ui_gradient_t *self, const lh_ui_rect_t *rect, lh_s32_t x,
                       lh_s32_t y);

/**
 * @brief The colour of @p self at @p frac on its stop axis: the stops either side
 *        mixed in proportion, the first stop's colour before it and the last's after.
 */
lh_void
lh_ui_gradient_color_at(const lh_ui_gradient_t *self, lh_byte_t frac, lh_ui_color_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GRADIENT_H */
