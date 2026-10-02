/**
 * @file fields.h
 * @brief Library-private: member fields of the header of a tree block.
 */

#ifndef LH_SRC_MEMORY_TREE_HEADER_FIELDS_H
#define LH_SRC_MEMORY_TREE_HEADER_FIELDS_H

/**
 * @def lh_memory_tree_header_fields(allocator_type, header_type, node_type, list_type,
 *                                    destructor_type)
 * @brief Where the block came from and where it sits in its tree.
 *
 * - `allocator`: the sized allocator the block (and so every child added to
 *   it) is allocated from and freed back to.
 * - `parent`: the owning block's header, or null for a root.
 * - `sibling`: this block's node in `parent->children`.
 * - `children`: the blocks this one owns, oldest first.
 * - `destructor`: run before the block is freed, or null.
 */
#define lh_memory_tree_header_fields(allocator_type, header_type, node_type, list_type,           \
                                     destructor_type)                                              \
    allocator_type *allocator;                                                                     \
    header_type *parent;                                                                           \
    node_type sibling;                                                                             \
    list_type children;                                                                            \
    destructor_type *destructor

#endif /* LH_SRC_MEMORY_TREE_HEADER_FIELDS_H */
