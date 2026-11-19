/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gl_gradient_sampler_t.
 */

#ifndef LH_GL_GRADIENT_SAMPLER_FIELDS_H
#define LH_GL_GRADIENT_SAMPLER_FIELDS_H

/**
 * @def lh_gl_gradient_sampler_fields(sampler_type, color_type)
 * @brief The core's sampler over the box, and the gradient's 256 colours as gl's.
 *
 * @param sampler_type Type of the core sampler (::lh_pixel_gradient_sampler_t).
 * @param color_type   Type of a gl colour (::lh_gl_color_t).
 */
#define lh_gl_gradient_sampler_fields(sampler_type, color_type)                                    \
    sampler_type sampler;                                                                          \
    color_type colors[256]

#endif /* LH_GL_GRADIENT_SAMPLER_FIELDS_H */
