/**
 * @file fields.h
 * @brief Member fields of ::lh_math_quat_t.
 */

#ifndef LH_MATH_QUAT_FIELDS_H
#define LH_MATH_QUAT_FIELDS_H

/**
 * @def lh_math_quat_fields(component_type)
 * @brief The vector part `x, y, z`, then the scalar part `w`.
 *
 * @param component_type Type of each component (::lh_float_t).
 */
#define lh_math_quat_fields(component_type)                                                             \
    component_type x;                                                                              \
    component_type y;                                                                              \
    component_type z;                                                                              \
    component_type w

#endif /* LH_MATH_QUAT_FIELDS_H */
