/**
 * @file fields.h
 * @brief Macro for declaring the links of ::lh_gx_node_t.
 */

#ifndef LH_GX_NODE_FIELDS_H
#define LH_GX_NODE_FIELDS_H

/**
 * @def lh_gx_node_fields(node_type, list_type, link_type)
 * @brief The owner, the children, and the node's own place among its owner's
 *        children.
 *
 * `owner` is ::lh_null for a node that is not attached — a root, or a node not
 * in any tree yet. `children` is the list of the nodes this one owns, in the
 * order they are drawn. `link` is this node's own entry in its owner's
 * `children`. None of them owns memory: they are links between nodes that live
 * wherever their application put them.
 *
 * @param node_type Type of the owner (::lh_gx_node).
 * @param list_type Type of the list of children (::lh_list_t).
 * @param link_type Type of the node's own link (::lh_list_node_t).
 */
#define lh_gx_node_fields(node_type, list_type, link_type)                                         \
    node_type *owner;                                                                              \
    list_type children;                                                                            \
    link_type link

#endif /* LH_GX_NODE_FIELDS_H */
