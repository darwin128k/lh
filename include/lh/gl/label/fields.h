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
 * `metrics` (::lh_gl_label_metrics_t) remembers what the text measured -- its size, the top of its
 * ink, its ink rect and where it starts (::lh_gl_label_get_text_origin) -- until
 * the text, the font, the rect, the padding, the alignment or the width it wraps
 * at changes, so a repaint draws the glyphs and measures nothing. `seen`
 * (::lh_gl_label_seen_t) holds the hash and the length that are
 * how
 * ::lh_gl_label_set_text_v tells a new string from the same one when the caller
 * has already overwritten the buffer the label points at, and where
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
    lh_gl_label_seen_t seen;                                                                       \
    lh_bool_t metrics_ready;                                                                       \
    lh_gl_label_metrics_t metrics

#endif /* LH_GL_LABEL_FIELDS_H */
