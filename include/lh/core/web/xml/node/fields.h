/**
 * @file fields.h
 * @brief Member fields of ::lh_web_xml_node_t.
 */

#ifndef LH_WEB_XML_NODE_FIELDS_H
#define LH_WEB_XML_NODE_FIELDS_H

/**
 * @def lh_web_xml_node_fields(link_type, kind_type, text_type, list_type)
 * @brief One node and its place in the tree.
 *
 * `link` threads the node into its parent's `children`, `kind` says what the
 * node is, `name` holds the name of an element or the target of a processing
 * instruction, `text` the content of a text, raw, comment, CDATA,
 * processing-instruction or doctype node, `attrs` the attributes of an
 * element in order (::lh_web_xml_attr_t), `children` the child nodes of an
 * element or a document in order.
 *
 * @param link_type Type of `link` (::lh_list_node_t).
 * @param kind_type Type of `kind` (::lh_web_xml_kind_t).
 * @param text_type Type of `name` and `text` (::lh_str_t).
 * @param list_type Type of `attrs` and `children` (::lh_list_t).
 */
#define lh_web_xml_node_fields(link_type, kind_type, text_type, list_type)                         \
    link_type link;                                                                                \
    kind_type kind;                                                                                \
    text_type name;                                                                                \
    text_type text;                                                                                \
    list_type attrs;                                                                               \
    list_type children

#endif /* LH_WEB_XML_NODE_FIELDS_H */
