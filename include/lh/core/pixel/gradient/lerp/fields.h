/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_pixel_gradient_lerp_t.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_LERP_FIELDS_H
#define LH_CORE_PIXEL_GRADIENT_LERP_FIELDS_H

/**
 * @def lh_pixel_gradient_lerp_fields(value_type)
 * @brief A colour between two stops, carried from step to step.
 *
 * Per channel — blue, green, red, alpha, as the bytes of `0xAARRGGBB` from the
 * low one up — `quotient` is the channel's value, `rest` its remainder
 * `0 .. span - 1`, `step` and `step_rest` what one step adds to each; `span` is
 * the number of steps between the two stops, above zero.
 *
 * @param value_type Type of each number (::lh_s32_t).
 */
#define lh_pixel_gradient_lerp_fields(value_type)                                                  \
    value_type quotient[4];                                                                        \
    value_type rest[4];                                                                            \
    value_type step[4];                                                                            \
    value_type step_rest[4];                                                                       \
    value_type span

#endif /* LH_CORE_PIXEL_GRADIENT_LERP_FIELDS_H */
