/**
 * @file fields.h
 * @brief Macro for declaring ordered pair member fields (`first` / `second`).
 */

#ifndef LH_PAIR_FIELDS_H
#define LH_PAIR_FIELDS_H

/**
 * @def lh_pair_fields(T1, T2)
 * @brief Expands to two member variable declarations for a heterogeneous pair struct.
 *
 * Intended to be embedded inside a struct or union body to inject typed
 * `first` and `second` fields in a single, consistent declaration.
 *
 * @param T1 Type of the `first` member.
 * @param T2 Type of the `second` member.
 *
 * Example usage:
 * @code{.c}
 * typedef struct {
 *     lh_pair_fields(int, float);
 * } IntFloatPair;
 *
 * IntFloatPair p;
 * p.first = 1;
 * p.second = 2.5f;
 * @endcode
 */
#define lh_pair_fields(T1, T2)                                                                     \
    T1 first;                                                                                      \
    T2 second

#endif // LH_PAIR_FIELDS_H
