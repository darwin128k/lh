/**
 * @file index.h
 * @brief Zero-based month index (`0` = January … `11` = December).
 *
 * Not a calendar ::lh_date_month_t. Wrap on this type, then convert with
 * ::lh_date_month_from_index. ::lh_date_month_add delegates here.
 */

#ifndef LH_DATE_MONTH_INDEX_H
#define LH_DATE_MONTH_INDEX_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>
#include <lh/util/interval.h>

/**
 * @typedef lh_date_month_index_t
 * @brief Month index in `[0, ::LH_DATE_MONTH_INDEX_MAX]`.
 *
 * Alias for: ::lh_u8_t
 */
typedef lh_u8_t lh_date_month_index_t;

/**
 * @def LH_DATE_MONTH_INDEX_JANUARY
 */
#define LH_DATE_MONTH_INDEX_JANUARY 0U
#define LH_DATE_MONTH_INDEX_FEBRUARY 1U
#define LH_DATE_MONTH_INDEX_DECEMBER 11U

/**
 * @def LH_DATE_MONTH_INDEX_MAX
 * @brief Last month index (closed upper bound).
 */
#define LH_DATE_MONTH_INDEX_MAX LH_DATE_MONTH_INDEX_DECEMBER

/**
 * @def LH_DATE_MONTH_INDEX_RADIX
 * @brief Months per year — closed size of `[0, ::LH_DATE_MONTH_INDEX_MAX]`.
 */
#define LH_DATE_MONTH_INDEX_RADIX lh_interval_closed_get_size(0U, LH_DATE_MONTH_INDEX_MAX)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Add @p value to @p self, wrapping on the month-index interval.
 *
 * @return Whole years of overflow. `0` if it fit.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_month_index_add(lh_date_month_index_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value from @p self, wrapping on the month-index interval.
 *
 * @return Whole years borrowed. `0` if it fit.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_month_index_sub(lh_date_month_index_t *self, lh_uint_t value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATE_MONTH_INDEX_H */
