/**
 * @file fields.h
 * @brief Member fields of ::lh_math_rect_t.
 */

#ifndef LH_MATH_RECT_FIELDS_H
#define LH_MATH_RECT_FIELDS_H

/**
 * @def lh_math_rect_fields(point_type, size_type)
 * @brief Top-left corner and extent.
 *
 * @param point_type  Type of `origin`.
 * @param size_type   Type of `size`.
 */
#define lh_math_rect_fields(point_type, size_type)                                                  \
    point_type origin;                                                                              \
    size_type size

#endif /* LH_MATH_RECT_FIELDS_H */