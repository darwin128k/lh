/**
 * @file shape.h
 * @brief ::lh_ui_input_caret_shape_t — how the caret is drawn.
 */

#ifndef LH_UI_INPUT_CARET_SHAPE_H
#define LH_UI_INPUT_CARET_SHAPE_H

/**
 * @enum lh_ui_input_caret_shape
 * @brief How the caret is drawn.
 *
 * Both take their height from the **font**, never from the text: the line of the font
 * standing on the baseline the label draws on. A caret measured off the text was a
 * different height for "2" and for "d" and a third one in an empty field, and stood
 * where the ink was rather than where the line is.
 *
 * - `bar` — one column, before the character at the caret.
 * - `block` — as wide as the character at the caret (or a '0' at the end of the text),
 *   with that character drawn over it in the field's own fill, the way a terminal does.
 */
typedef enum lh_ui_input_caret_shape
{
    lh_ui_input_caret_bar = 0,
    lh_ui_input_caret_block
} lh_ui_input_caret_shape_t; /**< How the caret is drawn. */

#endif /* LH_UI_INPUT_CARET_SHAPE_H */
