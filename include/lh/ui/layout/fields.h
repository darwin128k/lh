/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_layout_t.
 */

#ifndef LH_UI_LAYOUT_FIELDS_H
#define LH_UI_LAYOUT_FIELDS_H

/**
 * @def lh_ui_layout_fields(axis_type, scalar_type, justify_type, bool_type)
 * @brief The way children run, the room between neighbours, where the leftovers
 *        go, and whether children that do not fit start a new line -- with the room
 *        between lines.
 *
 * @param axis_type    Type of ::lh_ui_axis_t.
 * @param scalar_type  Type of the gaps.
 * @param justify_type Type of ::lh_ui_justify_t.
 * @param bool_type    Type of the wrap flag.
 */
#define lh_ui_layout_fields(axis_type, scalar_type, justify_type, bool_type)                       \
    axis_type axis;                                                                                \
    scalar_type gap;                                                                               \
    justify_type justify;                                                                          \
    bool_type wrap;                                                                                \
    scalar_type line_gap

#endif /* LH_UI_LAYOUT_FIELDS_H */