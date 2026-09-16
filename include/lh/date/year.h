/**
 * @file year.h
 * @brief Type definition for the calendar year.
 */

#ifndef LH_DATE_YEAR_H
#define LH_DATE_YEAR_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_date_year_t
 * @brief Calendar year (e.g. `2026` in `2026/09/16`).
 *
 * Range: 0 to 65535.
 *
 * Alias for: ::lh_u16_t
 */
typedef lh_u16_t lh_date_year_t;

#endif /* LH_DATE_YEAR_H */
