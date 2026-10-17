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

#include <lh/ui/gradient.h>
#include <lh/ui/gradient/sampler/fields.h>

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

/**
 * @struct lh_ui_gradient_sampler_lerp
 * @brief A colour between two stops walked one step of the axis at a time:
 *        each channel `(from * (span - at) + to * at) / span` as a quotient and
 *        remainder, and what one step adds to both.
 */
struct lh_ui_gradient_sampler_lerp
{
    lh_s32_t quotient[4];  /**< Channel values r, g, b, a. */
    lh_s32_t rest[4];      /**< Their remainders, `0 .. span - 1`. */
    lh_s32_t step[4];      /**< What one step adds to each quotient. */
    lh_s32_t step_rest[4]; /**< And to each remainder. */
    lh_s32_t span;         /**< Steps between the two stops (above zero). */
};
/** @copybrief lh_ui_gradient_sampler_lerp */
typedef struct lh_ui_gradient_sampler_lerp lh_ui_gradient_sampler_lerp_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Start a walk @p at steps into the @p span steps from @p from to
 *        @p to (`span > 0`). @p self must not be null (::lh_assert_runtime_ref).
 */
lh_void
lh_ui_gradient_sampler_lerp_init(lh_ui_gradient_sampler_lerp_t *self, const lh_ui_color_t *from,
                                 const lh_ui_color_t *to, lh_s32_t span, lh_s32_t at);

/**
 * @brief Move the walk one step on.
 */
lh_void
lh_ui_gradient_sampler_lerp_next(lh_ui_gradient_sampler_lerp_t *self);

/**
 * @brief The colour the walk is at, into @p out.
 */
lh_void
lh_ui_gradient_sampler_lerp_color(const lh_ui_gradient_sampler_lerp_t *self, lh_ui_color_t *out);

/**
 * @brief Axis position (`0..255`) of stop @p index of @p gradient.
 */
lh_s32_t
lh_ui_gradient_sampler_stop_frac(const lh_ui_gradient_t *gradient, lh_s32_t index);

/**
 * @brief The first stop from @p segment on that is at or past @p frac (the
 *        stop count when none is).
 */
lh_s32_t
lh_ui_gradient_sampler_segment_at(const lh_ui_gradient_t *gradient, lh_s32_t segment,
                                  lh_s32_t frac);

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

/**
 * @brief True when every pixel of @p self is its first colour: an empty box,
 *        or a linear gradient of no length.
 */
lh_bool_t
lh_ui_gradient_sampler_is_flat(const lh_ui_gradient_sampler_t *self);

/**
 * @brief @p count pixels of the first colour of @p self into @p out.
 */
lh_void
lh_ui_gradient_sampler_flat_run(const lh_ui_gradient_sampler_t *self, lh_s32_t count,
                                lh_ui_color_t *out);

/**
 * @brief @p count pixels of a linear gradient into @p out, from the pixel
 *        whose doubled offset from the middle is (@p dx, @p dy), each next
 *        pixel moving the axis by @p move (`4 * sine * 255` along a row,
 *        `-4 * cosine * 255` down a column).
 */
lh_void
lh_ui_gradient_sampler_linear_run(const lh_ui_gradient_sampler_t *self, lh_s64_t dx, lh_s64_t dy,
                                  lh_s64_t move, lh_s32_t count, lh_ui_color_t *out);

/**
 * @brief Pixels @p x0 .. @p x1 - 1 of a radial gradient's row whose doubled
 *        distance from the middle is @p dy into @p out: the half from the
 *        middle on walked, the other half mirrored.
 */
lh_void
lh_ui_gradient_sampler_radial_row(const lh_ui_gradient_sampler_t *self, lh_s32_t x0, lh_s32_t x1,
                                  lh_s64_t dy, lh_ui_color_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GRADIENT_SAMPLER_H */
