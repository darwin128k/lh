/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_label_t.
 */

#ifndef LH_GL_LABEL_FIELDS_H
#define LH_GL_LABEL_FIELDS_H

/**
 * @def lh_gl_label_fields(container_type, char_type)
 * @brief The container this label is, then the text it shows.
 *
 * The text is not copied. The container is first so an
 * ::lh_gl_entity_t * into the label still reaches the embedded entity.
 * The metrics fields remember what the text measured -- its size, the top of its
 * ink, its ink rect and where it starts (::lh_gl_label_get_text_origin) -- until
 * the text, the font, the rect, the padding, the alignment or the width it wraps
 * at changes, so a repaint draws the glyphs and measures nothing. `text_hash` / `text_length` are
 * how
 * ::lh_gl_label_set_text_v tells a new string from the same one when the caller
 * has already overwritten the buffer the label points at. `text_noted` is where
 * the text's ink was the last time the label recorded it, **relative to the
 * label's own origin**, which is the one way left to erase an old text whose
 * bytes the caller has already replaced.
 *
 * @param container_type Type of the embedded container.
 * @param char_type      Type of one character of the text.
 */
#define lh_gl_label_fields(container_type, char_type)                                              \
    container_type container;                                                                      \
    const char_type *text;                                                                         \
    lh_bool_t metrics_ready;                                                                       \
    lh_bool_t text_hashed;                                                                         \
    lh_u32_t text_hash;                                                                            \
    lh_u32_t text_length;                                                                          \
    lh_gl_rect_t text_noted;                                                                       \
    const struct lh_font *metrics_font;                                                         \
    lh_gl_rect_t metrics_rect;                                                                     \
    lh_gl_insets_t metrics_padding;                                                                \
    lh_gl_text_align_h_t metrics_align_h;                                                          \
    lh_gl_text_align_v_t metrics_align_v;                                                          \
    lh_gl_point_t metrics_origin;                                                                  \
    lh_gl_scalar_t metrics_ink_top;                                                                \
    lh_gl_scalar_t metrics_wrap;                                                                   \
    lh_gl_rect_t metrics_ink;                                                                      \
    lh_gl_size_t metrics_size

#endif /* LH_GL_LABEL_FIELDS_H */
