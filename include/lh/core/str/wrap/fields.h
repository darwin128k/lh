/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_str_wrap_t.
 */

#ifndef LH_CORE_STR_WRAP_FIELDS_H
#define LH_CORE_STR_WRAP_FIELDS_H

/**
 * @def lh_str_wrap_fields(char_type, width_type)
 * @brief One line of a text broken to a width.
 *
 * `begin` is the line's first byte; `end` one past its last drawn byte (a space
 * or a line feed it broke at is not part of it); `next` where the next line
 * begins, past the spaces and the line feed it broke at, or ::lh_null when the
 * text ends with this line; `width` the sum of the line's advances.
 *
 * @param char_type  Type of a byte of the text (::lh_char_t).
 * @param width_type Type of a width (::lh_s32_t).
 */
#define lh_str_wrap_fields(char_type, width_type)                                                  \
    const char_type *begin;                                                                        \
    const char_type *end;                                                                          \
    const char_type *next;                                                                         \
    width_type width

#endif /* LH_CORE_STR_WRAP_FIELDS_H */
