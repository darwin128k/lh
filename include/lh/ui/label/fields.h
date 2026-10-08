/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_label_t.
 */

#ifndef LH_UI_LABEL_FIELDS_H
#define LH_UI_LABEL_FIELDS_H

/**
 * @def lh_ui_label_fields(container_type, char_type)
 * @brief The container this label is, then the text it shows.
 *
 * The text is not copied. The container is first so an
 * ::lh_ui_entity_t * into the label still reaches the embedded entity.
 *
 * @param container_type Type of the embedded container.
 * @param char_type      Type of one character of the text.
 */
#define lh_ui_label_fields(container_type, char_type)                                        \
    container_type container;                                                                       \
    const char_type *text

#endif /* LH_UI_LABEL_FIELDS_H */
