/**
 * @file fields.h
 * @brief Member fields of ::lh_size_t.
 */

#ifndef LH_GEOM_SIZE_FIELDS_H
#define LH_GEOM_SIZE_FIELDS_H

/**
 * @def lh_size_fields(coord_type)
 * @brief Width and height.
 *
 * @param coord_type  Type of `width` and `height` (::lh_coord_t).
 */
#define lh_size_fields(coord_type)                                                                 \
    coord_type width;                                                                              \
    coord_type height

#endif /* LH_GEOM_SIZE_FIELDS_H */
