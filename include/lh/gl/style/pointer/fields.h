/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_style_pointer_t.
 */

#ifndef LH_GL_STYLE_POINTER_FIELDS_H
#define LH_GL_STYLE_POINTER_FIELDS_H

/**
 * @def lh_gl_style_pointer_fields(radius_type, style_type)
 * @brief How a style answers a pointer: where it is hit and how it looks pressed.
 *
 * `hit_radius` rounds the corners of what a hit test accepts (`0` = the
 * whole rect: a rounded look is pressed by its rect, which is what a finger
 * aims at), and `pressed` is the style in effect while the entity is pressed
 * (not owned, ::lh_null = it looks the same either way). A part of
 * ::lh_gl_style_t that lives inside it (::lh_gl_style_get_pointer).
 *
 * @param radius_type  ::lh_gl_scalar_t.
 * @param style_type   struct lh_gl_style.
 */
#define lh_gl_style_pointer_fields(radius_type, style_type)                                        \
    const style_type *pressed;                                                                     \
    radius_type hit_radius

#endif /* LH_GL_STYLE_POINTER_FIELDS_H */
