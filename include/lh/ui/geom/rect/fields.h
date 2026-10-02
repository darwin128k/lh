/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_rect_t.
 */

#ifndef LH_UI_GEOM_RECT_FIELDS_H
#define LH_UI_GEOM_RECT_FIELDS_H

/**
 * @def lh_ui_rect_fields(point_type, size_type)
 * @brief Top-left corner and extent.
 *
 * @param point_type  Type of `origin` (::lh_ui_point_t).
 * @param size_type   Type of `size` (::lh_ui_size_t).
 */
#define lh_ui_rect_fields(point_type, size_type)                                                   \
    point_type origin;                                                                             \
    size_type size

#endif /* LH_UI_GEOM_RECT_FIELDS_H */
