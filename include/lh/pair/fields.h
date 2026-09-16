/**
 * @file fields.h
 * @brief Macro for declaring ordered pair member fields (`first` / `second`).
 */

#ifndef LH_PAIR_FIELDS_H
#define LH_PAIR_FIELDS_H

/**
 * @def lh_pair_fields(first_type, second_type)
 * @brief Expands to two member variable declarations for a heterogeneous pair struct.
 *
 * Intended to be embedded inside a struct or union body to inject typed
 * `first` and `second` fields in a single, consistent declaration.
 *
 * @param first_type  Type of the `first` member.
 * @param second_type Type of the `second` member.
 *
 * Example usage:
 * @code{.c}
 * typedef struct {
 *     lh_pair_fields(int, float);
 * } int_float_pair;
 *
 * int_float_pair p;
 * p.first = 1;
 * p.second = 2.5f;
 * @endcode
 */
#define lh_pair_fields(first_type, second_type)                                                    \
    first_type first;                                                                              \
    second_type second

#endif /* LH_PAIR_FIELDS_H */
