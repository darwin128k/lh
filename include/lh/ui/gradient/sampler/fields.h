/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_gradient_sampler_t.
 */

#ifndef LH_UI_GRADIENT_SAMPLER_FIELDS_H
#define LH_UI_GRADIENT_SAMPLER_FIELDS_H

/**
 * @def lh_ui_gradient_sampler_fields(gradient_type, color_type, shape_type)
 * @brief A gradient over one box: the gradient, the colour of every step of the stop
 *        axis, the box's sizes and doubled middle, and a linear gradient's direction
 *        and the length of the line it runs along.
 *
 * @param gradient_type ::lh_ui_gradient_t.
 * @param color_type    ::lh_ui_color_t.
 * @param shape_type    ::lh_ui_gradient_shape_t.
 */
#define lh_ui_gradient_sampler_fields(gradient_type, color_type, shape_type)                        \
    const gradient_type *gradient;                                                                  \
    color_type colors[256];                                                                         \
    shape_type shape;                                                                               \
    lh_s64_t width;                                                                                 \
    lh_s64_t height;                                                                                \
    lh_s64_t middle_x;                                                                              \
    lh_s64_t middle_y;                                                                              \
    lh_s64_t sine;                                                                                  \
    lh_s64_t cosine;                                                                                \
    lh_s64_t length

#endif /* LH_UI_GRADIENT_SAMPLER_FIELDS_H */
