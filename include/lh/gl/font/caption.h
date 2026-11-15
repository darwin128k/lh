/**
 * @file caption.h
 * @brief A small 8×12 face for captions: ::lh_gl_font_caption.
 *
 * One static font. The run is the codes from space through `r`. Ink exists for
 * space, the digits, `C`, `a`, `d` and `r`. Any other code in that run has no
 * ink and does not advance. The glyphs and their bits stay in the library.
 * Nothing is allocated.
 */

#ifndef LH_GL_FONT_CAPTION_H
#define LH_GL_FONT_CAPTION_H

#include <lh/gl/font.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The shared caption font.
 *
 * Line height 14, ascent 12. A glyph is 8 pixels wide and 12 tall, and the pen
 * steps 8. The first call fills the static glyphs. Later calls return the same
 * font.
 */
const lh_gl_font_t *
lh_gl_font_caption(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_FONT_CAPTION_H */
