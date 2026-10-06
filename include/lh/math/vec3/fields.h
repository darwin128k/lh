/**
 * @file fields.h
 * @brief Member fields of ::lh_math_vec3_t.
 */

#ifndef LH_MATH_VEC3_FIELDS_H
#define LH_MATH_VEC3_FIELDS_H

/**
 * @def lh_math_vec3_fields(component_type)
 * @brief The 3 components, `x, y, z`, in that order.
 *
 * @param component_type Type of each component (::lh_math_scalar_t).
 */
#define lh_math_vec3_fields(component_type)                                                             \
    component_type x;                                                                              \
    component_type y;                                                                              \
    component_type z

#endif /* LH_MATH_VEC3_FIELDS_H */
