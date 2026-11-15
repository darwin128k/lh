/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_card_t.
 */

#ifndef LH_GL_CARD_FIELDS_H
#define LH_GL_CARD_FIELDS_H

/**
 * @def lh_gl_card_fields(layer_type, fill_type, text_type, scalar_type, bool_type)
 * @brief A layer, the fill and the line inside it, the corner radius and
 *        whether the card is shown.
 *
 * The layer comes first, so a card pointer is also its entity pointer.
 *
 * @param layer_type  ::lh_gl_layer_t. Must be the first member.
 * @param fill_type   ::lh_gl_fill_t. The face. It does not take a press.
 * @param text_type   ::lh_gl_text_t. The line. It does not take a press.
 * @param scalar_type ::lh_gl_scalar_t. `radius`.
 * @param bool_type   ::lh_bool_t. `shown`.
 */
#define lh_gl_card_fields(layer_type, fill_type, text_type, scalar_type, bool_type)                \
    layer_type layer;                                                                              \
    fill_type face;                                                                                \
    text_type label;                                                                               \
    scalar_type radius;                                                                            \
    bool_type shown

#endif /* LH_GL_CARD_FIELDS_H */
