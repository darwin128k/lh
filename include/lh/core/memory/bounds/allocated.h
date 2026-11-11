/**
 * @file allocated.h
 * @brief Helpers for ::lh_memory_bounds_allocated_t — heap-owning memory bounds.
 *
 * ::lh_memory_bounds_allocated_t is layout-identical to ::lh_memory_bounds_t, but
 * every function in this header treats the stored @c begin as a block
 * previously obtained from ::lh_runtime_allocator_alloc / ::lh_runtime_allocator_realloc,
 * and manages it accordingly (freeing it on clear/resize/exchange). The `_by`
 * forms do the same through an allocator the caller names
 * (::lh_memory_allocator_t); the plain forms are the `_by` forms over the runtime
 * allocator. A block is always released through the allocator that made it.
 *
 * @see lh_memory_bounds_t
 * @see lh_runtime_allocator_alloc
 */

#ifndef LH_MEMORY_BOUNDS_ALLOCATED_H
#define LH_MEMORY_BOUNDS_ALLOCATED_H

#include <lh/core/memory/allocator.h>
#include <lh/core/memory/bounds.h>

/**
 * @brief Half-open memory bounds owning a heap-allocated block.
 *
 * ::lh_memory_bounds_allocated_t is a typedef of ::lh_memory_bounds_t used to mark
 * that @c begin (when set) was obtained from the runtime allocator
 * and must be released through the functions declared in this header rather
 * than through the plain ::lh_memory_bounds_t API.
 */
typedef lh_memory_bounds_t lh_memory_bounds_allocated_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Free the owned block and reset @p self to empty bounds.
 *
 * @param self Allocated bounds to clear.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
lh_void
lh_memory_bounds_allocated_clear(lh_memory_bounds_allocated_t *self);

/**
 * @brief Free the owned block through @p allocator and reset @p self to empty bounds.
 *
 * @param self      Allocated bounds to clear.
 * @param allocator The allocator that made the block.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p allocator is ::lh_null.
 * @fails ::lh_runtime_error_code_deallocator_function_not_initialized
 *        @p allocator has no deallocation callback.
 */
lh_void
lh_memory_bounds_allocated_clear_by(lh_memory_bounds_allocated_t *self,
                                    lh_memory_allocator_t *allocator);

/**
 * @brief Resize through the runtime allocator without terminating on allocation failure.
 * Preserves the original bounds and bytes on failure. Zero releases and clears
 * the block. Requires the same allocator ownership contract as the checked form.
 * @return True on success, false on allocation failure.
 */
lh_bool_t
lh_memory_bounds_allocated_try_resize(lh_memory_bounds_allocated_t *self, lh_usize_t size);

/**
 * @brief Resize through @p allocator without terminating on allocation failure.
 *
 * Preserves the original bounds and bytes on failure. Zero releases and clears
 * the block. The block must have come from @p allocator (or be empty).
 *
 * @param self      Allocated bounds to resize.
 * @param allocator The allocator that made the block, and makes the new one.
 * @param size      Requested block size in bytes.
 * @return ::lh_bool_true on success, ::lh_bool_false on allocation failure.
 */
lh_bool_t
lh_memory_bounds_allocated_try_resize_by(lh_memory_bounds_allocated_t *self,
                                         lh_memory_allocator_t *allocator, lh_usize_t size);

/**
 * @brief Resize the block owned by @p self to @p size bytes.
 *
 * Reallocates the owned block through the runtime allocator and stores the
 * resulting half-open bounds in @p self. Passing @c 0 for @p size frees the
 * block and leaves @p self empty, matching
 * ::lh_runtime_allocator_realloc semantics.
 *
 * @param self Allocated bounds to resize.
 * @param size Requested block size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_memory_not_allocated
 *        Growing or newly allocating the block failed.
 * @fails ::lh_runtime_error_code_allocator_function_not_initialized
 *        The runtime allocation callback is not initialized.
 * @fails ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
lh_void
lh_memory_bounds_allocated_resize(lh_memory_bounds_allocated_t *self, lh_usize_t size);

/**
 * @brief Free the block owned by @p self, then take ownership of @p other's block.
 *
 * After the call, @p self owns the block previously owned by @p other, and
 * @p other is left empty. When @p self and @p other are the same object, the
 * owned block is simply freed.
 *
 * @param self  Allocated bounds to clear and replace.
 * @param other Valid allocated bounds to take ownership from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 * @fails ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
lh_void
lh_memory_bounds_allocated_exchange(lh_memory_bounds_allocated_t *self,
                                    lh_memory_bounds_allocated_t *other);

/**
 * @brief Free the block owned by @p self through @p allocator, then take
 *        ownership of @p other's block.
 *
 * Both blocks must come from @p allocator: @p self keeps releasing through it.
 *
 * @param self      Allocated bounds to clear and replace.
 * @param allocator The allocator both blocks came from.
 * @param other     Valid allocated bounds to take ownership from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self, @p allocator or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_bounds_allocated_exchange_by(lh_memory_bounds_allocated_t *self,
                                       lh_memory_allocator_t *allocator,
                                       lh_memory_bounds_allocated_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_BOUNDS_ALLOCATED_H */
