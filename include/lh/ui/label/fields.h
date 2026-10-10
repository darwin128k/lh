/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_label_t.
 */

#ifndef LH_UI_LABEL_FIELDS_H
#define LH_UI_LABEL_FIELDS_H

/**
 * @def lh_ui_label_fields(container_type, char_type)
 * @brief The container this label is, then the text it shows.
 *
 * The text is not copied. The container is first so an
 * ::lh_ui_entity_t * into the label still reaches the embedded entity.
 * The metrics fields remember where the ink went (::lh_ui_label_get_text_origin)
 * until the text, the font, the rect or the alignment changes, so a repaint
 * draws the glyphs and measures nothing. `text_hash` / `text_length` are how
 * ::lh_ui_label_set_text tells a new string from the same one when the caller
 * has already overwritten the buffer the label points at.
 *
 * @param container_type Type of the embedded container.
 * @param char_type      Type of one character of the text.
 */
#define lh_ui_label_fields(container_type, char_type)                                        \
    container_type container;                                                                       \
    const char_type *text;                                                                          \
    lh_bool_t metrics_ready;                                                                        \
    lh_bool_t text_hashed;                                                                          \
    lh_u32_t text_hash;                                                                             \
    lh_u32_t text_length;                                                                           \
    const struct lh_ui_font *metrics_font;                                                         \
    lh_ui_rect_t metrics_rect;                                                                      \
    lh_ui_insets_t metrics_padding;                                                                 \
    lh_ui_text_align_h_t metrics_align_h;                                                          \
    lh_ui_text_align_v_t metrics_align_v;                                                          \
    lh_ui_point_t metrics_origin;                                                                   \
    lh_ui_scalar_t metrics_ink_top;                                                                \
    lh_ui_size_t metrics_size

#endif /* LH_UI_LABEL_FIELDS_H */
