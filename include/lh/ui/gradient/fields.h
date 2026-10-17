/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_gradient_t.
 */

#ifndef LH_UI_GRADIENT_FIELDS_H
#define LH_UI_GRADIENT_FIELDS_H

#include <lh/assert/static.h>
#include <lh/config.h>

/*
 * The used-stop count and every stop index are ::lh_byte_t. The literal 255
 * stands for LH_BYTE_T_MAX, which is not a constant expression in C++.
 */
lh_assert_static(LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS >= 1 &&
                     LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS <= 255,
                 "LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS must be in 1..255");

/**
 * @def lh_ui_gradient_fields(stop_type, count_type, shape_type, angle_type)
 * @brief Fixed stop table, how many entries are in use, and how the stops are laid
 *        over a box: along a line at an angle, or out from the middle.
 *
 * Capacity is ::LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS.
 *
 * @param stop_type  Type of each stop.
 * @param count_type Type of the used-stop count.
 * @param shape_type Type of ::lh_ui_gradient_shape_t.
 * @param angle_type Type of the angle, in whole degrees.
 */
#define lh_ui_gradient_fields(stop_type, count_type, shape_type, angle_type)                       \
    stop_type stops[LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS];                                      \
    count_type stops_count;                                                                        \
    shape_type shape;                                                                              \
    angle_type angle

#endif /* LH_UI_GRADIENT_FIELDS_H */
