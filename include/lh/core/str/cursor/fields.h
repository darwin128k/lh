/**
 * @file fields.h
 * @brief Member fields of ::lh_str_cursor_t.
 */

#ifndef LH_STR_CURSOR_FIELDS_H
#define LH_STR_CURSOR_FIELDS_H

/**
 * @def lh_str_cursor_fields(text_type, size_type)
 * @brief A text and the place reading stands in it.
 *
 * `text` is the whole text, borrowed; `pos` the offset of the next character
 * to read, from 0 to the size of the text.
 *
 * @param text_type Type of `text` (::lh_str_view_t).
 * @param size_type Type of `pos` (::lh_usize_t).
 */
#define lh_str_cursor_fields(text_type, size_type)                                                 \
    text_type text;                                                                                \
    size_type pos

#endif /* LH_STR_CURSOR_FIELDS_H */
