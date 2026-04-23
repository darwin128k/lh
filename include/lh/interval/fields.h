/**
 * @file fields.h
 * @brief Macro for declaring bounds and flags member fields of an interval.
 */

#ifndef LH_INTERVAL_FIELDS_H
#define LH_INTERVAL_FIELDS_H

#include <lh/interval/flags.h>

/**
 * @def lh_interval_fields(T)
 * @brief Expands to a `bounds` field and a `flags` field for an interval
 * struct.
 *
 * Intended to be embedded inside a struct or union body to inject a typed
 * `bounds` field and an `lh_interval_flags_t flags` field in a single,
 * consistent declaration.
 *
 * @param T A struct type whose fields are declared
 *          via ::lh_interval_bounds_fields.
 *
 *          The type itself (not a variable) is passed here
 *          — it becomes the type of the `bounds` member.
 *
 * Example usage:
 * @code{.c}
 * typedef struct {
 *     lh_interval_bounds_fields(float);
 * } FloatBounds;
 *
 * typedef struct {
 *     lh_interval_fields(FloatBounds);
 * } FloatInterval;
 *
 * FloatInterval i;
 * i.bounds.first = 0.0f;
 * i.bounds.second = 1.0f;
 * i.flags = lh_interval_flags_closed;
 * @endcode
 */
#define lh_interval_fields(T)                                                                      \
    T bounds;                                                                                      \
    lh_interval_flags_t flags

#endif // LH_INTERVAL_FIELDS_H
