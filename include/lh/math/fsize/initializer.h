/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_fsize_t.
 */

#ifndef LH_MATH_FSIZE_INITIALIZER_H
#define LH_MATH_FSIZE_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_fsize_initializer(width, height)
 * @brief Produces a brace-enclosed initializer for ::lh_math_fsize_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_size_fields.
 *
 * @param width  Value for @c width.
 * @param height Value for @c height.
 *
 * Example usage:
 * @code{.c}
 * static const lh_math_fsize_t size = lh_math_fsize_initializer(0.5f, 2.0f);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_fsize_t
 */
#define lh_math_fsize_initializer(width, height) lh_initializer(width, height)

#endif /* LH_MATH_FSIZE_INITIALIZER_H */
