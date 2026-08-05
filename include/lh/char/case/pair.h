/**
 * @file pair.h
 * @brief Key/value pair for narrow (::lh_char_t) case tables
 *        (fold and simple lower/upper maps when both scalars fit in one code unit).
 *
 * Table rows are sorted by `first` (the source code unit)
 * for use with ::lh_interval_ropen_binary_search();
 * `second` holds the mapped code unit.
 *
 * Both members use ::lh_char_t via ::lh_pair_fields.
 */

#ifndef LH_CHAR_CASE_PAIR_H
#define LH_CHAR_CASE_PAIR_H

#include <lh/char.h>
#include <lh/pair/fields.h>

/**
 * @struct lh_char_case_pair
 * @typedef lh_char_case_pair_t
 * @brief One entry in a sorted narrow case mapping table (`first` → `second`).
 *
 * Intended for single-byte Latin-1 style mappings where both endpoints are
 * representable as ::lh_char_t (narrow case fold and simple maps).
 *
 * @see lh_pair_fields
 * @see lh_wchar_case_pair_t
 */
typedef struct lh_char_case_pair
{
    lh_pair_fields(lh_char_t, lh_char_t);
} lh_char_case_pair_t;

#endif /* LH_CHAR_CASE_PAIR_H */
