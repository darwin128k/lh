/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_scrollbar_t.
 */

#ifndef LH_UI_SCROLLBAR_FIELDS_H
#define LH_UI_SCROLLBAR_FIELDS_H

/**
 * @def lh_ui_scrollbar_fields(entity_type, axis_type, mode_type, container_type, style_type)
 * @brief The entity this scrollbar is (its rect is the track), the axis, when
 *        it is shown, the container it drives, and how the thumb is painted.
 *
 * Neither the container nor the thumb style is owned.
 *
 * @param entity_type    Type of the embedded entity.
 * @param axis_type      Type of the axis.
 * @param mode_type      Type of the show mode.
 * @param container_type Type of the container pointed at.
 * @param style_type     Type of the thumb style pointed at.
 */
#define lh_ui_scrollbar_fields(entity_type, axis_type, mode_type, container_type, style_type) \
    entity_type entity;                                                                             \
    axis_type axis;                                                                                 \
    mode_type mode;                                                                                 \
    container_type *container;                                                                      \
    const style_type *thumb_style

#endif /* LH_UI_SCROLLBAR_FIELDS_H */
