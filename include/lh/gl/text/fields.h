/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_text_t.
 */

#ifndef LH_GL_TEXT_FIELDS_H
#define LH_GL_TEXT_FIELDS_H

/**
 * @def lh_gl_text_fields(entity_type, font_type, color_type, char_type)
 * @brief The entity a text starts with, and the three things it borrows.
 *
 * The `entity` member comes first. The font, the color and the string are not
 * copied. A null among them draws nothing.
 *
 * @param entity_type ::lh_gl_entity_t. Must be the first member.
 * @param font_type   ::lh_gl_font_t. The member is `font`, borrowed.
 * @param color_type  ::lh_gl_color_t. The member is `color`, borrowed.
 * @param char_type   ::lh_char_t. The member is `text`, borrowed. One byte is
 *                    one code. A zero byte ends it.
 * @param bool_type   ::lh_bool_t. `shown` draws. `target` takes a press.
 */
#define lh_gl_text_fields(entity_type, font_type, color_type, char_type, bool_type)                \
    entity_type entity;                                                                            \
    const font_type *font;                                                                         \
    const color_type *color;                                                                       \
    const char_type *text;                                                                         \
    bool_type shown;                                                                               \
    bool_type target

#endif /* LH_GL_TEXT_FIELDS_H */
