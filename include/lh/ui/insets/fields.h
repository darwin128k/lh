/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_insets_t.
 */

#ifndef LH_UI_INSETS_FIELDS_H
#define LH_UI_INSETS_FIELDS_H

/**
 * @def lh_ui_insets_fields(scalar_type)
 * @brief Distance in from each side of a rect.
 *
 * @param scalar_type ::lh_ui_scalar_t.
 */
#define lh_ui_insets_fields(scalar_type)                                                            \
    scalar_type left;                                                                               \
    scalar_type top;                                                                                \
    scalar_type right;                                                                              \
    scalar_type bottom

#endif /* LH_UI_INSETS_FIELDS_H */
