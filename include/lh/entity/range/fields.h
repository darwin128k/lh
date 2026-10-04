/**
 * @file fields.h
 * @brief Member fields a ranged widget adds after its 2D fields.
 */

#ifndef LH_ENTITY_RANGE_FIELDS_H
#define LH_ENTITY_RANGE_FIELDS_H

/**
 * @def lh_entity_range_fields(int_type)
 * @brief Inclusive `minimum` and `maximum`, and the `value` kept inside
 *        them. A trackbar, a progress bar, a knob, a spin box and a
 *        scrollbar all start with this.
 *
 * @param int_type Type of the three numbers (::lh_int_t).
 */
#define lh_entity_range_fields(int_type)                                                           \
    int_type minimum;                                                                              \
    int_type maximum;                                                                              \
    int_type value

#endif /* LH_ENTITY_RANGE_FIELDS_H */
