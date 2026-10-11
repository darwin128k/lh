/**
 * @file overflow.h
 * @brief What a line of text that is wider than its box does: ::lh_ui_text_overflow_t.
 */

#ifndef LH_UI_TEXT_OVERFLOW_H
#define LH_UI_TEXT_OVERFLOW_H

/**
 * @brief What a label draws of a line that does not fit across its padded box.
 *
 * ::lh_ui_text_overflow_clip is what a label did before it could say anything else:
 * the line is drawn whole and the box's clip cuts it, mid-letter if that is where the
 * edge is. ::lh_ui_text_overflow_ellipsis draws as much of the line as fits **with**
 * an ellipsis after it, so what is cut is said to be cut -- CSS's
 * `text-overflow: ellipsis`. Only a single line is shortened; text with line breaks
 * in it is drawn as it is.
 */
typedef enum lh_ui_text_overflow
{
    lh_ui_text_overflow_clip = 0,
    lh_ui_text_overflow_ellipsis = 1
} lh_ui_text_overflow_t;

/**
 * @brief Whether a label breaks its text into lines as wide as its padded box.
 *
 * ::lh_ui_text_wrap_none is what a label did before: a line ends where the text has a
 * line break and nowhere else. ::lh_ui_text_wrap_words also ends one before a word that
 * would not fit, and inside a word wider than the whole box -- CSS's
 * `white-space: normal`.
 */
typedef enum lh_ui_text_wrap
{
    lh_ui_text_wrap_none = 0,
    lh_ui_text_wrap_words = 1
} lh_ui_text_wrap_t;

#endif /* LH_UI_TEXT_OVERFLOW_H */
