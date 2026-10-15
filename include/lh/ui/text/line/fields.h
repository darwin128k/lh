/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_text_line_t.
 */

#ifndef LH_UI_TEXT_LINE_FIELDS_H
#define LH_UI_TEXT_LINE_FIELDS_H

/**
 * @def lh_ui_text_line_fields(char_type, scalar_type, bool_type)
 * @brief One laid-out line of a text: where it is in the string and what it
 *        measures.
 *
 * `begin .. end` are the bytes the line draws; `next` is where the line after it
 * starts (::lh_null after the last). `width` is the pen advance over the line,
 * spaces included. `ink_top` / `ink_bottom` are the first and last ink rows of
 * its glyphs, down from the top of its line box; both `0` and `inked` false when
 * nothing in it has ink.
 *
 * @param char_type   ::lh_char_t.
 * @param scalar_type ::lh_ui_scalar_t.
 * @param bool_type   ::lh_bool_t.
 */
#define lh_ui_text_line_fields(char_type, scalar_type, bool_type)                                         \
    const char_type *begin;                                                                               \
    const char_type *end;                                                                                 \
    const char_type *next;                                                                                \
    scalar_type width;                                                                                    \
    scalar_type ink_top;                                                                                  \
    scalar_type ink_bottom;                                                                               \
    bool_type inked

#endif /* LH_UI_TEXT_LINE_FIELDS_H */
