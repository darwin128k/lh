/**
 * @file fields.h
 * @brief Member fields of ::lh_entity_label_t.
 */

#ifndef LH_ENTITY_LABEL_FIELDS_H
#define LH_ENTITY_LABEL_FIELDS_H

/**
 * @def lh_entity_label_fields(entity_type, char_type)
 * @brief The entity this label is, then the text it shows.
 *
 * The text is not copied.
 *
 * @param entity_type Type of the embedded entity.
 * @param char_type   Type of one character of the text.
 */
#define lh_entity_label_fields(entity_type, char_type)                                              \
    entity_type entity;                                                                             \
    const char_type *text

#endif /* LH_ENTITY_LABEL_FIELDS_H */
