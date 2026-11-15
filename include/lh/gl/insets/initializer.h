/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_insets_t.
 */

#ifndef LH_GL_INSETS_INITIALIZER_H
#define LH_GL_INSETS_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_insets_initializer(left, top, right, bottom)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_insets_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_insets_fields.
 *
 * @param left   Value for @c left.
 * @param top    Value for @c top.
 * @param right  Value for @c right.
 * @param bottom Value for @c bottom.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_insets_t padding = lh_gl_insets_initializer(8, 4, 8, 4);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_insets_t
 */
#define lh_gl_insets_initializer(left, top, right, bottom) lh_initializer(left, top, right, bottom)

#endif /* LH_GL_INSETS_INITIALIZER_H */
