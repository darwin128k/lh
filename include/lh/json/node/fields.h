/**
 * @file fields.h
 * @brief Member fields of ::lh_json_node_t.
 */

#ifndef LH_JSON_NODE_FIELDS_H
#define LH_JSON_NODE_FIELDS_H

/**
 * @def lh_json_node_fields(link_type, kind_type, flag_type, text_type, list_type)
 * @brief One JSON value and its place in the tree.
 *
 * `link` threads the node into its parent's `children`, `kind` says which
 * value it is, `flag` holds a bool, `key` the member name inside an object,
 * `text` a string (unescaped) or a number (as written), `children` the
 * elements of an array or the members of an object.
 *
 * @param link_type Type of `link` (::lh_list_node_t).
 * @param kind_type Type of `kind` (::lh_json_kind_t).
 * @param flag_type Type of `flag` (::lh_bool_t).
 * @param text_type Type of `key` and `text` (::lh_str_t).
 * @param list_type Type of `children` (::lh_list_t).
 */
#define lh_json_node_fields(link_type, kind_type, flag_type, text_type, list_type)                 \
    link_type link;                                                                                \
    kind_type kind;                                                                                \
    flag_type flag;                                                                                \
    text_type key;                                                                                 \
    text_type text;                                                                                \
    list_type children

#endif /* LH_JSON_NODE_FIELDS_H */
