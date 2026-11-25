/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_label_metrics_t.
 */

#ifndef LH_GL_LABEL_METRICS_FIELDS_H
#define LH_GL_LABEL_METRICS_FIELDS_H

/**
 * @def lh_gl_label_metrics_fields()
 * @brief What a label measured the last time.
 *
 * The size of the text, the top of its ink, its ink rect and where it starts,
 * with what they were measured against: the font, the rect, the padding, the
 * alignment and the width it wraps at. Whether they hold is the label's own
 * `metrics_ready`; the label measures again once any input changed, so a repaint draws the glyphs
 * and measures nothing. A part of ::lh_gl_label_t that lives inside it
 * (::lh_gl_label_get_metrics).
 */
#define lh_gl_label_metrics_fields()                                                               \
    lh_gl_rect_t rect;                                                                             \
    lh_gl_rect_t ink;                                                                              \
    lh_gl_insets_t padding;                                                                        \
    lh_gl_point_t origin;                                                                          \
    lh_gl_size_t size;                                                                             \
    const struct lh_font *font;                                                                    \
    lh_gl_scalar_t ink_top;                                                                        \
    lh_gl_scalar_t wrap;                                                                           \
    lh_gl_text_align_h_t align_h;                                                                  \
    lh_gl_text_align_v_t align_v

#endif /* LH_GL_LABEL_METRICS_FIELDS_H */
