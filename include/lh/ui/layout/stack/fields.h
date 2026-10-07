/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_layout_stack_t.
 */

#ifndef LH_UI_LAYOUT_STACK_FIELDS_H
#define LH_UI_LAYOUT_STACK_FIELDS_H

/**
 * @def lh_ui_layout_stack_fields(axis_type, scalar_type, bool_type)
 * @brief The axis children follow, the gap between them, and whether each
 *        child takes the whole cross size of the content box.
 *
 * @param axis_type   ::lh_ui_axis_t.
 * @param scalar_type ::lh_ui_scalar_t.
 * @param bool_type   ::lh_bool_t.
 */
#define lh_ui_layout_stack_fields(axis_type, scalar_type, bool_type)                                \
    axis_type axis;                                                                                 \
    scalar_type gap;                                                                                \
    bool_type stretch

#endif /* LH_UI_LAYOUT_STACK_FIELDS_H */
