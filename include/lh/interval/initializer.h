/**
 * @file initializer.h
 * @brief Initializer macro for interval structs.
 *
 * Provides ::lh_interval_initializer — a convenience wrapper around
 * ::lh_initializer for initializing structs that embed ::lh_interval_fields.
 */

#ifndef LH_INTERVAL_INITIALIZER_H
#define LH_INTERVAL_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_interval_initializer(initializer, flags)
 * @brief Create a brace-enclosed initializer list for an interval struct.
 *
 * Expands to `{initializer, flags}` via ::lh_initializer, * matching the field order produced by
 * ::lh_interval_fields (`bounds` then `flags`). Intended to be passed to ::lh_initializer_of or
 * ::lh_initializer_of_struct.
 *
 * @param initializer Bounds initializer (e.g. ::lh_interval_bounds_initializer).
 * @param flags       Interval flags value (e.g. `lh_interval_flags_closed`).
 *
 * Example usage:
 * @code{.c}
 * struct int_interval { lh_interval_fields(struct int_bounds); };
 *
 * struct int_interval i = lh_initializer_of_struct(
 *     int_interval,
 *     lh_interval_initializer(
 *         lh_interval_bounds_initializer(0, 100),
 *         lh_interval_flags_closed));
 * @endcode
 */
#define lh_interval_initializer(initializer, flags) lh_initializer(initializer, flags)

#endif // LH_INTERVAL_INITIALIZER_H
