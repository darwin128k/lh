/**
 * @file allocated.h
 * @brief Helpers for ::lh_memory_typed_allocated_t — heap-owning typed memory.
 *
 * ::lh_memory_typed_allocated_t is layout-identical to ::lh_memory_typed_t, but
 * every function in this header treats the embedded @c bounds field as a block
 * previously obtained from ::lh_runtime_allocator_alloc / ::lh_runtime_allocator_realloc
 * (as ::lh_memory_bounds_allocated_t) and manages it accordingly (freeing it on
 * clear/resize/exchange).
 *
 * @see lh_memory_typed_t
 * @see lh_memory_bounds_allocated_t
 */

#ifndef LH_MEMORY_TYPED_ALLOCATED_H
#define LH_MEMORY_TYPED_ALLOCATED_H

#include <lh/memory/typed.h>
#include <lh/memory/bounds/allocated.h>

/**
 * @brief Typed view owning a heap-allocated block.
 *
 * ::lh_memory_typed_allocated_t is a typedef of ::lh_memory_typed_t used to mark
 * that its embedded @c bounds field (when set) was obtained from the runtime
 * allocator and must be released through the functions declared in this header
 * rather than through the plain ::lh_memory_typed_t API.
 */
typedef lh_memory_typed_t lh_memory_typed_allocated_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Free the block owned by @p self and reset it to an empty typed value.
 *
 * Element type size is preserved; only the owned bounds are cleared.
 *
 * @param self Allocated typed value to clear.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_allocated_clear(lh_memory_typed_allocated_t *self);

/**
 * @brief Resize the block owned by @p self to hold @p count elements.
 *
 * Reallocates the owned block through the runtime allocator to
 * <tt>count * type_size</tt> bytes and stores the resulting bounds in @p self.
 * Passing @c 0 for @p count frees the block and leaves @p self empty.
 *
 * @param self  Allocated typed value to resize.
 * @param count Requested element count.
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
lh_memory_typed_allocated_resize(lh_memory_typed_allocated_t *self, lh_usize_t count);

/**
 * @brief Free the block owned by @p self, then take ownership of @p other's block.
 *
 * After the call, @p self owns the block and element type size previously
 * owned by @p other, and @p other is left empty (its own type size is kept).
 * When @p self and @p other are the same object, the owned block is simply
 * freed.
 *
 * @param self  Allocated typed value to clear and replace.
 * @param other Allocated typed value to take ownership from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_allocated_exchange(lh_memory_typed_allocated_t *self,
                                   lh_memory_typed_allocated_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_TYPED_ALLOCATED_H */
