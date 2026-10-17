/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_place_t.
 */

#ifndef LH_UI_LAYOUT_PLACE_FIELDS_H
#define LH_UI_LAYOUT_PLACE_FIELDS_H

/**
 * @def lh_ui_place_fields(size_mode_type, scalar_type, align_type, insets_type, size_type)
 * @brief How much room along the flow, where on the other axis, the room kept
 *        clear around it, and the least and most it may be.
 *
 * @param size_mode_type Type of ::lh_ui_place_size_t.
 * @param scalar_type    Type of the length along the flow (or a fill's weight).
 * @param align_type     Type of ::lh_ui_place_align_t.
 * @param insets_type    Type of ::lh_ui_insets_t, for the margin.
 * @param size_type      Type of ::lh_ui_size_t, for the minimum and the maximum.
 */
#define lh_ui_place_fields(size_mode_type, scalar_type, align_type, insets_type, size_type)        \
    size_mode_type size_mode;                                                                      \
    scalar_type size;                                                                              \
    align_type align;                                                                              \
    insets_type margin;                                                                            \
    size_type min;                                                                                 \
    size_type max

#endif /* LH_UI_LAYOUT_PLACE_FIELDS_H */
