/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_gradient_t.
 */

#ifndef LH_UI_GRADIENT_FIELDS_H
#define LH_UI_GRADIENT_FIELDS_H

#include <lh/config.h>

/**
 * @def lh_ui_gradient_fields(stop_type, count_type)
 * @brief Fixed stop table and how many entries are in use.
 *
 * Capacity is ::LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS.
 *
 * @param stop_type  Type of each stop.
 * @param count_type Type of the used-stop count.
 */
#define lh_ui_gradient_fields(stop_type, count_type)                                                \
    stop_type stops[LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS];                                       \
    count_type stops_count

#endif /* LH_UI_GRADIENT_FIELDS_H */
