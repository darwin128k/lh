/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_typed_t.
 */

#ifndef LH_MEMORY_TYPED_INITIALIZER_H
#define LH_MEMORY_TYPED_INITIALIZER_H

#include <lh/memory/bounds/initializer.h>

/**
 * @def lh_memory_typed_initializer(begin, end, type_size)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_typed_t.
 *
 * Expands to ::lh_initializer with @p begin and @p end as the @c bounds field,
 * and @p type_size as the @c type_size field.
 *
 * @param begin     Start of the half-open span.
 * @param end       One past the last byte.
 * @param type_size Size of one element in bytes.
 *
 * Example:
 * @code
 * static lh_memory_typed_t typed = lh_memory_typed_initializer(ptr, ptr + n, sizeof(int));
 * @endcode
 *
 * @see lh_initializer
 */
#define lh_memory_typed_initializer(begin, end, type_size)                                         \
    lh_initializer(lh_memory_bounds_initializer(begin, end), type_size)

/**
 * @def lh_memory_typed_empty_initializer(type_size)
 * @brief Produces a brace-enclosed "empty" initializer for ::lh_memory_typed_t.
 *
 * Expands to ::lh_memory_typed_initializer(::lh_null, ::lh_null, @p type_size).
 *
 * @param type_size Size of one element in bytes.
 *
 * @see lh_memory_typed_initializer
 */
#define lh_memory_typed_empty_initializer(type_size)                                               \
    lh_memory_typed_initializer(lh_null, lh_null, type_size)

#endif // LH_MEMORY_TYPED_INITIALIZER_H