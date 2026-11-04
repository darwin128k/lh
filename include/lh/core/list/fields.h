/**
 * @file fields.h
 * @brief Member fields of ::lh_list_t.
 */

#ifndef LH_LIST_FIELDS_H
#define LH_LIST_FIELDS_H

/**
 * @def lh_list_fields(node_type)
 * @brief The head: a node that is not an element.
 *
 * The list is a ring through `head`: `head.next` is the first element,
 * `head.prev` the last, and an empty list's head links to itself.
 *
 * @param node_type Type of `head` (::lh_list_node_t).
 */
#define lh_list_fields(node_type) node_type head

#endif /* LH_LIST_FIELDS_H */
