/**
 * @file fields.h
 * @brief Macro for declaring `alloc_cb` / `dealloc_cb` fields of an allocator struct.
 */

#ifndef LH_MEMORY_ALLOCATOR_FIELDS_H
#define LH_MEMORY_ALLOCATOR_FIELDS_H

/**
 * @def lh_memory_allocator_fields(allocator_type, deallocator_type)
 * @brief Expands to two callback pointer members for allocation and deallocation.
 *
 * Intended to be embedded inside a struct body so ::lh_memory_allocator_t
 * (and similar wrappers) share one declaration shape.
 *
 * @param allocator_type   Function type for allocation (e.g. ::lh_memory_allocator_alloc_fn);
 *                         the member is `allocator_type *alloc_cb`.
 * @param deallocator_type Function type for deallocation (e.g. ::lh_memory_allocator_dealloc_fn);
 *                         the member is `deallocator_type *dealloc_cb`.
 *
 * Example usage:
 * @code{.c}
 * typedef struct lh_memory_allocator {
 *     lh_memory_allocator_fields(lh_memory_allocator_alloc_fn, lh_memory_allocator_dealloc_fn);
 * } lh_memory_allocator_t;
 * @endcode
 *
 * @see lh_memory_allocator_t
 */
#define lh_memory_allocator_fields(allocator_type, deallocator_type)                               \
    allocator_type *alloc_cb;                                                                      \
    deallocator_type *dealloc_cb

#endif // LH_MEMORY_ALLOCATOR_FIELDS_H
