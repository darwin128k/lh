/**
 * @file fields.h
 * @brief Member fields of ::lh_atomic_bool_t.
 */

#ifndef LH_ATOMIC_BOOL_FIELDS_H
#define LH_ATOMIC_BOOL_FIELDS_H

/**
 * @def lh_atomic_bool_fields(counter_type)
 * @brief The flag, kept as 0 or 1 in a shared counter.
 *
 * @param counter_type ::lh_atomic_uint_t.
 */
#define lh_atomic_bool_fields(counter_type) counter_type value

#endif /* LH_ATOMIC_BOOL_FIELDS_H */
