/**
 * @file fields.h
 * @brief Member fields of ::lh_vec4_t.
 */

#ifndef LH_VEC4_FIELDS_H
#define LH_VEC4_FIELDS_H

/**
 * @def lh_vec4_fields(component_type)
 * @brief The 4 components, `x, y, z, w`, in that order.
 *
 * @param component_type Type of each component (::lh_float_t).
 */
#define lh_vec4_fields(component_type)                                                             \
    component_type x;                                                                              \
    component_type y;                                                                              \
    component_type z;                                                                              \
    component_type w

#endif /* LH_VEC4_FIELDS_H */
