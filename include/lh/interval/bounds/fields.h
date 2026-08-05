/**
 * @file fields.h
 * @brief Macro for interval bound member fields via a homogeneous ::lh_pair_fields.
 *
 * Interval endpoints are a same-typed ordered pair (::lh_pair_fields(@p T, @p T)).
 * Members are named `first` and `second`; for bounds, treat `first` as the lower
 * endpoint and `second` as the upper endpoint (including half-open interval
 * conventions used elsewhere in the library).
 */

#ifndef LH_INTERVAL_BOUNDS_FIELDS_H
#define LH_INTERVAL_BOUNDS_FIELDS_H

#include <lh/pair/fields.h>

/**
 * @def lh_interval_bounds_fields(T)
 * @brief Expands to ::lh_pair_fields(@p T, @p T) — two fields of type @p T.
 *
 * Intended to be embedded inside a struct or union body.
 * Injected members are `first` and `second` (see ::lh_pair_fields).
 * Use them as lower/upper endpoints according to your interval convention.
 *
 * @param T Type of both bound fields (both `first` and `second` have this type).
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
#define lh_interval_bounds_fields(T) lh_pair_fields(T, T)

#endif /* LH_INTERVAL_BOUNDS_FIELDS_H */
