/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_typed_t.
 */

#ifndef LH_MEMORY_TYPED_INITIALIZER_H
#define LH_MEMORY_TYPED_INITIALIZER_H

#include <lh/memory/bounds/initializer.h>

/**
 * @def lh_memory_typed_initializer(begin, size, type_size)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_typed_t.
 *
 * Expands to ::lh_initializer with @p begin and @p size as the @c bounds field,
 * @p type_size as the @c type_size field, and <tt>size / type_size</tt> as the
 * cached @c size field — computed here, once, rather than left for the first read to derive.
 *
 * @param begin     Start of the half-open span.
 * @param size      Length of the span in bytes, not in elements.
 * @param type_size Size of one element in bytes.
 *
 * Example:
 * @code
 * static lh_memory_typed_t typed = lh_memory_typed_initializer(ptr, n * sizeof(int), sizeof(int));
 * @endcode
 *
 * @see lh_initializer
 */
#define lh_memory_typed_initializer(begin, size, type_size)                                        \
    lh_initializer(lh_memory_bounds_initializer(begin, size), type_size,                           \
                   lh_math_div(size, type_size))

/**
 * @def lh_memory_typed_empty_initializer(type_size)
 * @brief Produces a brace-enclosed "empty" initializer for ::lh_memory_typed_t.
 *
 * Expands to ::lh_memory_typed_initializer(::lh_null, 0, @p type_size).
 *
 * @param type_size Size of one element in bytes.
 *
 * @see lh_memory_typed_initializer
 */
#define lh_memory_typed_empty_initializer(type_size)                                               \
    lh_memory_typed_initializer(lh_null, 0U, type_size)

#endif /* LH_MEMORY_TYPED_INITIALIZER_H */
