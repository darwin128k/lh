/**
 * @file fields.h
 * @brief Parsed HTTP/1 status-line metadata.
 */
#ifndef LH_HTTP_RESPONSE_FIELDS_H
#define LH_HTTP_RESPONSE_FIELDS_H

/**
 * @def lh_http_response_fields(view_type, status_type, minor_type, size_type)
 * @brief Expands to the status-line members of a response struct.
 *
 * Intended to be embedded inside a struct body so ::lh_http_response_t and
 * compatible wrappers share one declaration shape.
 *
 * @param view_type   Type of the reason view (::lh_str_view_t).
 * @param status_type Type of the status code (::lh_u16_t).
 * @param minor_type  Type of the minor version (::lh_u8_t).
 * @param size_type   Type of the field count (::lh_usize_t).
 */
#define lh_http_response_fields(view_type, status_type, minor_type, size_type)                     \
    view_type reason;                                                                              \
    status_type status;                                                                            \
    minor_type version_minor;                                                                      \
    size_type header_count

#endif /* LH_HTTP_RESPONSE_FIELDS_H */
