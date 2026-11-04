/**
 * @file fields.h
 * @brief Member fields of ::lh_ini_parser_t.
 */

#ifndef LH_INI_PARSER_FIELDS_H
#define LH_INI_PARSER_FIELDS_H

/**
 * @def lh_ini_parser_fields(reader_type, text_type, error_type)
 * @brief Where reading stands in the text, and what the INI grammar has
 *        seen of it.
 *
 * `reader` walks the lines and counts them, `section` is the name of the
 * last header read, `error` what stopped the parse.
 *
 * @param reader_type Type of `reader` (::lh_str_line_reader_t).
 * @param text_type   Type of `section` (::lh_str_view_t).
 * @param error_type  Type of `error` (::lh_ini_error_t).
 */
#define lh_ini_parser_fields(reader_type, text_type, error_type)                                   \
    reader_type reader;                                                                            \
    text_type section;                                                                             \
    error_type error

#endif /* LH_INI_PARSER_FIELDS_H */
