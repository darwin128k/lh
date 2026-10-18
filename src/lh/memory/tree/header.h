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
#include <lh/memory/tree/destructor/cb.h>
#include <lh/memory/tree/header/fields.h>

/**
 * @struct lh_memory_tree_header
 * @brief Fields via ::lh_memory_tree_header_fields.
 */
struct lh_memory_tree_header
{
    /** Members: ::lh_memory_tree_header_fields. */
    lh_memory_tree_header_fields(lh_memory_sized_allocator_t, struct lh_memory_tree_header,
                                 lh_list_node_t, lh_list_t, lh_memory_tree_destructor_fn);
};
/** @copybrief lh_memory_tree_header */
typedef struct lh_memory_tree_header lh_memory_tree_header_t;

/**
 * @def LH_MEMORY_TREE_HEADER_SIZE
 * @brief The header rounded up to 16 bytes, so user data keeps the alignment
 *        the sized allocator gives its blocks.
 */
#define LH_MEMORY_TREE_HEADER_SIZE ((sizeof(lh_memory_tree_header_t) + 15U) / 16U * 16U)

/* The header's fields go through these and only these; the header is
   library-private, so are they. */

/** @brief The header in front of the user data @p ptr. */
lh_memory_tree_header_t *
lh_memory_tree_header_of(const lh_ptr ptr);

/** @brief The user data behind @p header. */
lh_ptr
lh_memory_tree_header_get_data(const lh_memory_tree_header_t *header);

/** @brief The header whose @c sibling node is @p node; ::lh_null for ::lh_null. */
lh_memory_tree_header_t *
lh_memory_tree_header_of_sibling(const lh_list_node_t *node);

/** @brief The sized allocator the block came from. */
lh_memory_sized_allocator_t *
lh_memory_tree_header_get_allocator(const lh_memory_tree_header_t *header);

/** @brief Store the sized allocator the block came from. */
lh_void
lh_memory_tree_header_set_allocator(lh_memory_tree_header_t *header,
                                    lh_memory_sized_allocator_t *allocator);

/** @brief The owning block's header, or ::lh_null for a root. */
lh_memory_tree_header_t *
lh_memory_tree_header_get_parent(const lh_memory_tree_header_t *header);

/** @brief Store the owning block's header; the lists are not touched. */
lh_void
lh_memory_tree_header_set_parent(lh_memory_tree_header_t *header, lh_memory_tree_header_t *parent);

/** @brief This block's node in its parent's children. */
lh_list_node_t *
lh_memory_tree_header_get_sibling(lh_memory_tree_header_t *header);

/** @brief The blocks this one owns, oldest first. */
lh_list_t *
lh_memory_tree_header_get_children(lh_memory_tree_header_t *header);

/** @brief The destructor run before the block is freed, or ::lh_null. */
lh_memory_tree_destructor_cb
lh_memory_tree_header_get_destructor(const lh_memory_tree_header_t *header);

/** @brief Store the destructor run before the block is freed (::lh_null for none). */
lh_void
lh_memory_tree_header_set_destructor(lh_memory_tree_header_t *header,
                                     lh_memory_tree_destructor_cb destructor);

/** @brief The first child's header, or ::lh_null. */
lh_memory_tree_header_t *
lh_memory_tree_header_get_first_child(lh_memory_tree_header_t *header);

/** @brief Append @p header to @p parent's children (::lh_null @p parent: nothing). */
lh_void
lh_memory_tree_header_link(lh_memory_tree_header_t *header, lh_memory_tree_header_t *parent);

/** @brief A fresh header from @p allocator under @p parent (::lh_null for a root). */
lh_void
lh_memory_tree_header_init(lh_memory_tree_header_t *header, lh_memory_sized_allocator_t *allocator,
                           lh_memory_tree_header_t *parent);

/** @brief A new block of @p size user bytes from @p allocator under @p parent
 *         (::lh_null for a root), as its header. */
lh_memory_tree_header_t *
lh_memory_tree_header_new(lh_memory_sized_allocator_t *allocator, lh_memory_tree_header_t *parent,
                          lh_usize_t size);

#endif /* LH_SRC_MEMORY_TREE_HEADER_H */
