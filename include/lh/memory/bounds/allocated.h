/**
 * @file allocated.h
 * @brief Helpers for ::lh_memory_bounds_allocated_t — heap-owning memory bounds.
 *
 * ::lh_memory_bounds_allocated_t is layout-identical to ::lh_memory_bounds_t, but
 * every function in this header treats the stored @c first endpoint as a block
 * previously obtained from ::lh_runtime_allocator_alloc / ::lh_runtime_allocator_realloc,
 * and manages it accordingly (freeing it on clear/resize/exchange).
 *
 * @see lh_memory_bounds_t
 * @see lh_runtime_allocator_alloc
 */

#ifndef LH_MEMORY_BOUNDS_ALLOCATED_H
#define LH_MEMORY_BOUNDS_ALLOCATED_H

#include <lh/memory/bounds.h>

/**
 * @brief Half-open memory bounds owning a heap-allocated block.
 *
 * ::lh_memory_bounds_allocated_t is a typedef of ::lh_memory_bounds_t used to mark
 * that the @c first endpoint (when set) was obtained from the runtime allocator
 * and must be released through the functions declared in this header rather
 * than through the plain ::lh_memory_bounds_t API.
 */
typedef lh_memory_bounds_t lh_memory_bounds_allocated_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return the size in bytes of the owned block, or @c 0 when uninitialized.
 *
 * Unlike ::lh_memory_bounds_get_size, this does not require @p self to be
 * valid — an uninitialized (empty) allocated bounds simply reports size @c 0.
 *
 * @param self Allocated bounds to inspect.
 * @return Size in bytes of the owned block, or @c 0.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_bounds_allocated_get_size(const lh_memory_bounds_allocated_t *self);

/**
 * @brief Free the owned block and reset @p self to empty bounds.
 *
 * @param self Allocated bounds to clear.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_allocated_clear(lh_memory_bounds_allocated_t *self);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_memory_not_allocated
 *        Growing or newly allocating the block failed.
 * @throw ::lh_runtime_error_code_allocator_function_not_initialized
 *        The runtime allocation callback is not initialized.
 * @throw ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
LH_ATTRIBUTE_SYMBOL
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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 * @throw ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_allocated_exchange(lh_memory_bounds_allocated_t *self,
                                    lh_memory_bounds_allocated_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_BOUNDS_ALLOCATED_H */
