/**
 * @file initializer.h
 * @brief Initializer macro for pair structs.
 *
 * Provides ::lh_pair_initializer — a convenience wrapper around
 * ::lh_initializer for initializing structs that embed ::lh_pair_fields.
 */

#ifndef LH_PAIR_INITIALIZER_H
#define LH_PAIR_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_pair_initializer(first, second)
 * @brief Create a brace-enclosed initializer list for a pair struct.
 *
 * Expands to `{first, second}` via ::lh_initializer,
 * matching the field order produced by ::lh_pair_fields.
 * Intended to be passed to ::lh_initializer_of or ::lh_initializer_of_struct.
 *
 * @param first  Value for the `first` member.
 * @param second Value for the `second` member.
 *
 * Example usage:
 * @code{.c}
 * struct int_pair { lh_pair_fields(int, int); };
 *
 * struct int_pair p = lh_initializer_of_struct(
 *     int_pair, lh_pair_initializer(1, 2));
 * @endcode
 */
#define lh_pair_initializer(first, second) lh_initializer(first, second)

#endif // LH_PAIR_INITIALIZER_H
