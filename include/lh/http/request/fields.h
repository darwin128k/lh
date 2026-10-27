/**
 * @file fields.h
 * @brief Parsed HTTP/1 request-line metadata.
 */
#ifndef LH_HTTP_REQUEST_FIELDS_H
#define LH_HTTP_REQUEST_FIELDS_H

/**
 * @def lh_http_request_fields(view_type, minor_type, size_type)
 * @brief Expands to the request-line members of a request struct.
 *
 * Intended to be embedded inside a struct body so ::lh_http_request_t and
 * compatible wrappers share one declaration shape.
 *
 * @param view_type  Type of the method and target views (::lh_str_view_t).
 * @param minor_type Type of the minor version (::lh_u8_t).
 * @param size_type  Type of the field count (::lh_usize_t).
 */
#define lh_http_request_fields(view_type, minor_type, size_type)                                   \
    view_type method;                                                                              \
    view_type target;                                                                              \
    minor_type version_minor;                                                                      \
    size_type header_count

#endif /* LH_HTTP_REQUEST_FIELDS_H */
