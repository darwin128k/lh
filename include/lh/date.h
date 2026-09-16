/**
 * @file date.h
 * @brief Calendar date value type (`year/month/day`).
 *
 * Pure value — no clock, no OS. Gregorian calendar on parse.
 */

#ifndef LH_DATE_H
#define LH_DATE_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/date/day.h>
#include <lh/date/month.h>
#include <lh/date/view/fields.h>
#include <lh/date/year.h>
#include <lh/numeric/fixed/limits.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @def LH_DATE_YEAR_MAX
 * @brief Largest valid ::lh_date_year_t.
 *
 * Expands to ::LH_U16_T_MAX.
 */
#define LH_DATE_YEAR_MAX LH_U16_T_MAX

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
 * @def LH_DATE_DAY_MIN
 * @brief First day of a month.
 */
#define LH_DATE_DAY_MIN 1U

/**
 * @def LH_DATE_TEXT_MAX
 * @brief Longest `YYYY/MM/DD` text, excluding NUL (`65535/12/31` is 11).
 */
#define LH_DATE_TEXT_MAX 11U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @struct lh_date
 * @brief Gregorian calendar date.
 */
struct lh_date
{
    lh_date_fields(lh_date_year_t, lh_date_month_t, lh_date_day_t);
};

/**
 * @typedef lh_date_t
 * @brief Alias for `struct lh_date`.
 */
typedef struct lh_date lh_date_t;

LH_ATTRIBUTE_SYMBOL
void
lh_date_pack(lh_date_t *self, const lh_date_year_t *year, const lh_date_month_t *month,
             const lh_date_day_t *day);

LH_ATTRIBUTE_SYMBOL
void
lh_date_unpack(const lh_date_t *self, lh_date_year_t *year, lh_date_month_t *month,
               lh_date_day_t *day);

LH_ATTRIBUTE_SYMBOL
void
lh_date_assign(lh_date_t *self, const lh_date_t *other);

LH_ATTRIBUTE_SYMBOL
void
lh_date_set(lh_date_t *self, lh_date_year_t year, lh_date_month_t month, lh_date_day_t day);

LH_ATTRIBUTE_SYMBOL
lh_date_year_t
lh_date_get_year(const lh_date_t *self);

LH_ATTRIBUTE_SYMBOL
lh_date_month_t
lh_date_get_month(const lh_date_t *self);

LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_get_day(const lh_date_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_date_equals(const lh_date_t *self, const lh_date_t *other);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_date_is_at_least(const lh_date_t *self, const lh_date_t *minimum);

/**
 * @brief Parse `Y/M/D` (slashes; leading zeros on month/day allowed).
 *
 * All of @p str_size must be consumed. Rejects impossible calendar days.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_date_parse(lh_str_cptr str, lh_usize_t str_size, lh_date_t *out);

/**
 * @brief Format @p self as `YYYY/MM/DD` (month and day zero-padded).
 *
 * No NUL terminator. Returns 0 if @p str_size is too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_date_format(const lh_date_t *self, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATE_H */
