/**
 * @file fields.h
 * @brief Macro for declaring fields of typed memory range wrappers.
 */

#ifndef LH_MEMORY_TYPED_FIELDS_H
#define LH_MEMORY_TYPED_FIELDS_H

#include <lh/size.h>

/**
 * @def lh_memory_typed_fields(T)
 * @brief Declares `range` and `type_size` members for typed-memory structs.
 *
 * Embeds the given type @p T as the @c range field and adds a @c type_size field.
 * The resulting struct inherits all fields from @p T (if any), plus these two new fields.
 *
 * @param T Type of the @c range field — must be a struct type with two pointer fields
 *        named @c first and @c second (for example ::lh_memory_range_t,
 *        ::lh_memory_view_t). These fields are accessed via the wrapper's
 *        @c range member.
 *
 * Example usage — creates a typed wrapper around lh_memory_range_t:
 * @code{.c}
 * typedef struct lh_memory_typed {
 *     lh_memory_typed_fields(lh_memory_range_t);
 * } lh_memory_typed_t;
 * @endcode
 *
 * The resulting struct has:
 * @code{.c}
 * lh_memory_range_t range;   // inherited from T
 * lh_usize_t type_size;      // size of one element in bytes
 * @endcode
 *
 * Example usage — creates a typed wrapper around a custom range type:
 * @code{.c}
 * typedef struct my_range {
 *     void *begin;
 *     void *end;
 * } my_range_t;
 *
 * typedef struct my_typed {
 *     lh_memory_typed_fields(my_range_t);
 * } my_typed_t;
 * @endcode
 */
#define lh_memory_typed_fields(T)                                                                  \
    T range;                                                                                       \
    lh_usize_t type_size

#endif // LH_MEMORY_TYPED_FIELDS_H
