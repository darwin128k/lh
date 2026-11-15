/**
 * @file v.h
 * @brief ::lh_gl_text_align_v_t — where the text sits down the box.
 */

#ifndef LH_GL_TEXT_ALIGN_V_H
#define LH_GL_TEXT_ALIGN_V_H

/**
 * @brief Where the text sits down the box.
 *
 * ::lh_gl_text_align_v_top is what a label did before alignment existed;
 * ::lh_gl_text_align_v_center and ::lh_gl_text_align_v_bottom move the text's top
 * edge to the middle or the bottom of the padded box, and text taller than the box
 * starts at the top for all three.
 */
typedef enum lh_gl_text_align_v
{
    lh_gl_text_align_v_top = 0,    /**< The text starts at the top of the padded box. */
    lh_gl_text_align_v_center = 1, /**< The text is centred in the padded box. */
    lh_gl_text_align_v_bottom = 2  /**< The text ends at the bottom of the padded box. */
} lh_gl_text_align_v_t;

#endif /* LH_GL_TEXT_ALIGN_V_H */
