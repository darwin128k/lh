/**
 * @file month.h
 * @brief Calendar month (`1`–`12`) and conversion to ::lh_date_month_index_t.
 *
 * Add/sub wrap through the 0-based index, then convert back.
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

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Convert calendar month `1…12` to index `0…11`.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_month_index_t
lh_date_month_to_index(lh_date_month_t month);

/**
 * @brief Convert index `0…11` to calendar month `1…12`.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_month_t
lh_date_month_from_index(lh_date_month_index_t index);

/**
 * @brief Add @p value months to @p self (calendar `1…12`), wrapping via index.
 *
 * @return Whole years of overflow.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_month_add(lh_date_month_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value months from @p self, wrapping via index.
 *
 * @return Whole years borrowed.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_month_sub(lh_date_month_t *self, lh_uint_t value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATE_MONTH_H */
