/**
 * @file fields.h
 * @brief Member fields of ::lh_math_vec2_t.
 */

#ifndef LH_MATH_VEC2_FIELDS_H
#define LH_MATH_VEC2_FIELDS_H

/**
 * @def lh_math_vec2_fields(component_type)
 * @brief The 2 components, `x, y`, in that order.
 *
 * @param component_type Type of each component (::lh_float_t).
 */
#define lh_math_vec2_fields(component_type)                                                             \
    component_type x;                                                                              \
    component_type y

#endif /* LH_MATH_VEC2_FIELDS_H */
