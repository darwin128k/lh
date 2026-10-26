/** @file fields.h @brief Parsed HTTP/1 request-line metadata. */
#ifndef LH_HTTP_REQUEST_FIELDS_H
#define LH_HTTP_REQUEST_FIELDS_H

#define lh_http_request_fields(view_type, minor_type, size_type)                                   \
    view_type method;                                                                              \
    view_type target;                                                                              \
    minor_type version_minor;                                                                      \
    size_type header_count

#endif
