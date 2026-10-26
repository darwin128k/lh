/** @file fields.h @brief Non-owning HTTP field-name and field-value views. */
#ifndef LH_HTTP_HEADER_FIELDS_H
#define LH_HTTP_HEADER_FIELDS_H

#define lh_http_header_fields(view_type)                                                           \
    view_type name;                                                                                \
    view_type value

#endif
