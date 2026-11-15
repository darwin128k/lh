/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_text_shadow_t.
 */

#ifndef LH_GL_TEXT_SHADOW_FIELDS_H
#define LH_GL_TEXT_SHADOW_FIELDS_H

/**
 * @def lh_gl_text_shadow_fields(entity_type, line_type, shadow_type, color_type, count_type,
 *      bool_type)
 * @brief The entity, the borrowed line, the borrowed shadow and the scratch.
 *
 * @param entity_type ::lh_gl_entity_t. Must be the first member.
 * @param line_type   ::lh_gl_text_t. `line` is borrowed. The ink is that line's.
 * @param shadow_type ::lh_gl_shadow_t. `shadow` is borrowed. Its color is the
 *                    ink, its spread is the blur, its offset shifts the glyphs.
 * @param color_type  ::lh_gl_color_t. `scratch` is the caller's buffer. It holds
 *                    the ink pixmap and then the blur scratch. Not owned.
 * @param count_type  ::lh_usize_t. How many colours @p scratch holds.
 * @param bool_type   ::lh_bool_t. `shown` draws. A press is always declined.
 */
#define lh_gl_text_shadow_fields(entity_type, line_type, shadow_type, color_type, count_type,      \
                                 bool_type)                                                        \
    entity_type entity;                                                                            \
    const line_type *line;                                                                         \
    const shadow_type *shadow;                                                                     \
    color_type *scratch;                                                                           \
    count_type scratch_count;                                                                      \
    bool_type shown

#endif /* LH_GL_TEXT_SHADOW_FIELDS_H */
