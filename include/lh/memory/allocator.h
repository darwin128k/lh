/**
 * @file allocator.h
 * @brief Configurable memory allocator wrapper with alloc/dealloc (and
 *        optional realloc) callbacks.
 *
 * ::lh_memory_allocator_t stores function pointers for allocation and
 * deallocation, plus an optional native reallocation that lets a block grow
 * in place. Changing alloc or dealloc drops realloc (it would no longer
 * match the heap); set it again with ::lh_memory_allocator_set_realloc_cb.
 *
 * Every callback also receives the allocator's context
 * (::lh_memory_allocator_set_context): the state of a stateful allocator — a
 * pool over a static buffer on a heap-less target, an arena, a per-module
 * sandbox. Several such allocators can then exist at once.
 *
 * Every operation comes in two forms built on the same bricks. A `try_`
 * function reports failure as its result (::lh_null or ::lh_bool_false) and
 * leaves the decision to the caller. Its counterpart without the prefix is
 * the `try_` call followed by a runtime check: absent callbacks and failed
 * allocations end in ::lh_runtime_check_fail.
 */

#ifndef LH_MEMORY_ALLOCATOR_H
#define LH_MEMORY_ALLOCATOR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/memory/allocator/alloc/cb.h>
#include <lh/memory/allocator/dealloc/cb.h>
#include <lh/memory/allocator/fields.h>
#include <lh/memory/allocator/realloc/cb.h>

/**
 * @struct lh_memory_allocator
 * @brief Pair of allocation/deallocation callbacks used by memory helpers.
 *
 * Stores function pointers for allocate/free operations:
 * - `alloc_cb` allocates a block of requested size
 * - `dealloc_cb` releases a previously allocated block
 * - `realloc_cb` resizes a block, possibly in place (optional, may be null)
 * - `context` is passed as the first argument to all three
 *
 * The concrete fields are expanded by ::lh_memory_allocator_fields.
 */
struct lh_memory_allocator
{
    /** Members: ::lh_memory_allocator_fields. */
    lh_memory_allocator_fields(lh_memory_allocator_alloc_fn, lh_memory_allocator_dealloc_fn,
                               lh_memory_allocator_realloc_fn, lh_ptr);
};
/** @copybrief lh_memory_allocator */
typedef struct lh_memory_allocator lh_memory_allocator_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Set the native reallocation callback (::lh_null for none).
 *
 * @p realloc_cb must belong to the same heap as the current alloc/dealloc
 * callbacks. Setting alloc or dealloc afterwards clears it again.
 */
lh_void
lh_memory_allocator_set_realloc_cb(lh_memory_allocator_t *self,
                                   lh_memory_allocator_realloc_cb realloc_cb);

/**
 * @brief Set the context handed to every callback (::lh_null for none).
 *
 * Callbacks and context are set independently; ::lh_memory_allocator_init and
 * ::lh_memory_allocator_deinit reset it to ::lh_null.
 */
lh_void
lh_memory_allocator_set_context(lh_memory_allocator_t *self, lh_ptr context);

/**
 * @brief Current callback context, or ::lh_null.
 */
lh_ptr
lh_memory_allocator_get_context(const lh_memory_allocator_t *self);

/**
 * @brief Get current allocation callback from @p self.
 * @param self Allocator object to read from.
 * @return Current allocation callback (may be ::lh_null).
 */
lh_memory_allocator_alloc_cb
lh_memory_allocator_get_alloc_cb(const lh_memory_allocator_t *self);

/**
 * @brief Get current deallocation callback from @p self.
 * @param self Allocator object to read from.
 * @return Current deallocation callback (may be ::lh_null).
 */
lh_memory_allocator_dealloc_cb
lh_memory_allocator_get_dealloc_cb(const lh_memory_allocator_t *self);

/**
 * @brief Current native reallocation callback, or ::lh_null.
 */
lh_memory_allocator_realloc_cb
lh_memory_allocator_get_realloc_cb(const lh_memory_allocator_t *self);

/**
 * @brief Write only the allocation callback.
 */
lh_void
lh_memory_allocator_set_alloc_cb(lh_memory_allocator_t *self,
                                 lh_memory_allocator_alloc_cb alloc_cb);

/**
 * @brief Write only the deallocation callback.
 */
lh_void
lh_memory_allocator_set_dealloc_cb(lh_memory_allocator_t *self,
                                   lh_memory_allocator_dealloc_cb dealloc_cb);

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
lh_ptr
lh_memory_allocator_alloc(lh_memory_allocator_t *self, lh_usize_t size);

/**
 * @brief Allocate @p count zeroed items of @p item_size bytes.
 *
 * ::lh_memory_allocator_try_alloc_zero followed by runtime checks: fails when
 * the allocation callback is not initialized, the size overflows or
 * allocation fails.
 *
 * @param self Allocator object to use.
 * @param count Number of items.
 * @param item_size Size of one item in bytes.
 * @return Pointer to the zeroed block.
 */
lh_ptr
lh_memory_allocator_alloc_zero(lh_memory_allocator_t *self, lh_usize_t count,
                               lh_usize_t item_size);

/**
 * @brief Deallocate a memory block using deallocator callback.
 *
 * If @p ptr is null, function returns immediately. Fails with a runtime check
 * when a nonnull @p ptr meets an uninitialized deallocation callback.
 *
 * @param self Allocator object to use.
 * @param ptr Pointer to memory block to free.
 */
lh_void
lh_memory_allocator_dealloc(lh_memory_allocator_t *self, lh_ptr ptr);

/**
 * @brief Set both callbacks at once.
 *
 * @param self Allocator object to modify.
 * @param alloc_cb New allocation callback.
 * @param dealloc_cb New deallocation callback.
 */
lh_void
lh_memory_allocator_set(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb,
                        lh_memory_allocator_dealloc_cb dealloc_cb);

/**
 * @brief Reallocate memory block using allocator callbacks.
 *
 * If sizes are equal, returns @p old_ptr unchanged.
 * If @p old_ptr is null, behaves like ::lh_memory_allocator_alloc.
 * If @p new_size is zero, frees @p old_ptr with ::lh_memory_allocator_dealloc
 * and returns ::lh_null. Otherwise ::lh_memory_allocator_try_realloc followed
 * by runtime checks: fails when a needed allocation callback is not
 * initialized (::lh_memory_allocator_realloc_needs_alloc) or resizing fails.
 *
 * @param self Allocator object to use.
 * @param old_ptr Previously allocated block (or ::lh_null).
 * @param old_size Previous block size in bytes.
 * @param new_size Requested block size in bytes.
 * @return Pointer to resized block, @p old_ptr, or ::lh_null depending on inputs.
 */
lh_ptr
lh_memory_allocator_realloc(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                            lh_usize_t new_size);

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
lh_void
lh_memory_allocator_init(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb,
                         lh_memory_allocator_dealloc_cb dealloc_cb);

/**
 * @brief Reset allocator to empty state (both callbacks set to ::lh_null).
 * @param self Allocator object to reset.
 */
lh_void
lh_memory_allocator_deinit(lh_memory_allocator_t *self);

/**
 * @brief Copy the allocator state from @p other into @p self.
 * @param self  Destination allocator (not null).
 * @param other Source allocator (not null).
 */
lh_void
lh_memory_allocator_assign(lh_memory_allocator_t *self, const lh_memory_allocator_t *other);

/**
 * @brief Apply the zero-initialization policy to bytes [@p old_size, @p new_size).
 *
 * With ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED on, zeroes the
 * bytes a block gained; otherwise does nothing. A null @p block or a
 * @p new_size not above @p old_size is ignored. Allocation and native
 * resizing call it for every block they return.
 *
 * @param block Block of at least @p new_size bytes (or ::lh_null).
 * @param old_size Size of the prefix that is kept as is.
 * @param new_size Current size of @p block in bytes.
 */
lh_void
lh_memory_allocator_init_tail(lh_ptr block, lh_usize_t old_size, lh_usize_t new_size);

/**
 * @brief Allocate without treating allocation failure as a runtime error.
 *
 * @p self must be initialized and have compatible alloc/dealloc callbacks.
 * Returns ::lh_null if the allocation callback is absent or allocation fails.
 * Successful blocks follow the configured zero-initialization policy and
 * are owned by the caller; release them with ::lh_memory_allocator_dealloc
 * on the same allocator. A zero size is passed to the allocator unchanged.
 */
lh_ptr
lh_memory_allocator_try_alloc(lh_memory_allocator_t *self, lh_usize_t size);

/**
 * @brief Allocate @p count zeroed items of @p item_size bytes, or ::lh_null.
 *
 * Composed of ::lh_interval_closed_is_mul_overflow, ::lh_memory_allocator_try_alloc
 * and ::lh_memory_std_set. Returns ::lh_null when count * item_size overflows
 * ::lh_usize_t, the allocation callback is absent or allocation fails. The
 * block is zeroed regardless of the zero-initialization policy. A zero
 * product is passed to the allocator unchanged.
 */
lh_ptr
lh_memory_allocator_try_alloc_zero(lh_memory_allocator_t *self, lh_usize_t count,
                                   lh_usize_t item_size);

/**
 * @brief Release @p ptr without treating a missing callback as a runtime error.
 *
 * Null @p ptr is ignored and reports success.
 *
 * @return ::lh_bool_true when nothing had to be released or the block was
 *         released; ::lh_bool_false when the deallocation callback is absent,
 *         in which case @p ptr stays owned by the caller.
 */
lh_bool_t
lh_memory_allocator_try_dealloc(lh_memory_allocator_t *self, lh_ptr ptr);

/**
 * @brief Resize @p old_ptr through the native reallocation callback only.
 *
 * Returns ::lh_null when no reallocation callback is set or it fails; the
 * original block then stays owned by the caller. Bytes a successful block
 * gained follow ::lh_memory_allocator_init_tail.
 */
lh_ptr
lh_memory_allocator_try_resize(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                               lh_usize_t new_size);

/**
 * @brief Resize @p old_ptr by allocating, copying and releasing.
 *
 * Copies the first min(@p old_size, @p new_size) bytes into a new block and
 * releases @p old_ptr. Returns ::lh_null without touching @p old_ptr when the
 * deallocation callback is absent or allocation fails. @p old_ptr must be
 * nonnull.
 */
lh_ptr
lh_memory_allocator_try_move(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                             lh_usize_t new_size);

/**
 * @brief Whether resizing @p old_ptr to @p new_size calls the allocation callback.
 *
 * True for a positive @p new_size when @p old_ptr is null, or when the sizes
 * differ and no native reallocation callback is set.
 */
lh_bool_t
lh_memory_allocator_realloc_needs_alloc(const lh_memory_allocator_t *self, lh_ptr old_ptr,
                                        lh_usize_t old_size, lh_usize_t new_size);

/**
 * @brief Resize without treating allocation failure as a runtime error.
 *
 * @p old_ptr must belong to @p self and @p old_size must be its actual size.
 * Null input allocates (::lh_memory_allocator_try_alloc); equal sizes return
 * @p old_ptr; zero @p new_size releases a nonnull block
 * (::lh_memory_allocator_try_dealloc) and returns ::lh_null. Otherwise
 * ::lh_memory_allocator_try_resize when a native reallocation callback is set,
 * else ::lh_memory_allocator_try_move.
 * On failure, returns ::lh_null and leaves the original block owned by the
 * caller and unchanged. The allocator and its context must remain valid.
 */
lh_ptr
lh_memory_allocator_try_realloc(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                                lh_usize_t new_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_ALLOCATOR_H */