/**
 * @file month.h
 * @brief Calendar month (`1`–`12`) and conversion to ::lh_date_month_index_t.
 *
 * Add/sub wrap through the 0-based index, then convert back.
 * Out-of-range stored months (`0` or `> 12`) make add/sub a no-op (return `0`).
 */

#ifndef LH_DATE_MONTH_H
#define LH_DATE_MONTH_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/date/month/index.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>

/**
 * @typedef lh_date_month_t
 * @brief Calendar month (`1` = January … `12` = December).
 *
 * Alias for: ::lh_u8_t
 */
typedef lh_u8_t lh_date_month_t;

/**
 * @def LH_DATE_MONTH_MIN
 * @brief January.
 */
#define LH_DATE_MONTH_MIN 1U

/**
 * @def LH_DATE_MONTH_MAX
 * @brief December.
 */
#define LH_DATE_MONTH_MAX 12U

/**
 * @def LH_DATE_MONTHS_PER_YEAR
 * @brief Months in a year.
 */
#define LH_DATE_MONTHS_PER_YEAR 12U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Convert calendar month `1…12` to index `0…11`.
 *
 * Does not range-check. Month `0` underflows the unsigned result.
 *
 * @param month Calendar month (::LH_DATE_MONTH_MIN–::LH_DATE_MONTH_MAX).
 * @return `month - ::LH_DATE_MONTH_MIN`.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_month_index_t
lh_date_month_to_index(lh_date_month_t month);

/**
 * @brief Convert index `0…11` to calendar month `1…12`.
 *
 * @param index Month index (::LH_DATE_MONTH_INDEX_JANUARY–::LH_DATE_MONTH_INDEX_MAX).
 * @return `index + ::LH_DATE_MONTH_MIN`.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_month_t
lh_date_month_from_index(lh_date_month_index_t index);

/**
 * @brief Add @p value months to @p self (calendar `1…12`), wrapping via
 *        ::lh_date_month_index_add.
 *
 * If @p self is not in `[::LH_DATE_MONTH_MIN, ::LH_DATE_MONTH_MAX]`, returns `0`
 * and leaves @p self unchanged.
 *
 * @param self  Month to update (not null).
 * @param value Months to add (any ::lh_uint_t).
 *
 * @return Whole years of overflow. `0` if it fit (or the month was invalid).
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_month_add(lh_date_month_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value months from @p self, wrapping via
 *        ::lh_date_month_index_sub.
 *
 * Invalid stored month: same no-op as ::lh_date_month_add.
 *
 * @param self  Month to update (not null).
 * @param value Months to subtract (any ::lh_uint_t).
 *
 * @return Whole years borrowed. `0` if it fit (or the month was invalid).
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_month_sub(lh_date_month_t *self, lh_uint_t value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATE_MONTH_H */
