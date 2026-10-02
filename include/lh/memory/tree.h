/**
 * @file tree.h
 * @brief Ownership tree of memory blocks: freeing a block frees everything
 *        it owns (the talloc / LVGL model).
 *
 * Every tree block may have a parent block. Freeing a block runs its
 * destructor, then frees its children (each the same way, recursively),
 * then the block itself. So a whole structure — an object, its buffers,
 * the strings inside them — goes away with one ::lh_memory_tree_free of its
 * top block, and nothing below it can be forgotten.
 *
 * A sandbox is just a root: give a module (a plugin, a level, a request) a
 * root block, allocate everything of the module under it, and freeing the
 * root on unload releases whatever the module did not free itself. With
 * ::lh_memory_tree_allocator_init the root also becomes an ordinary
 * ::lh_memory_allocator_t: set it as the runtime allocator while the module
 * runs, and lh's own allocations (arrays, strings, ...) land in the sandbox
 * too.
 *
 * It is all opt-in and deterministic: no scanning, no collector thread. A
 * block without a parent is just a block. Memory comes from a sized
 * allocator (::lh_memory_sized_allocator_t, which may wrap malloc, a static
 * pool, ...); each block costs the sized header plus a tree header (64 bytes
 * on 64-bit targets, 32 on 32-bit ones).
 *
 * Only pointers from these functions may be passed back to them.
 */

#ifndef LH_MEMORY_TREE_H
#define LH_MEMORY_TREE_H

#include <lh/compiler/extern/c.h>
#include <lh/memory/allocator.h>
#include <lh/memory/sized/allocator.h>
#include <lh/memory/tree/destructor/cb.h>
#include <lh/ptr.h>
#include <lh/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Allocate a root block of @p size bytes from @p allocator.
 *
 * A @p size of 0 is allowed: an empty block that only owns children, which
 * is what a sandbox root usually is.
 *
 * @param allocator Sized allocator to take this block, and later its
 *                  children, from (not ::lh_null; must outlive the tree).
 * @param size      Bytes the caller needs.
 * @return The block's user data (never ::lh_null).
 */
lh_ptr
lh_memory_tree_alloc(lh_memory_sized_allocator_t *allocator, lh_usize_t size);

/**
 * @brief Allocate a block of @p size bytes owned by @p parent.
 *
 * It comes from @p parent's allocator and is freed together with @p parent
 * at the latest.
 *
 * @param parent A tree block (not ::lh_null).
 * @param size   Bytes the caller needs.
 * @return The block's user data (never ::lh_null).
 */
lh_ptr
lh_memory_tree_alloc_child(lh_ptr parent, lh_usize_t size);

/**
 * @brief Free @p ptr and everything it owns.
 *
 * Order: @p ptr's destructor, then each child (oldest first) the same way,
 * then @p ptr's memory. A parent's destructor therefore still sees its
 * children. @p ptr is first detached from its own parent. ::lh_null is
 * ignored.
 *
 * @param ptr A tree block, or ::lh_null.
 */
lh_void
lh_memory_tree_free(lh_ptr ptr);

/**
 * @brief Resize @p ptr to @p new_size bytes, keeping its place in the tree.
 *
 * The leading bytes are kept; the block may move, and its parent, children
 * and destructor move with it. A @p new_size of 0 keeps an empty block (use
 * ::lh_memory_tree_free to free).
 *
 * @param ptr      A tree block (not ::lh_null).
 * @param new_size Requested size in bytes.
 * @return The block's (possibly moved) user data.
 */
lh_ptr
lh_memory_tree_realloc(lh_ptr ptr, lh_usize_t new_size);

/**
 * @brief Size, in bytes, @p ptr was last allocated or resized to.
 * @param ptr A tree block (not ::lh_null).
 */
lh_usize_t
lh_memory_tree_get_size(const lh_ptr ptr);

/**
 * @brief Move @p ptr under @p parent, or make it a root with ::lh_null.
 *
 * For an object that has to outlive its current owner. It becomes
 * @p parent's youngest child; its own children come along.
 *
 * @param ptr    A tree block (not ::lh_null).
 * @param parent New owner: a tree block that is not @p ptr nor one of its
 *               descendants (that would make a cycle), or ::lh_null.
 */
lh_void
lh_memory_tree_set_parent(lh_ptr ptr, lh_ptr parent);

/**
 * @brief The block that owns @p ptr, or ::lh_null for a root.
 * @param ptr A tree block (not ::lh_null).
 */
lh_ptr
lh_memory_tree_get_parent(const lh_ptr ptr);

/**
 * @brief @p ptr's oldest child, or ::lh_null when it owns nothing.
 *
 * With ::lh_memory_tree_get_next_sibling, walks what a block owns: for
 * example what a module left allocated in its sandbox root.
 *
 * @param ptr A tree block (not ::lh_null).
 */
lh_ptr
lh_memory_tree_get_first_child(const lh_ptr ptr);

/**
 * @brief The next younger child of @p ptr's parent, or ::lh_null after the
 *        youngest (and for a root).
 * @param ptr A tree block (not ::lh_null).
 */
lh_ptr
lh_memory_tree_get_next_sibling(const lh_ptr ptr);

/**
 * @brief Run @p destructor on @p ptr just before it is freed (::lh_null for
 *        none).
 *
 * The way to release what memory alone does not: a file, a socket, a
 * window owned by the block.
 *
 * @param ptr        A tree block (not ::lh_null).
 * @param destructor Called with @p ptr, or ::lh_null.
 */
lh_void
lh_memory_tree_set_destructor(lh_ptr ptr, lh_memory_tree_destructor_cb destructor);

/**
 * @brief Make @p self an allocator whose blocks are children of @p root.
 *
 * Its alloc is ::lh_memory_tree_alloc_child of @p root, its dealloc
 * ::lh_memory_tree_free and its realloc ::lh_memory_tree_realloc. Installed
 * as the runtime allocator (::lh_runtime_allocator), everything lh allocates
 * meanwhile belongs to @p root and is freed with it at the latest.
 *
 * @param self Allocator to set up (its previous state is overwritten).
 * @param root A tree block (not ::lh_null) that outlives every use of @p self.
 */
lh_void
lh_memory_tree_allocator_init(lh_memory_allocator_t *self, lh_ptr root);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_TREE_H */
