/**
 * @file fields.h
 * @brief Member fields of ::lh_memory_sized_allocator_t.
 */

#ifndef LH_MEMORY_SIZED_ALLOCATOR_FIELDS_H
#define LH_MEMORY_SIZED_ALLOCATOR_FIELDS_H

/**
 * @def lh_memory_sized_allocator_fields(inner_type)
 * @brief The allocator every block is obtained from and returned to.
 *
 * @param inner_type Type of `inner` (`lh_memory_allocator_t *`).
 */
#define lh_memory_sized_allocator_fields(inner_type) inner_type inner

#endif /* LH_MEMORY_SIZED_ALLOCATOR_FIELDS_H */
