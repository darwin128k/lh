/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_pixel_gradient_sampler_t.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_SAMPLER_FIELDS_H
#define LH_CORE_PIXEL_GRADIENT_SAMPLER_FIELDS_H

/**
 * @def lh_pixel_gradient_sampler_fields(shape_type, length_type)
 * @brief A gradient laid over one box, as whole numbers a row is walked with.
 *
 * `shape` is the gradient's; `width` and `height` the box's; `middle_x` and
 * `middle_y` the box's middle, doubled (so a box of odd size loses no half);
 * `sine` and `cosine` the gradient's direction times 4096; `length` the box
 * seen along that direction, doubled, times 4096 — the scale a linear position
 * is divided by.
 *
 * @param shape_type  Type of the shape (::lh_pixel_gradient_shape_t).
 * @param length_type Type of the lengths (::lh_s64_t).
 */
#define lh_pixel_gradient_sampler_fields(shape_type, length_type)                                  \
    shape_type shape;                                                                              \
    length_type width;                                                                             \
    length_type height;                                                                            \
    length_type middle_x;                                                                          \
    length_type middle_y;                                                                          \
    length_type sine;                                                                              \
    length_type cosine;                                                                            \
    length_type length

#endif /* LH_CORE_PIXEL_GRADIENT_SAMPLER_FIELDS_H */
