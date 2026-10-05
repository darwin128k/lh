/**
 * @file fields.h
 * @brief Member fields of ::lh_entity_class_t.
 */

#ifndef LH_ENTITY_CLASS_FIELDS_H
#define LH_ENTITY_CLASS_FIELDS_H

/**
 * @def lh_entity_class_fields(event_type, class_type)
 * @brief The event function shared by every instance, and the class it
 *        extends.
 *
 * @param event_type Type of the event function.
 * @param class_type Type of the base class.
 */
#define lh_entity_class_fields(event_type, class_type)                                              \
    event_type event;                                                                               \
    const class_type *base

#endif /* LH_ENTITY_CLASS_FIELDS_H */
