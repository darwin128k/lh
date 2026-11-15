/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_place_t.
 */

#ifndef LH_GL_LAYOUT_PLACE_INITIALIZER_H
#define LH_GL_LAYOUT_PLACE_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_place_initializer(size_mode, size, align, margin, min, max)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_place_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_place_fields.
 *
 * @param size_mode How the length along the flow is taken (::lh_gl_place_size_t).
 * @param size      Length along the flow, or a fill's weight.
 * @param align     Where on the other axis (::lh_gl_place_align_t).
 * @param margin    Initializer of the ::lh_gl_insets_t kept clear around it.
 * @param min       Initializer of the least ::lh_gl_size_t it may be.
 * @param max       Initializer of the most ::lh_gl_size_t it may be.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_place_t place = lh_gl_place_initializer(
 *     lh_gl_place_size_fixed,                // size_mode
 *     120,                                   // size
 *     lh_gl_place_align_center,              // align
 *     lh_gl_insets_initializer(4, 4, 4, 4),  // margin
 *     lh_initializer(0, 0),                  // min
 *     lh_initializer(0, 0));                 // max
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_place_t
 */
#define lh_gl_place_initializer(size_mode, size, align, margin, min, max)                          \
    lh_initializer(size_mode, size, align, margin, min, max)

#endif /* LH_GL_LAYOUT_PLACE_INITIALIZER_H */
