/**
 * @file sampler.h
 * @brief A gradient laid over one box, read a row at a time:
 *        ::lh_ui_gradient_sampler_t.
 *
 * ::lh_ui_gradient_frac_at and ::lh_ui_gradient_color_at answer for one pixel, and
 * between them a pixel paid a division, a square root for a radial gradient, and a
 * walk over the stops with four more divisions. Over a row none of that has to be
 * paid again: the place on the stop axis moves by the same step from one pixel to the
 * next (a remainder carried along keeps it exact), a radial one crosses from one step
 * of the axis to the next at fixed squared distances, and a step's colour is the same
 * wherever it falls, so it is worked out once. The colours are those two functions'
 * to the last bit.
 */

#ifndef LH_UI_GRADIENT_SAMPLER_H
#define LH_UI_GRADIENT_SAMPLER_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient.h>
#include <lh/ui/gradient/sampler/fields.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

/**
 * @struct lh_ui_gradient_sampler
 * @brief A gradient over one box.
 */
struct lh_ui_gradient_sampler
{
    /** Members: ::lh_ui_gradient_sampler_fields. */
    lh_ui_gradient_sampler_fields(lh_ui_gradient_t, lh_ui_color_t, lh_ui_gradient_shape_t);
};
/** @copybrief lh_ui_gradient_sampler */
typedef struct lh_ui_gradient_sampler lh_ui_gradient_sampler_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p gradient laid over @p rect. @p gradient is read as rows are asked for, so
 *        it has to outlive @p self unchanged.
 */
lh_void
lh_ui_gradient_sampler_init(lh_ui_gradient_sampler_t *self, const lh_ui_gradient_t *gradient,
                            const lh_ui_rect_t *rect);

/**
 * @brief The colours of pixels @p x0 .. @p x1 - 1 of row @p y into @p out: for each,
 *        ::lh_ui_gradient_color_at of ::lh_ui_gradient_frac_at.
 */
lh_void
lh_ui_gradient_sampler_row(lh_ui_gradient_sampler_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                           lh_ui_color_t *out);

/**
 * @brief The colours of pixels @p y0 .. @p y1 - 1 of column @p x into @p out, the
 *        same as ::lh_ui_gradient_sampler_row would give them a row at a time: what
 *        a gradient that runs straight down is drawn from.
 */
lh_void
lh_ui_gradient_sampler_column(lh_ui_gradient_sampler_t *self, lh_s32_t x, lh_s32_t y0, lh_s32_t y1,
                              lh_ui_color_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GRADIENT_SAMPLER_H */
