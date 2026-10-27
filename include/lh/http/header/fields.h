/**
 * @file fields.h
 * @brief Non-owning HTTP field-name and field-value views.
 */
#ifndef LH_HTTP_HEADER_FIELDS_H
#define LH_HTTP_HEADER_FIELDS_H

/**
 * @def lh_http_header_fields(view_type)
 * @brief Expands to the `name` and `value` members of a header struct.
 *
 * Intended to be embedded inside a struct body so ::lh_http_header_t and
 * compatible wrappers share one declaration shape.
 *
 * @param view_type Type of both members (::lh_str_view_t).
 */
#define lh_http_header_fields(view_type)                                                           \
    view_type name;                                                                                \
    view_type value

#endif /* LH_HTTP_HEADER_FIELDS_H */
