/**
 * @file fields.h
 * @brief Member fields of ::lh_math_size_t.
 */

#ifndef LH_MATH_SIZE_FIELDS_H
#define LH_MATH_SIZE_FIELDS_H

/**
 * @def lh_math_size_fields(coord_type)
 * @brief Width and height.
 *
 * @param coord_type  Type of `width` and `height`.
 */
#define lh_math_size_fields(coord_type)                                                             \
    coord_type width;                                                                               \
    coord_type height

#endif /* LH_MATH_SIZE_FIELDS_H */