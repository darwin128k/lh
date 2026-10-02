/**
 * @file link.h
 * @brief Library-private: raw writes of a node's two links.
 *
 * Only node.c and list.c use these, for operations that rewire many links
 * at once (splice, sort). Not installed and not part of the API: a caller
 * with raw link writes could break a list's ring, which the public
 * operations never do.
 */

#ifndef LH_SRC_LIST_NODE_LINK_H
#define LH_SRC_LIST_NODE_LINK_H

#include <lh/list/node.h>

/** @brief Point @p self's `next` at @p next. Nothing else changes. */
void
lh_list_node_set_next(lh_list_node_t *self, lh_list_node_t *next);

/** @brief Point @p self's `prev` at @p prev. Nothing else changes. */
void
lh_list_node_set_prev(lh_list_node_t *self, lh_list_node_t *prev);

#endif /* LH_SRC_LIST_NODE_LINK_H */
