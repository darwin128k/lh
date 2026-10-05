/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_gradient_t.
 */

#ifndef LH_UI_GRADIENT_FIELDS_H
#define LH_UI_GRADIENT_FIELDS_H

/**
 * @def lh_ui_gradient_fields(color_type, kind_type, axis_type)
 * @brief Two stops, the kind, and the axis a linear gradient runs along.
 *
 * @param color_type Type of a stop.
 * @param kind_type  Type of the kind.
 * @param axis_type  Type of the linear axis.
 */
#define lh_ui_gradient_fields(color_type, kind_type, axis_type)                                     \
    color_type from;                                                                                \
    color_type to;                                                                                  \
    kind_type kind;                                                                                 \
    axis_type axis

#endif /* LH_UI_GRADIENT_FIELDS_H */
