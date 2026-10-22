/**
 * @file head.h
 * @brief Library-private: the list's head node and the ring rewiring built on it.
 *
 * Only list.c uses these. The head is the ring's sentinel: handing it out, or
 * splicing raw link ranges, would let a caller break the ring, which the
 * public operations never do.
 */

#ifndef LH_SRC_LIST_HEAD_H
#define LH_SRC_LIST_HEAD_H

#include <lh/list.h>

/** @brief The head node of @p self. */
lh_list_node_t *
lh_list_get_head_mut(lh_list_t *self);

/** @brief The head node of @p self, read-only. */
const lh_list_node_t *
lh_list_get_head(const lh_list_t *self);

/** @brief @p node as an element of @p self: ::lh_null for the head, where walking stops. */
lh_list_node_t *
lh_list_as_element(const lh_list_t *self, lh_list_node_t *node);

/** @brief Join every element of @p other between @p prev and @p next, neighbours
 *         in another list, then leave @p other empty. */
lh_void
lh_list_splice_between(lh_list_t *other, lh_list_node_t *prev, lh_list_node_t *next);

#endif /* LH_SRC_LIST_HEAD_H */
