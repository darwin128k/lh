/**
 * @file fields.h
 * @brief Member fields of ::lh_web_js_node_t.
 */

#ifndef LH_WEB_JS_NODE_FIELDS_H
#define LH_WEB_JS_NODE_FIELDS_H

/**
 * @def lh_web_js_node_fields(link_type, kind_type, op_type, flag_type, text_type, list_type)
 * @brief One expression or statement and its place in the tree.
 *
 * `link` threads the node into its parent's `children`, `kind` says what the
 * node is, `op` holds the operator of a unary, update, binary or assignment
 * node, `flag` a boolean literal or the prefix form of an update, `name` an
 * identifier (a variable, a property, a function, a parameter of `catch`) or
 * the flags of a regular expression, `text` the value of a literal (a
 * string unescaped, the digits of a number, the pattern of a regular
 * expression), `children` the operands, elements or statements in order.
 * Which of them a kind uses is listed with the kind (lh/core/web/js/kind.h).
 *
 * @param link_type Type of `link` (::lh_list_node_t).
 * @param kind_type Type of `kind` (::lh_web_js_kind_t).
 * @param op_type   Type of `op` (::lh_web_js_op_t).
 * @param flag_type Type of `flag` (::lh_bool_t).
 * @param text_type Type of `name` and `text` (::lh_str_t).
 * @param list_type Type of `children` (::lh_list_t).
 */
#define lh_web_js_node_fields(link_type, kind_type, op_type, flag_type, text_type, list_type)      \
    link_type link;                                                                                \
    kind_type kind;                                                                                \
    op_type op;                                                                                    \
    flag_type flag;                                                                                \
    text_type name;                                                                                \
    text_type text;                                                                                \
    list_type children

#endif /* LH_WEB_JS_NODE_FIELDS_H */
