/**
 * @file fields.h
 * @brief Member fields of ::lh_math_ipoint_t.
 */

#ifndef LH_MATH_IPOINT_FIELDS_H
#define LH_MATH_IPOINT_FIELDS_H

/**
 * @def lh_math_ipoint_fields(coord_type)
 * @brief Horizontal and vertical coordinate.
 *
 * @param coord_type  Type of `x` and `y`.
 */
#define lh_math_ipoint_fields(coord_type)                                                            \
    coord_type x;                                                                                   \
    coord_type y

#endif /* LH_MATH_IPOINT_FIELDS_H */