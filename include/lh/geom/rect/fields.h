/**
 * @file fields.h
 * @brief Member fields of ::lh_rect_t.
 */

#ifndef LH_GEOM_RECT_FIELDS_H
#define LH_GEOM_RECT_FIELDS_H

/**
 * @def lh_rect_fields(point_type, size_type)
 * @brief Top-left corner and extent.
 *
 * @param point_type  Type of `origin` (::lh_point_t).
 * @param size_type   Type of `size` (::lh_size_t).
 */
#define lh_rect_fields(point_type, size_type)                                                      \
    point_type origin;                                                                             \
    size_type size

#endif /* LH_GEOM_RECT_FIELDS_H */
