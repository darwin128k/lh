/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_transition_t.
 */

#ifndef LH_UI_TRANSITION_FIELDS_H
#define LH_UI_TRANSITION_FIELDS_H

/**
 * @def lh_ui_transition_fields(entity_type, style_type)
 * @brief One look in flight: the entity it belongs to, the style it is heading for,
 *        the look it set out from, the look on screen, and the time.
 *
 * `from` and `look` are **copies**. The look an entity sets out from is often no
 * style at all -- it is the half-finished look of a transition that was turned round
 * -- and the look on screen is new every frame, so neither can be a pointer into the
 * application's styles. The entity points at `look` while the transition runs, and at
 * `to` again when it lands. `entity` of ::lh_null is a slot with nothing in it.
 *
 * @param entity_type Type of the entity in flight.
 * @param style_type  ::lh_ui_style_t.
 */
#define lh_ui_transition_fields(entity_type, style_type)                                            \
    entity_type *entity;                                                                            \
    const style_type *to;                                                                           \
    style_type from;                                                                                \
    style_type look;                                                                                \
    lh_u16_t elapsed;                                                                               \
    lh_u16_t total

#endif /* LH_UI_TRANSITION_FIELDS_H */
