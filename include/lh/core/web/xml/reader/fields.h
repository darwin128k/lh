/**
 * @file fields.h
 * @brief Member fields of ::lh_web_xml_reader_t.
 */

#ifndef LH_WEB_XML_READER_FIELDS_H
#define LH_WEB_XML_READER_FIELDS_H

/**
 * @def lh_web_xml_reader_fields(cursor_type, size_type, error_type)
 * @brief The text being read, where reading stands in it, and what stopped it.
 *
 * `cursor` holds the text and the position, `depth` how many elements are
 * open, `error` what stopped the read.
 *
 * @param cursor_type Type of `cursor` (::lh_str_cursor_t).
 * @param size_type   Type of `depth` (::lh_usize_t).
 * @param error_type  Type of `error` (::lh_web_xml_error_t).
 */
#define lh_web_xml_reader_fields(cursor_type, size_type, error_type)                               \
    cursor_type cursor;                                                                            \
    size_type depth;                                                                               \
    error_type error

#endif /* LH_WEB_XML_READER_FIELDS_H */
