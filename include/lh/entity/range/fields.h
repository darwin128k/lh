/**
 * @file fields.h
 * @brief Member fields a ranged widget adds after its 2D fields.
 */

#ifndef LH_ENTITY_RANGE_FIELDS_H
#define LH_ENTITY_RANGE_FIELDS_H

/**
 * @def lh_entity_range_fields(int_type)
 * @brief Inclusive `minimum` and `maximum`, the `start` value a reset returns
 *        to, the `value` kept inside them, and the `thickness` of the bar that
 *        shows it. A trackbar, a progress bar, a knob, a spin box and a
 *        scrollbar all start with this.
 *
 * Every number a ranged widget draws or drags with lives here, so nothing in
 * it is written down twice: the thickness is a field rather than a constant in
 * the paint path, and the travel is derived from the entity's own size.
 *
 * @param int_type Type of the five numbers (::lh_int_t).
 */
#define lh_entity_range_fields(int_type)                                                           \
    int_type minimum;                                                                              \
    int_type maximum;                                                                              \
    int_type start;                                                                                \
    int_type value;                                                                                \
    int_type thickness

#endif /* LH_ENTITY_RANGE_FIELDS_H */
