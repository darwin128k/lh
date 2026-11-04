/**
 * @file fields.h
 * @brief Member fields of ::lh_str_line_reader_t.
 */

#ifndef LH_STR_LINE_READER_FIELDS_H
#define LH_STR_LINE_READER_FIELDS_H

/**
 * @def lh_str_line_reader_fields(text_type, size_type)
 * @brief The text, the offset of the next unread line, and the 1-based
 *        number of the last line read.
 *
 * @param text_type Type of `text` (::lh_str_view_t).
 * @param size_type Type of `pos` and `number` (::lh_usize_t).
 */
#define lh_str_line_reader_fields(text_type, size_type)                                            \
    text_type text;                                                                                \
    size_type pos;                                                                                 \
    size_type number

#endif /* LH_STR_LINE_READER_FIELDS_H */
