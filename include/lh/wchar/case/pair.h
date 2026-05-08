/**
 * @file pair.h
 * @brief UCS-4 key/value pair for Unicode wchar case tables
 *        (fold and simple lower/upper maps).
 *
 * Table rows are sorted by `first` (the source scalar)
 * for use with ::lh_interval_ropen_binary_search();
 * `second` holds the mapped code point.
 *
 * Both members use ::lh_uchar32_t via ::lh_pair_fields.
 */

#ifndef LH_WCHAR_CASE_PAIR_H
#define LH_WCHAR_CASE_PAIR_H

#include <lh/uchar.h>
#include <lh/pair/fields.h>

/**
 * @struct lh_wchar_case_pair
 * @typedef lh_wchar_case_pair_t
 * @brief One entry in a sorted UCS-4 case mapping table (`first` → `second`).
 *
 * Shared by case folding (::lh_wchar_fold_case)
 * and simple case conversion (::lh_wchar_to_lower / ::lh_wchar_to_upper).
 *
 * @see lh_pair_fields
 */
typedef struct lh_wchar_case_pair
{
    lh_pair_fields(lh_uchar32_t, lh_uchar32_t);
} lh_wchar_case_pair_t;

#endif // LH_WCHAR_CASE_PAIR_H
