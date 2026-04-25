/**
 * @file offset.h
 * @brief Member-offset utility macro.
 *
 * Provides a thin wrapper around the standard `offsetof` operator.
 */
#ifndef LH_UTIL_OFFSET_H
#define LH_UTIL_OFFSET_H

#include <stddef.h>

/**
 * @def lh_offset_of(T, member)
 * @brief Byte offset of @p member within type @p T.
 *
 * Expands to `offsetof(T, member)` and yields a `size_t` constant expression.
 * Useful for compile-time layout checks and low-level container patterns.
 *
 * @param T      Any complete struct or union type.
 * @param member Member name inside @p T.
 *
 * @code{.c}
 * lh_offset_of(lh_memory_typed_t, range);      // usually 0
 * lh_offset_of(lh_memory_typed_t, type_size);  // offset after range
 * @endcode
 */
#define lh_offset_of(T, member) offsetof(T, member)

#endif // LH_UTIL_OFFSET_H