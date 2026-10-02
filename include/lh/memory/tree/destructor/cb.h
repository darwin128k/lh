/**
 * @file cb.h
 * @brief Pointer alias for ::lh_memory_tree_destructor_fn.
 */

#ifndef LH_MEMORY_TREE_DESTRUCTOR_CB_H
#define LH_MEMORY_TREE_DESTRUCTOR_CB_H

#include <lh/memory/tree/destructor/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_memory_tree_destructor_cb
 * @brief Pointer to ::lh_memory_tree_destructor_fn.
 */
#define lh_memory_tree_destructor_cb lh_ptr_of(lh_memory_tree_destructor_fn)

#endif /* LH_MEMORY_TREE_DESTRUCTOR_CB_H */
