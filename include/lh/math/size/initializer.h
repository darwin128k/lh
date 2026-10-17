/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_size_t.
 */

#ifndef LH_MATH_SIZE_INITIALIZER_H
#define LH_MATH_SIZE_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_size_initializer(width, height)
 * @brief Produces a brace-enclosed initializer for ::lh_math_size_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_size_fields.
 *
 * @param width  Value for @c width.
 * @param height Value for @c height.
 *
 * Example usage:
 * @code{.c}
 * static const lh_math_size_t size = lh_math_size_initializer(640, 480);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_size_t
 */
#define lh_math_size_initializer(width, height) lh_initializer(width, height)

#endif /* LH_MATH_SIZE_INITIALIZER_H */
