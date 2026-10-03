/**
 * @file fields.h
 * @brief Member fields ::lh_entity_rect_t adds to a 2D entity.
 */

#ifndef LH_ENTITY_RECT_FIELDS_H
#define LH_ENTITY_RECT_FIELDS_H

/**
 * @def lh_entity_rect_fields(vec2_type, color_type)
 * @brief The rectangle's `size` (width, height) in its own space and the
 *        `color` it is filled with. Expanded after ::lh_entity_2d_fields.
 *
 * @param vec2_type  Type of `size` (::lh_vec2_t).
 * @param color_type Type of `color` (::lh_ui_color_t).
 */
#define lh_entity_rect_fields(vec2_type, color_type)                                               \
    vec2_type size;                                                                                \
    color_type color

#endif /* LH_ENTITY_RECT_FIELDS_H */
