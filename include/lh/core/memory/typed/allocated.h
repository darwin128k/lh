/**
 * @file allocated.h
 * @brief Helpers for ::lh_memory_typed_allocated_t — heap-owning typed memory.
 *
 * ::lh_memory_typed_allocated_t is layout-identical to ::lh_memory_typed_t, but
 * every function in this header treats its bounds (::lh_memory_typed_get_bounds_mut) as a block
 * previously obtained from ::lh_runtime_allocator_alloc / ::lh_runtime_allocator_realloc
 * (as ::lh_memory_bounds_allocated_t) and manages it accordingly (freeing it on
 * clear/resize/exchange). The `_by` forms work through an allocator the caller
 * names; the plain forms are the `_by` forms over the runtime allocator.
 *
 * @see lh_memory_typed_t
 * @see lh_memory_bounds_allocated_t
 */

#ifndef LH_MEMORY_TYPED_ALLOCATED_H
#define LH_MEMORY_TYPED_ALLOCATED_H

#include <lh/core/memory/typed.h>
#include <lh/core/memory/bounds/allocated.h>

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
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
lh_void
lh_memory_typed_allocated_clear(lh_memory_typed_allocated_t *self);

/**
 * @brief Free the block owned by @p self through @p allocator and reset it to
 *        an empty typed value. Element type size is preserved.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p allocator is ::lh_null.
 */
lh_void
lh_memory_typed_allocated_clear_by(lh_memory_typed_allocated_t *self,
                                   lh_memory_allocator_t *allocator);

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
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_deallocator_function_not_initialized
 *        The runtime deallocation callback is not initialized.
 */
lh_void
lh_memory_typed_allocated_exchange(lh_memory_typed_allocated_t *self,
                                   lh_memory_typed_allocated_t *other);

/**
 * @brief ::lh_memory_typed_allocated_exchange for two blocks that both came
 *        from @p allocator; the old block of @p self is freed through it.
 */
lh_void
lh_memory_typed_allocated_exchange_by(lh_memory_typed_allocated_t *self,
                                      lh_memory_allocator_t *allocator,
                                      lh_memory_typed_allocated_t *other);

/**
 * @brief Resize to count elements, preserving ownership and contents on failure.
 * Returns false for byte-size overflow or runtime allocation failure. A zero count
 * releases the block. Requires the allocator contract of the checked form.
 */
lh_bool_t
lh_memory_typed_allocated_try_resize(lh_memory_typed_allocated_t *self, lh_usize_t count);

/**
 * @brief Resize to @p count elements through @p allocator, preserving ownership
 *        and contents on failure.
 *
 * @return ::lh_bool_false for byte-size overflow or allocation failure. A zero
 *         count releases the block.
 */
lh_bool_t
lh_memory_typed_allocated_try_resize_by(lh_memory_typed_allocated_t *self,
                                        lh_memory_allocator_t *allocator, lh_usize_t count);

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
lh_memory_typed_allocated_resize(lh_memory_typed_allocated_t *self, lh_usize_t count);

/**
 * @brief ::lh_memory_typed_allocated_resize through @p allocator.
 *
 * @fails ::lh_runtime_error_code_memory_not_allocated
 *        Growing or newly allocating the block failed.
 */
lh_void
lh_memory_typed_allocated_resize_by(lh_memory_typed_allocated_t *self,
                                    lh_memory_allocator_t *allocator, lh_usize_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_TYPED_ALLOCATED_H */
