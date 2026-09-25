/**
 * @file year.h
 * @brief Calendar year value type and wrap add/sub.
 *
 * Closed interval `[0, ::LH_DATE_YEAR_MAX]`. Leap-year test lives here so
 * day/month helpers do not copy the Gregorian rule.
 *
 * There is no separate init: the type is ::lh_u16_t. Wrap add/sub take
 * ::lh_uint_t so the amount can be larger than the year radix.
 */

#ifndef LH_DATE_YEAR_H
#define LH_DATE_YEAR_H

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
 * @brief Years in the closed interval — ::lh_interval_closed_get_size of
 *        `[0, ::LH_DATE_YEAR_MAX]`.
 */
#define LH_DATE_YEAR_RADIX lh_interval_closed_get_size(0U, LH_DATE_YEAR_MAX)

/**
 * @def LH_DATE_YEAR_LEAP_CYCLE
 * @brief Common leap-year stride (divisible by 4).
 */
#define LH_DATE_YEAR_LEAP_CYCLE 4U

/**
 * @def LH_DATE_YEAR_CENTURY
 * @brief Century year (not a leap year unless also ::LH_DATE_YEAR_GREGORIAN_CYCLE).
 */
#define LH_DATE_YEAR_CENTURY 100U

/**
 * @def LH_DATE_YEAR_GREGORIAN_CYCLE
 * @brief 400-year Gregorian leap-year exception for centuries.
 */
#define LH_DATE_YEAR_GREGORIAN_CYCLE 400U

/**
 * @def LH_DATE_YEAR_DAYS_COMMON
 * @brief Days in a non-leap year. A leap year has one more (::lh_date_year_is_leap).
 */
#define LH_DATE_YEAR_DAYS_COMMON 365U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True if @p self is a Gregorian leap year.
 *
 * Rule: divisible by ::LH_DATE_YEAR_LEAP_CYCLE, except centuries not divisible
 * by ::LH_DATE_YEAR_GREGORIAN_CYCLE.
 *
 * @param self Year value (by value, not a pointer).
 * @return ::lh_bool_true if February has 29 days in @p self.
 */
lh_bool_t
lh_date_year_is_leap(lh_date_year_t self);

/**
 * @brief Days in @p self (365, or 366 when ::lh_date_year_is_leap).
 *
 * @param self Year value (by value, not a pointer).
 */
lh_u16_t
lh_date_year_days(lh_date_year_t self);

/**
 * @brief Add @p value years, wrapping on `[0, ::LH_DATE_YEAR_MAX]`.
 *
 * @param self  Year to update (not null).
 * @param value Years to add (any ::lh_uint_t).
 *
 * @return Whole radix overflows (`total / radix` beyond the interval). `0` if it fit.
 */
lh_uint_t
lh_date_year_add(lh_date_year_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value years, wrapping on `[0, ::LH_DATE_YEAR_MAX]`.
 *
 * @param self  Year to update (not null).
 * @param value Years to subtract (any ::lh_uint_t).
 *
 * @return Whole radix units borrowed. `0` if it fit.
 */
lh_uint_t
lh_date_year_sub(lh_date_year_t *self, lh_uint_t value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATE_YEAR_H */
