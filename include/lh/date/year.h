/**
 * @file year.h
 * @brief Calendar year value type and wrap add/sub.
 *
 * Closed interval `[0, ::LH_DATE_YEAR_MAX]`. Leap-year test lives here so
 * day/month helpers do not copy the Gregorian rule.
 */

#ifndef LH_DATE_YEAR_H
#define LH_DATE_YEAR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/limits.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>
#include <lh/util/interval.h>

/**
 * @typedef lh_date_year_t
 * @brief Calendar year (e.g. `2026` in `2026/09/16`).
 *
 * Alias for: ::lh_u16_t
 */
typedef lh_u16_t lh_date_year_t;

/**
 * @def LH_DATE_YEAR_MAX
 * @brief Largest valid ::lh_date_year_t (::LH_U16_T_MAX).
 */
#define LH_DATE_YEAR_MAX LH_U16_T_MAX

/**
 * @def LH_DATE_YEAR_RADIX
 * @brief ::lh_interval_closed_get_size of `[0, ::LH_DATE_YEAR_MAX]`.
 */
#define LH_DATE_YEAR_RADIX lh_interval_closed_get_size(0U, LH_DATE_YEAR_MAX)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True if @p self is a Gregorian leap year.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_date_year_is_leap(lh_date_year_t self);

/**
 * @brief Add @p value years, wrapping on `[0, ::LH_DATE_YEAR_MAX]`.
 *
 * @return Whole radix overflows. `0` if it fit.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_year_add(lh_date_year_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value years, wrapping on `[0, ::LH_DATE_YEAR_MAX]`.
 *
 * @return Whole radix units borrowed. `0` if it fit.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_year_sub(lh_date_year_t *self, lh_uint_t value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATE_YEAR_H */
