/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_wchar_case_pair_t.
 */

#ifndef LH_WCHAR_CASE_PAIR_INITIALIZER_H
#define LH_WCHAR_CASE_PAIR_INITIALIZER_H

#include <lh/pair/initializer.h>

/**
 * @def lh_wchar_case_pair_initializer(first, second)
 * @brief Produces a brace-enclosed initializer for ::lh_wchar_case_pair_t.
 *
 * Expands to ::lh_pair_initializer(@p first, @p second), the field order of
 * ::lh_pair_fields.
 *
 * @param first  Source code unit (the key the table is sorted by).
 * @param second Mapped code unit.
 *
 * Example usage:
 * @code{.c}
 * static const lh_wchar_case_pair_t pair = lh_wchar_case_pair_initializer(0x0410u, 0x0430u);
 * @endcode
 *
 * @see lh_pair_initializer
 * @see lh_wchar_case_pair_t
 */
#define lh_wchar_case_pair_initializer(first, second) lh_pair_initializer(first, second)

#endif /* LH_WCHAR_CASE_PAIR_INITIALIZER_H */
