/**
 * @file fields.h
 * @brief Macro for interval bound member fields via a homogeneous ::lh_pair_fields.
 *
 * Interval endpoints are a same-typed ordered pair
 * (::lh_pair_fields(@p bound_type, @p bound_type)).
 * Members are named `first` and `second`; for bounds, treat `first` as the lower
 * endpoint and `second` as the upper endpoint (including half-open interval
 * conventions used elsewhere in the library).
 */

#ifndef LH_INTERVAL_BOUNDS_FIELDS_H
#define LH_INTERVAL_BOUNDS_FIELDS_H

#include <lh/pair/fields.h>

/**
 * @def lh_interval_bounds_fields(bound_type)
 * @brief Expands to ::lh_pair_fields(@p bound_type, @p bound_type).
 *
 * Intended to be embedded inside a struct or union body.
 * Injected members are `first` and `second` (see ::lh_pair_fields).
 * Use them as lower/upper endpoints according to your interval convention.
 *
 * @param bound_type Type of both bound fields (`first` and `second`).
 *
 * @see lh_pair_fields
 * @see lh_interval_bounds_initializer
 *
 * Example usage:
 * @code{.c}
 * struct float_bounds {
 *     lh_interval_bounds_fields(float);
 * };
 *
 * struct float_bounds b;
 * b.first = 0.0f;
 * b.second = 1.0f;
 * @endcode
 */
#define lh_interval_bounds_fields(bound_type) lh_pair_fields(bound_type, bound_type)

#endif /* LH_INTERVAL_BOUNDS_FIELDS_H */
