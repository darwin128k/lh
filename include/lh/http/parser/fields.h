/**
 * @file fields.h
 * @brief HTTP parser budgets and last-call diagnostics.
 */
#ifndef LH_HTTP_PARSER_FIELDS_H
#define LH_HTTP_PARSER_FIELDS_H

/**
 * @def lh_http_parser_fields(size_type, error_type)
 * @brief Expands to the budget and diagnostic members of a parser struct.
 *
 * Intended to be embedded inside a struct body so ::lh_http_parser_t and
 * compatible wrappers share one declaration shape.
 *
 * @param size_type  Type of the four byte counters (::lh_usize_t).
 * @param error_type Type of the error category (::lh_http_error_t).
 */
#define lh_http_parser_fields(size_type, error_type)                                               \
    size_type head_size_max;                                                                       \
    size_type line_size_max;                                                                       \
    size_type consumed;                                                                            \
    size_type error_offset;                                                                        \
    error_type error

#endif /* LH_HTTP_PARSER_FIELDS_H */
