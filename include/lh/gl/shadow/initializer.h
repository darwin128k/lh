/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_shadow_t.
 */

#ifndef LH_GL_SHADOW_INITIALIZER_H
#define LH_GL_SHADOW_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_shadow_initializer(color, spread, offset_x, offset_y)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_shadow_t.
 *
 * Expands to ::lh_initializer with the arguments in struct order.
 *
 * @param color    Initializer of the ::lh_gl_color_t (::lh_gl_color_initializer).
 * @param spread   Distance the alpha fades over.
 * @param offset_x Horizontal shift of the shadow.
 * @param offset_y Vertical shift of the shadow.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_shadow_t shadow = lh_gl_shadow_initializer(
 *     lh_gl_color_initializer(0x00, 0x00, 0x00, 0x40),  // color
 *     8,                                                // spread
 *     0,                                                // offset_x
 *     2);                                               // offset_y
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_shadow_t
 */
#define lh_gl_shadow_initializer(color, spread, offset_x, offset_y)                                \
    lh_initializer(color, spread, offset_x, offset_y)

#endif /* LH_GL_SHADOW_INITIALIZER_H */
