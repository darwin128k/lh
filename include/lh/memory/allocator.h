/**
 * @file allocator.h
 * @brief Configurable memory allocator wrapper with alloc/dealloc callbacks.
 *
 * ::lh_memory_allocator_t stores two function pointers:
 * one for allocation and one for deallocation.
 *
 * The API provides init/set/get helpers plus allocation/deallocation
 * calls that validate callback availability via runtime checks.
 */

#ifndef LH_MEMORY_ALLOCATOR_H
#define LH_MEMORY_ALLOCATOR_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/memory/allocator/alloc/cb.h>
#include <lh/memory/allocator/dealloc/cb.h>
#include <lh/memory/allocator/fields.h>

/**
 * @struct lh_memory_allocator
 * @brief Pair of allocation/deallocation callbacks used by memory helpers.
 *
 * Stores function pointers for allocate/free operations:
 * - `alloc_cb` allocates a block of requested size
 * - `dealloc_cb` releases a previously allocated block
 *
 * The concrete fields are expanded by ::lh_memory_allocator_fields.
 */
typedef struct lh_memory_allocator {
    lh_memory_allocator_fields(lh_memory_allocator_alloc_fn, lh_memory_allocator_dealloc_fn);
} lh_memory_allocator_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Pack optional callback pointers into allocator fields.
 *
 * Only fields with non-null input pointers are updated.
 *
 * @param self Allocator object to modify.
 * @param alloc_cb Optional pointer to allocation callback value (or ::lh_null to skip).
 * @param dealloc_cb Optional pointer to deallocation callback value (or ::lh_null to skip).
 */
LH_ATTRIBUTE_SYMBOL
lh_void lh_memory_allocator_pack(lh_memory_allocator_t *self,
                                 lh_memory_allocator_alloc_cb *alloc_cb,
                                 lh_memory_allocator_dealloc_cb *dealloc_cb);

/**
 * @brief Unpack allocator callback fields into optional output pointers.
 *
 * @param self Allocator object to read from.
 * @param alloc_cb Optional output for allocation callback (or ::lh_null to skip).
 * @param dealloc_cb Optional output for deallocation callback (or ::lh_null to skip).
 */
LH_ATTRIBUTE_SYMBOL
lh_void lh_memory_allocator_unpack(const lh_memory_allocator_t *self,
                                   lh_memory_allocator_alloc_cb *alloc_cb,
                                   lh_memory_allocator_dealloc_cb *dealloc_cb);

/**
 * @brief Set both callbacks at once.
 *
 * @param self Allocator object to modify.
 * @param alloc_cb New allocation callback.
 * @param dealloc_cb New deallocation callback.
 */
LH_ATTRIBUTE_SYMBOL
lh_void lh_memory_allocator_set(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb,
                                lh_memory_allocator_dealloc_cb dealloc_cb);

/**
 * @brief Initialize allocator callbacks.
 *
 * Call this (or ::lh_memory_allocator_set) before first use of
 * ::lh_memory_allocator_alloc / ::lh_memory_allocator_dealloc.
 *
 * @param self Allocator object to initialize.
 * @param alloc_cb Allocation callback (may be ::lh_null to leave allocation disabled).
 * @param dealloc_cb Deallocation callback (may be ::lh_null to leave deallocation disabled).
 */
LH_ATTRIBUTE_SYMBOL
lh_void lh_memory_allocator_init(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb,
                                 lh_memory_allocator_dealloc_cb dealloc_cb);

/**
 * @brief Reset allocator to empty state (both callbacks set to ::lh_null).
 * @param self Allocator object to reset.
 */
LH_ATTRIBUTE_SYMBOL
lh_void lh_memory_allocator_deinit(lh_memory_allocator_t *self);

/**
 * @brief Get current allocation callback from @p self.
 * @param self Allocator object to read from.
 * @return Current allocation callback (may be ::lh_null).
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_allocator_alloc_cb lh_memory_allocator_get_alloc_cb(lh_memory_allocator_t *self);

/**
 * @brief Get current deallocation callback from @p self.
 * @param self Allocator object to read from.
 * @return Current deallocation callback (may be ::lh_null).
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_allocator_dealloc_cb lh_memory_allocator_get_dealloc_cb(lh_memory_allocator_t *self);

/**
 * @brief Allocate a memory block using allocator callback.
 *
 * Fails with runtime checks when callback
 * is not initialized or allocation fails.
 *
 * @param self Allocator object to use.
 * @param size Requested size in bytes.
 * @return Pointer to allocated memory.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr lh_memory_allocator_alloc(lh_memory_allocator_t *self, lh_usize_t size);

/**
 * @brief Deallocate a memory block using deallocator callback.
 *
 * If @p ptr is null, function returns immediately.
 *
 * @param self Allocator object to use.
 * @param ptr Pointer to memory block to free.
 */
LH_ATTRIBUTE_SYMBOL
lh_void lh_memory_allocator_dealloc(lh_memory_allocator_t *self, lh_ptr ptr);

/**
 * @brief Reallocate memory block using allocator callbacks.
 *
 * If sizes are equal, returns @p old_ptr unchanged.
 * If @p old_ptr is null, behaves like ::lh_memory_allocator_alloc.
 * If @p new_size is zero, frees @p old_ptr and returns ::lh_null.
 *
 * @param self Allocator object to use.
 * @param old_ptr Previously allocated block (or ::lh_null).
 * @param old_size Previous block size in bytes.
 * @param new_size Requested block size in bytes.
 * @return Pointer to resized block, @p old_ptr, or ::lh_null depending on inputs.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr lh_memory_allocator_realloc(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                                   lh_usize_t new_size);

LH_COMPILER_EXTERN_C_END

#endif // LH_MEMORY_ALLOCATOR_H