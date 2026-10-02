/**
 * @file fn.h
 * @brief Callable signature for tree block destructors.
 */

#ifndef LH_MEMORY_TREE_DESTRUCTOR_FN_H
#define LH_MEMORY_TREE_DESTRUCTOR_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

/**
 * @typedef lh_memory_tree_destructor_fn
 * @brief Function type `lh_void (lh_ptr)` run on a tree block just before it
 *        is freed (::lh_memory_tree_set_destructor).
 *
 * @param ptr The block's user data. Its children are still alive; the
 *            destructor may use them but must not free @p ptr itself.
 */
typedef lh_void(lh_memory_tree_destructor_fn)(lh_ptr ptr);

#endif /* LH_MEMORY_TREE_DESTRUCTOR_FN_H */
