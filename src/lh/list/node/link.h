/**
 * @file link.h
 * @brief Library-private: raw writes of a node's two links.
 *
 * Only node.c and list.c use these, for operations that rewire links
 * (insert, splice, sort). Not installed and not part of the API: a caller
 * with raw link writes could break a list's ring, which the public
 * operations never do.
 */

#ifndef LH_SRC_LIST_NODE_LINK_H
#define LH_SRC_LIST_NODE_LINK_H

#include <lh/list/node.h>

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Point @p self's `next` at @p next. Nothing else changes. */
lh_void
lh_list_node_set_next(lh_list_node_t *self, lh_list_node_t *next);

/** @brief Point @p self's `prev` at @p prev. Nothing else changes. */
lh_void
lh_list_node_set_prev(lh_list_node_t *self, lh_list_node_t *prev);

/** @brief Put unlinked @p self between neighbours @p prev and @p next. */
lh_void
lh_list_node_link(lh_list_node_t *self, lh_list_node_t *prev, lh_list_node_t *next);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_LIST_NODE_LINK_H */
