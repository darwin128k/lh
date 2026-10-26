/** @file fields.h @brief HTTP parser budgets and last-call diagnostics. */
#ifndef LH_HTTP_PARSER_FIELDS_H
#define LH_HTTP_PARSER_FIELDS_H

#define lh_http_parser_fields(size_type, error_type)                                               \
    size_type head_size_max;                                                                       \
    size_type line_size_max;                                                                       \
    size_type consumed;                                                                            \
    size_type error_offset;                                                                        \
    error_type error

#endif
