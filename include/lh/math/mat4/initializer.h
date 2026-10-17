/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_mat4_t.
 */

#ifndef LH_MATH_MAT4_INITIALIZER_H
#define LH_MATH_MAT4_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_mat4_initializer(column0, column1, column2, column3)
 * @brief Produces a brace-enclosed initializer for ::lh_math_mat4_t.
 *
 * Expands to two nested ::lh_initializer lists: the outer one is the struct,
 * the inner one its @c columns array (::lh_math_mat4_fields). The matrix is
 * column-major, so each argument is one column, not one row.
 *
 * @param column0 Initializer of column 0 (::lh_math_vec4_initializer).
 * @param column1 Initializer of column 1 (::lh_math_vec4_initializer).
 * @param column2 Initializer of column 2 (::lh_math_vec4_initializer).
 * @param column3 Initializer of column 3 (::lh_math_vec4_initializer).
 *
 * Example usage:
 * @code{.c}
 * static const lh_math_mat4_t identity = lh_math_mat4_initializer(
 *     lh_math_vec4_initializer(1.0f, 0.0f, 0.0f, 0.0f),   // column 0
 *     lh_math_vec4_initializer(0.0f, 1.0f, 0.0f, 0.0f),   // column 1
 *     lh_math_vec4_initializer(0.0f, 0.0f, 1.0f, 0.0f),   // column 2
 *     lh_math_vec4_initializer(0.0f, 0.0f, 0.0f, 1.0f));  // column 3
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_mat4_t
 */
#define lh_math_mat4_initializer(column0, column1, column2, column3)                               \
    lh_initializer(lh_initializer(column0, column1, column2, column3))

#endif /* LH_MATH_MAT4_INITIALIZER_H */
