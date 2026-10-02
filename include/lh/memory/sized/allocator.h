/**
 * @file allocator.h
 * @brief An allocator that remembers the size of every block it hands out.
 *
 * ::lh_memory_sized_allocator_t is an ordinary ::lh_memory_allocator_t — a
 * typedef of it, layout-identical, set up with the same API (callbacks and
 * `self`) — used through the functions below, which prefix each block with a
 * small header holding the block's size. So, unlike the plain allocator,
 * nothing needs the caller to remember sizes: free a block without its size,
 * resize it without its old size, or ask a block how big it is.
 *
 * Layout of every block, as obtained from the allocator's callbacks:
 * @code
 * [ size | padding ][ user data ... ]
 *  <-- 16 bytes -->  ^ the pointer the caller sees
 * @endcode
 * The header is 16 bytes, not just `sizeof(lh_usize_t)`, so the user data
 * keeps the alignment the callbacks give their blocks (16 for `malloc` on
 * x86-64, which SSE loads rely on).
 *
 * Only pointers obtained through these functions may be passed back to them,
 * and blocks must not be mixed with the plain ::lh_memory_allocator_alloc /
 * ::lh_memory_allocator_dealloc on the same allocator: one kind has a header
 * in front of it, the other does not.
 */

#ifndef LH_MEMORY_SIZED_ALLOCATOR_H
#define LH_MEMORY_SIZED_ALLOCATOR_H

#include <lh/compiler/extern/c.h>
#include <lh/memory/allocator.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @def LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE
 * @brief Bytes in front of every block: its size, padded to keep alignment.
 */
#define LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE 16U

/**
 * @typedef lh_memory_sized_allocator_t
 * @brief An ::lh_memory_allocator_t whose blocks carry their size.
 *
 * Same type, same setup (::lh_memory_allocator_init,
 * ::lh_memory_allocator_set_context, ...); the name marks that its blocks go
 * through the lh_memory_sized_allocator_* functions.
 */
typedef lh_memory_allocator_t lh_memory_sized_allocator_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Allocate @p size bytes and remember @p size.
 *
 * A @p size of 0 is allowed: the block holds only its header and the returned
 * pointer must still be freed.
 *
 * @param self Sized allocator.
 * @param size Bytes the caller needs.
 * @return The block's user data (never ::lh_null).
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_overflow
 *        @p size plus the header does not fit in ::lh_usize_t.
 * @fails ::lh_runtime_error_code_memory_not_allocated
 *        The allocation callback could not allocate.
 */
lh_ptr
lh_memory_sized_allocator_alloc(lh_memory_sized_allocator_t *self, lh_usize_t size);

/**
 * @brief Free the block at @p ptr; no size needed.
 *
 * ::lh_null is ignored.
 *
 * @param self Sized allocator that allocated @p ptr.
 * @param ptr  Pointer returned by this allocator, or ::lh_null.
 */
lh_void
lh_memory_sized_allocator_dealloc(lh_memory_sized_allocator_t *self, lh_ptr ptr);

/**
 * @brief Resize the block at @p ptr to @p new_size bytes; no old size needed.
 *
 * Same rules as ::lh_memory_allocator_realloc: ::lh_null allocates, a
 * @p new_size of 0 frees and returns ::lh_null, otherwise the leading bytes
 * are kept and the (possibly moved) block is returned.
 *
 * @param self     Sized allocator that allocated @p ptr.
 * @param ptr      Pointer returned by this allocator, or ::lh_null.
 * @param new_size Requested size in bytes.
 * @return The resized block's user data, or ::lh_null when @p new_size is 0.
 * @fails ::lh_runtime_error_code_overflow
 *        @p new_size plus the header does not fit in ::lh_usize_t.
 * @fails ::lh_runtime_error_code_memory_not_allocated
 *        The allocation callback could not allocate.
 */
lh_ptr
lh_memory_sized_allocator_realloc(lh_memory_sized_allocator_t *self, lh_ptr ptr, lh_usize_t new_size);

/**
 * @brief Size, in bytes, the block at @p ptr was last allocated or resized to.
 *
 * @param ptr Pointer returned by a sized allocator (not ::lh_null).
 * @return The block's size.
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p ptr is ::lh_null.
 */
lh_usize_t
lh_memory_sized_allocator_get_size(const lh_ptr ptr);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_SIZED_ALLOCATOR_H */
