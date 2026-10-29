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

#include <lh/ui/gradient/sampler/lerp.h>

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

/** The colour of every step of the axis, as ::lh_ui_gradient_color_at has it. Between
   two stops each channel is `(from * (span - at) + to * at) / span`, and one step on
   adds `to - from` to what is divided: the quotient and its remainder are carried
   instead of dividing again (::lh_ui_gradient_sampler_lerp_t). A step before its
   stop's start -- stops out of order -- is asked of ::lh_ui_gradient_color_at itself. */
lh_void
lh_ui_gradient_sampler_fill(lh_ui_gradient_sampler_t *self);

/** The colours of pixels @p x0 .. @p x1 - 1 of a radial gradient's row whose doubled
   distance from the middle is @p dy: `dx * 1024 / w` cut toward zero, carried from
   pixel to pixel as a quotient rounded down and its remainder. The squared distance
   then crosses the steps of the axis at ::lh_ui_gradient_sampler_reach, and the step
   is walked to from the last pixel's. */
lh_void
lh_ui_gradient_sampler_radial_run(const lh_ui_gradient_sampler_t *self, lh_s32_t x0, lh_s32_t x1,
                                  lh_s64_t dy, lh_ui_color_t *out);

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

/**
 * @brief The gradient sampled (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_gradient_t *
lh_ui_gradient_sampler_get_gradient(const lh_ui_gradient_sampler_t *self);

/**
 * @brief Store the gradient sampled (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param gradient The new value.
 */
lh_void
lh_ui_gradient_sampler_set_gradient(lh_ui_gradient_sampler_t *self, const lh_ui_gradient_t * gradient);

/**
 * @brief The colors of the stops, ready to blend (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_color_t *
lh_ui_gradient_sampler_get_colors(const lh_ui_gradient_sampler_t *self);

/**
 * @brief The colors of the stops, ready to blend, to change in place (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_color_t *
lh_ui_gradient_sampler_get_colors_mut(lh_ui_gradient_sampler_t *self);

/**
 * @brief Whether the gradient is linear or radial (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_ui_gradient_shape_t
lh_ui_gradient_sampler_get_shape(const lh_ui_gradient_sampler_t *self);

/**
 * @brief Store whether the gradient is linear or radial (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param shape The new value.
 */
lh_void
lh_ui_gradient_sampler_set_shape(lh_ui_gradient_sampler_t *self, lh_ui_gradient_shape_t shape);

/**
 * @brief The width of the area the gradient fills, fixed point (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_ui_gradient_sampler_get_width(const lh_ui_gradient_sampler_t *self);

/**
 * @brief Store the width of the area the gradient fills, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param width The new value.
 */
lh_void
lh_ui_gradient_sampler_set_width(lh_ui_gradient_sampler_t *self, lh_s64_t width);

/**
 * @brief The height of the area the gradient fills, fixed point (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_ui_gradient_sampler_get_height(const lh_ui_gradient_sampler_t *self);

/**
 * @brief Store the height of the area the gradient fills, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param height The new value.
 */
lh_void
lh_ui_gradient_sampler_set_height(lh_ui_gradient_sampler_t *self, lh_s64_t height);

/**
 * @brief The horizontal middle of the area, fixed point (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_ui_gradient_sampler_get_middle_x(const lh_ui_gradient_sampler_t *self);

/**
 * @brief Store the horizontal middle of the area, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param middle_x The new value.
 */
lh_void
lh_ui_gradient_sampler_set_middle_x(lh_ui_gradient_sampler_t *self, lh_s64_t middle_x);

/**
 * @brief The vertical middle of the area, fixed point (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_ui_gradient_sampler_get_middle_y(const lh_ui_gradient_sampler_t *self);

/**
 * @brief Store the vertical middle of the area, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param middle_y The new value.
 */
lh_void
lh_ui_gradient_sampler_set_middle_y(lh_ui_gradient_sampler_t *self, lh_s64_t middle_y);

/**
 * @brief The sine of the gradient angle, fixed point (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_ui_gradient_sampler_get_sine(const lh_ui_gradient_sampler_t *self);

/**
 * @brief Store the sine of the gradient angle, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param sine The new value.
 */
lh_void
lh_ui_gradient_sampler_set_sine(lh_ui_gradient_sampler_t *self, lh_s64_t sine);

/**
 * @brief The cosine of the gradient angle, fixed point (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_ui_gradient_sampler_get_cosine(const lh_ui_gradient_sampler_t *self);

/**
 * @brief Store the cosine of the gradient angle, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param cosine The new value.
 */
lh_void
lh_ui_gradient_sampler_set_cosine(lh_ui_gradient_sampler_t *self, lh_s64_t cosine);

/**
 * @brief The length the gradient runs over, fixed point (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_ui_gradient_sampler_get_length(const lh_ui_gradient_sampler_t *self);

/**
 * @brief Store the length the gradient runs over, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param length The new value.
 */
lh_void
lh_ui_gradient_sampler_set_length(lh_ui_gradient_sampler_t *self, lh_s64_t length);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GRADIENT_SAMPLER_H */
