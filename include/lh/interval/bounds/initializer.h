/**
 * @file initializer.h
 * @brief Initializer macro for interval bounds structs.
 *
 * Provides ::lh_interval_bounds_initializer — a thin wrapper around
 * ::lh_pair_initializer for structs that embed ::lh_interval_bounds_fields
 * (::lh_pair_fields of the same type).
 */

#ifndef LH_INTERVAL_BOUNDS_INITIALIZER_H
#define LH_INTERVAL_BOUNDS_INITIALIZER_H

#include <lh/pair/initializer.h>

/**
 * @def lh_interval_bounds_initializer(lower, upper)
 * @brief Create a brace-enclosed initializer list for an interval bounds struct.
 *
 * Expands to the same initializer list as ::lh_pair_initializer(lower, upper)
 * (i.e. `{lower, upper}` via ::lh_initializer), matching the `first` / `second`
 * field order from ::lh_interval_bounds_fields.
 * Intended to be passed to ::lh_initializer_of or ::lh_initializer_of_struct.
 *
 * @param lower Value for the `first` member (lower endpoint in typical interval usage).
 * @param upper Value for the `second` member (upper endpoint in typical interval usage).
 *
 * @see lh_pair_initializer
 * @see lh_interval_bounds_fields
 *
 * Example usage:
 * @code{.c}
 * struct int_bounds { lh_interval_bounds_fields(int); };
 *
 * struct int_bounds b = lh_initializer_of_struct(
 *     int_bounds, lh_interval_bounds_initializer(0, 100));
 * @endcode
 */
#define lh_interval_bounds_initializer(lower, upper) lh_pair_initializer(lower, upper)

#endif // LH_INTERVAL_BOUNDS_INITIALIZER_H
