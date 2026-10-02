/**
 * @file fields.h
 * @brief Member fields of ::lh_entity_class_t.
 */

#ifndef LH_ENTITY_CLASS_FIELDS_H
#define LH_ENTITY_CLASS_FIELDS_H

/**
 * @def lh_entity_class_fields(class_type, size_type, constructor_type, destructor_type, event_type)
 * @brief What a class is built on and what it adds, in this order.
 *
 * @param class_type       Type `base` points to (`struct lh_entity_class`);
 *                         `base` is null only for ::lh_entity_base_class.
 * @param size_type        Type of `size`: bytes of one instance, including
 *                         every base's fields (::lh_usize_t).
 * @param constructor_type Function type of `constructor` (may be null).
 * @param destructor_type  Function type of `destructor` (may be null).
 * @param event_type       Function type of `event` (may be null).
 */
#define lh_entity_class_fields(class_type, size_type, constructor_type, destructor_type,          \
                               event_type)                                                         \
    const class_type *base;                                                                        \
    size_type size;                                                                                \
    constructor_type *constructor;                                                                 \
    destructor_type *destructor;                                                                   \
    event_type *event

#endif /* LH_ENTITY_CLASS_FIELDS_H */
