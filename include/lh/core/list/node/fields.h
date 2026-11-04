/**
 * @file fields.h
 * @brief Member fields of ::lh_list_node_t.
 */

#ifndef LH_LIST_NODE_FIELDS_H
#define LH_LIST_NODE_FIELDS_H

/**
 * @def lh_list_node_fields(link_type)
 * @brief The two links of a doubly linked node.
 *
 * A node that is in no list links to itself both ways.
 *
 * @param link_type Type of `next` and `prev` (`struct lh_list_node *`).
 */
#define lh_list_node_fields(link_type)                                                             \
    link_type next;                                                                                \
    link_type prev

#endif /* LH_LIST_NODE_FIELDS_H */
