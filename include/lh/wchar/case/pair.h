/**
 * @file pair.h
 * @brief Wide key/value pair for Unicode wchar case tables
 *        (fold and simple lower/upper maps).
 *
 * Table rows are sorted by `first` (the source scalar)
 * for use with ::lh_interval_ropen_binary_search();
 * `second` holds the mapped code point.
 *
 * Both members use ::lh_wchar_t via ::lh_pair_fields. Supplementary-plane rows
 * (above 0xFFFF) exist only where ::LH_WCHAR_T_MAX can hold them; no case
 * mapping crosses the BMP boundary, so a 16-bit ::lh_wchar_t loses nothing.
 */

#ifndef LH_WCHAR_CASE_PAIR_H
#define LH_WCHAR_CASE_PAIR_H

#include <lh/wchar.h>
#include <lh/pair/fields.h>

/**
 * @struct lh_wchar_case_pair
 * @brief One entry in a sorted UCS-4 case mapping table (`first` → `second`).
 *
 * Shared by case folding (::lh_wchar_fold_case)
 * and simple case conversion (::lh_wchar_to_lower / ::lh_wchar_to_upper).
 *
 * @see lh_pair_fields
 */
struct lh_wchar_case_pair
{
    /** Members: ::lh_pair_fields. */
    lh_pair_fields(lh_wchar_t, lh_wchar_t);
};
/** @copybrief lh_wchar_case_pair */
typedef struct lh_wchar_case_pair lh_wchar_case_pair_t;

#endif /* LH_WCHAR_CASE_PAIR_H */
