/** @file fields.h @brief Parsed HTTP/1 status-line metadata. */
#ifndef LH_HTTP_RESPONSE_FIELDS_H
#define LH_HTTP_RESPONSE_FIELDS_H

#define lh_http_response_fields(view_type, status_type, minor_type, size_type)                     \
    view_type reason;                                                                              \
    status_type status;                                                                            \
    minor_type version_minor;                                                                      \
    size_type header_count

#endif
