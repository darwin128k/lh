/**
 * @file h.h
 * @brief ::lh_ui_text_align_h_t — where the text sits across the box.
 */

#ifndef LH_UI_TEXT_ALIGN_H_H
#define LH_UI_TEXT_ALIGN_H_H

/**
 * @brief Where the text sits across the box.
 *
 * The default, ::lh_ui_text_align_h_left, is what a label did before alignment
 * existed: the text starts at the padding. The other two move the text's left
 * edge to the middle or the right edge of the padded box; text wider than the box
 * starts at the left for all three, because there is nowhere else to start.
 */
typedef enum lh_ui_text_align_h
{
    lh_ui_text_align_h_left = 0,   /**< The text starts at the left of the padded box. */
    lh_ui_text_align_h_center = 1, /**< The text is centred in the padded box. */
    lh_ui_text_align_h_right = 2   /**< The text ends at the right of the padded box. */
} lh_ui_text_align_h_t;

#endif /* LH_UI_TEXT_ALIGN_H_H */
