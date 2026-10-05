/**
 * @file fields.h
 * @brief Member fields of ::lh_entity_t.
 */

#ifndef LH_ENTITY_FIELDS_H
#define LH_ENTITY_FIELDS_H

/**
 * @def lh_entity_fields(rect_type, class_type)
 * @brief The area this entity covers, and the class it belongs to.
 *
 * @param rect_type  Type of the area.
 * @param class_type Type of the class pointer.
 */
#define lh_entity_fields(rect_type, class_type)                                                     \
    rect_type rect;                                                                                 \
    const class_type *class_p

#endif /* LH_ENTITY_FIELDS_H */
