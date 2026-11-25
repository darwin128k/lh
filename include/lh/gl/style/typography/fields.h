/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_style_typography_t.
 */

#ifndef LH_GL_STYLE_TYPOGRAPHY_FIELDS_H
#define LH_GL_STYLE_TYPOGRAPHY_FIELDS_H

/**
 * @def lh_gl_style_typography_fields(paint_type, font_type)
 * @brief The typography of a style: how an entity draws its text.
 *
 * The font (not owned, may be ::lh_null), the paint the text is drawn with
 * (held by value), where inside the padded box the text starts (`align_h`,
 * `align_v`), what a line too wide for it draws (`overflow`) and whether the
 * text is broken into lines that fit (`wrap`). A part of ::lh_gl_style_t that
 * lives inside it (::lh_gl_style_get_typography).
 *
 * @param paint_type   ::lh_gl_paint_t.
 * @param font_type    ::lh_gl_font_t.
 */
#define lh_gl_style_typography_fields(paint_type, font_type)                                       \
    paint_type paint;                                                                              \
    const font_type *font;                                                                         \
    lh_gl_text_align_h_t align_h;                                                                  \
    lh_gl_text_align_v_t align_v;                                                                  \
    lh_gl_text_overflow_t overflow;                                                                \
    lh_gl_text_wrap_t wrap

#endif /* LH_GL_STYLE_TYPOGRAPHY_FIELDS_H */
