/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_layout_t.
 */

#ifndef LH_UI_LAYOUT_FIELDS_H
#define LH_UI_LAYOUT_FIELDS_H

/**
 * @def lh_ui_layout_fields(axis_type, scalar_type, justify_type)
 * @brief The way children run, the room between neighbours, and where the
 *        leftovers go.
 *
 * @param axis_type    Type of ::lh_ui_axis_t.
 * @param scalar_type  Type of the gap.
 * @param justify_type Type of ::lh_ui_justify_t.
 */
#define lh_ui_layout_fields(axis_type, scalar_type, justify_type) \
    axis_type axis;                                               \
    scalar_type gap;                                              \
    justify_type justify

#endif /* LH_UI_LAYOUT_FIELDS_H */