/**
 * @file header.h
 * @brief Library-private: the header in front of every tree block.
 *
 * A tree block is a sized-allocator block whose user data starts with this
 * header; the caller's data follows it at ::LH_MEMORY_TREE_HEADER_SIZE:
 * @code
 * [ sized header ][ tree header | padding ][ user data ... ]
 *                  ^ lh_memory_tree_header_t ^ the pointer the caller sees
 * @endcode
 */

#ifndef LH_SRC_MEMORY_TREE_HEADER_H
#define LH_SRC_MEMORY_TREE_HEADER_H

#include <lh/list.h>
#include <lh/memory/sized/allocator.h>
#include <lh/memory/tree/destructor/fn.h>
#include <lh/memory/tree/header/fields.h>

/**
 * @struct lh_memory_tree_header
 * @brief Fields via ::lh_memory_tree_header_fields.
 */
struct lh_memory_tree_header
{
    lh_memory_tree_header_fields(lh_memory_sized_allocator_t, struct lh_memory_tree_header,
                                 lh_list_node_t, lh_list_t, lh_memory_tree_destructor_fn);
};
typedef struct lh_memory_tree_header lh_memory_tree_header_t;

/**
 * @def LH_MEMORY_TREE_HEADER_SIZE
 * @brief The header rounded up to 16 bytes, so user data keeps the alignment
 *        the sized allocator gives its blocks.
 */
#define LH_MEMORY_TREE_HEADER_SIZE ((sizeof(lh_memory_tree_header_t) + 15U) / 16U * 16U)

#endif /* LH_SRC_MEMORY_TREE_HEADER_H */
