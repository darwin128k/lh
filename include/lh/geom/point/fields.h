/**
 * @file fields.h
 * @brief Member fields of ::lh_point_t.
 */

#ifndef LH_GEOM_POINT_FIELDS_H
#define LH_GEOM_POINT_FIELDS_H

/**
 * @def lh_point_fields(coord_type)
 * @brief Horizontal and vertical coordinate.
 *
 * @param coord_type  Type of `x` and `y` (::lh_coord_t).
 */
#define lh_point_fields(coord_type)                                                                \
    coord_type x;                                                                                  \
    coord_type y

#endif /* LH_GEOM_POINT_FIELDS_H */
