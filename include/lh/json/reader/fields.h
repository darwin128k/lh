/**
 * @file fields.h
 * @brief Member fields of ::lh_json_reader_t.
 */

#ifndef LH_JSON_READER_FIELDS_H
#define LH_JSON_READER_FIELDS_H

/**
 * @def lh_json_reader_fields(text_type, size_type, error_type)
 * @brief The text being read and where reading stands in it.
 *
 * `text` is the whole JSON text, `pos` the offset of the next character,
 * `depth` how many arrays and objects are open, `error` what stopped the
 * read.
 *
 * @param text_type  Type of `text` (::lh_str_view_t).
 * @param size_type  Type of `pos` and `depth` (::lh_usize_t).
 * @param error_type Type of `error` (::lh_json_error_t).
 */
#define lh_json_reader_fields(text_type, size_type, error_type)                                    \
    text_type text;                                                                                \
    size_type pos;                                                                                 \
    size_type depth;                                                                               \
    error_type error

#endif /* LH_JSON_READER_FIELDS_H */
