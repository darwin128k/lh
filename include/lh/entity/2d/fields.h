/**
 * @file fields.h
 * @brief Member fields ::lh_entity_2d_t adds to an entity.
 */

#ifndef LH_ENTITY_2D_FIELDS_H
#define LH_ENTITY_2D_FIELDS_H

/**
 * @def lh_entity_2d_fields(vec2_type, angle_type, style_type, effect_type)
 * @brief Where the entity is, how big its box is, and how that box is
 *        painted. Expanded after ::lh_entity_fields.
 *
 * - `position`, `angle`, `scale`: the place relative to the parent.
 * - `size`: width (`x`) and height (`y`) of the box in the entity's own
 *   space. Zero is no box: nothing is painted and children are not cut.
 * - `style`: pointer to the ::lh_ui_style_t the box is painted with, or
 *   null when there is nothing to paint. The colors live in the style,
 *   not in the entity. The entity does not own the style.
 * - `effect`: pointer to an ::lh_ui_effect_t drawn before the fill, or
 *   null. The entity does not own it.
 *
 * @param vec2_type   Type of `position`, `scale` and `size` (::lh_math_vec2_t).
 * @param angle_type  Type of `angle`, radians about the z axis (::lh_float_t).
 * @param style_type  Type of `style` (`const lh_ui_style_t *`).
 * @param effect_type Type of `effect` (`const lh_ui_effect_t *`).
 */
#define lh_entity_2d_fields(vec2_type, angle_type, style_type, effect_type)                        \
    vec2_type position;                                                                            \
    angle_type angle;                                                                              \
    vec2_type scale;                                                                               \
    vec2_type size;                                                                                \
    style_type style;                                                                              \
    effect_type effect

#endif /* LH_ENTITY_2D_FIELDS_H */
