/**
 * @file fields.h
 * @brief Member fields of ::lh_web_xml_attr_t.
 */

#ifndef LH_WEB_XML_ATTR_FIELDS_H
#define LH_WEB_XML_ATTR_FIELDS_H

/**
 * @def lh_web_xml_attr_fields(link_type, text_type, flag_type)
 * @brief One attribute and its place in the element.
 *
 * `link` threads the attribute into its element's `attrs`, `name` is the
 * attribute name, `value` its value (unescaped), `bare` says the attribute
 * is written without a value (`<input disabled>`).
 *
 * @param link_type Type of `link` (::lh_list_node_t).
 * @param text_type Type of `name` and `value` (::lh_str_t).
 * @param flag_type Type of `bare` (::lh_bool_t).
 */
#define lh_web_xml_attr_fields(link_type, text_type, flag_type)                                    \
    link_type link;                                                                                \
    text_type name;                                                                                \
    text_type value;                                                                               \
    flag_type bare

#endif /* LH_WEB_XML_ATTR_FIELDS_H */
