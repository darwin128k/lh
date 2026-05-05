/**
 * @file allocated.h
 * @brief Allocated memory range with ownership semantics.
 *
 * Provides a type and functions for managing a memory range that
 * owns its allocated storage. Includes operations for getting size,
 * clearing (freeing) the allocation, exchanging ranges, and resizing.
 *
 * @see lh_memory_range_t
 */

#ifndef LH_MEMORY_RANGE_ALLOCATED_H
#define LH_MEMORY_RANGE_ALLOCATED_H

#include <lh/attribute/symbol.h>
#include <lh/memory/range.h>

/**
 * @typedef lh_memory_range_allocated_t
 * @brief Alias for ::lh_memory_range_t with allocated-storage semantics.
 *
 * @see lh_memory_range_t
 * @see lh_memory_range_allocated_get_size
 * @see lh_memory_range_allocated_clear
 */
typedef lh_memory_range_t lh_memory_range_allocated_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Byte size of the stored span, or @c 0 if the range is uninitialized.
 *
 * @param self Range to inspect (uninitialized is defined as
 *             ::lh_memory_range_is_uninitialized).
 * @return Span length in bytes, or @c 0 when uninitialized; otherwise
 *         same as ::lh_memory_range_get_size.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t lh_memory_range_allocated_get_size(const lh_memory_range_allocated_t *self);

/**
 * @brief Free the owned block and clear stored endpoints.
 *
 * Uses ::lh_runtime_allocator_free for @c first when non-null, then
 * ::lh_memory_range_clear (see ::lh_memory_range_allocated_clear implementation).
 *
 * @param self Range whose storage to release; may be uninitialized (no-op free).
 */
LH_ATTRIBUTE_SYMBOL
lh_void lh_memory_range_allocated_clear(lh_memory_range_allocated_t *self);

/**
 * @brief Release @p self’s storage, then swap bounds with @p other.
 *
 * Frees the block described by @p self (as ::lh_memory_range_allocated_clear), then
 * ::lh_memory_range_swap with @p other.
 *
 * @param self  Range to clear and then exchange.
 * @param other Second range; receives cleared bounds from @p self after the swap
 *              (per ::lh_memory_range_swap).
 */
LH_ATTRIBUTE_SYMBOL
lh_void lh_memory_range_allocated_exchange(lh_memory_range_allocated_t *self,
                                           lh_memory_range_allocated_t *other);

/**
 * @brief Resize the owned block in place using the runtime allocator.
 *
 * Reallocates from the current @c first pointer and length from
 * ::lh_memory_range_allocated_get_size to @p size bytes, then updates bounds with
 * ::lh_memory_range_set_by_size_or_clear (clears the range on failure).
 *
 * @param self Range holding the block to resize.
 * @param size New size in bytes.
 */
LH_ATTRIBUTE_SYMBOL
lh_void lh_memory_range_allocated_resize(lh_memory_range_allocated_t *self, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif // LH_MEMORY_RANGE_ALLOCATED_H
