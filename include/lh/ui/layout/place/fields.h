/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_place_t.
 */

#ifndef LH_UI_LAYOUT_PLACE_FIELDS_H
#define LH_UI_LAYOUT_PLACE_FIELDS_H

/**
 * @def lh_ui_place_fields(size_mode_type, scalar_type, align_type)
 * @brief How much room along the flow, and where on the other axis.
 *
 * @param size_mode_type Type of ::lh_ui_place_size_t.
 * @param scalar_type    Type of the length along the flow.
 * @param align_type     Type of ::lh_ui_place_align_t.
 */
#define lh_ui_place_fields(size_mode_type, scalar_type, align_type) \
    size_mode_type size_mode;                                       \
    scalar_type size;                                               \
    align_type align

#endif /* LH_UI_LAYOUT_PLACE_FIELDS_H */