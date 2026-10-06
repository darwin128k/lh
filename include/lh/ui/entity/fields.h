/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_entity_t.
 */

#ifndef LH_UI_ENTITY_FIELDS_H
#define LH_UI_ENTITY_FIELDS_H

/**
 * @def lh_ui_entity_fields(rect_type, style_type, class_type)
 * @brief The area this entity covers, how it is painted, and the class it
 *        belongs to.
 *
 * The style is not copied: several entities may share one.
 *
 * @param rect_type  Type of the area.
 * @param style_type Type of the paint recipe pointed at.
 * @param class_type Type of the class pointer.
 */
#define lh_ui_entity_fields(rect_type, style_type, class_type)                                      \
    rect_type rect;                                                                                 \
    const style_type *style;                                                                        \
    const class_type *class_p

#endif /* LH_UI_ENTITY_FIELDS_H */
