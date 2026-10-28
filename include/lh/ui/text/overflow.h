/**
 * @file overflow.h
 * @brief What a line of text that is wider than its box does: ::lh_ui_text_overflow_t.
 */

#ifndef LH_UI_TEXT_OVERFLOW_H
#define LH_UI_TEXT_OVERFLOW_H

#include <lh/ui/text/wrap.h>

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

#endif /* LH_UI_TEXT_OVERFLOW_H */
