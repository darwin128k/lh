/**
 * @file wrap.h
 * @brief ::lh_gl_text_wrap_t — whether a label breaks its text into lines as wide as its padded box.
 */

#ifndef LH_GL_TEXT_WRAP_H
#define LH_GL_TEXT_WRAP_H

/**
 * @brief Whether a label breaks its text into lines as wide as its padded box.
 *
 * ::lh_gl_text_wrap_none is what a label did before: a line ends where the text has a
 * line break and nowhere else. ::lh_gl_text_wrap_words also ends one before a word that
 * would not fit, and inside a word wider than the whole box -- CSS's
 * `white-space: normal`.
 */
typedef enum lh_gl_text_wrap
{
    lh_gl_text_wrap_none = 0,
    lh_gl_text_wrap_words = 1
} lh_gl_text_wrap_t;

#endif /* LH_GL_TEXT_WRAP_H */
