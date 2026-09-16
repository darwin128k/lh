/**
 * @file datetime.h
 * @brief Date and time of day together (::lh_date_t + ::lh_time_t).
 *
 * Pure value — no clock, no OS, no timezone.
 */

#ifndef LH_DATETIME_H
#define LH_DATETIME_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/date.h>
#include <lh/datetime/view/fields.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/time.h>

/**
 * @def LH_DATETIME_TEXT_MAX
 * @brief Longest `YYYY/MM/DD HH:MM:SS` text, excluding NUL (11 + 1 + 8 = 20).
 */
#define LH_DATETIME_TEXT_MAX (LH_DATE_TEXT_MAX + 1U + LH_TIME_TEXT_MAX)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @struct lh_datetime
 * @brief Calendar date plus time of day.
 */
struct lh_datetime
{
    lh_datetime_fields(lh_date_t, lh_time_t);
};

/**
 * @typedef lh_datetime_t
 * @brief Alias for `struct lh_datetime`.
 */
typedef struct lh_datetime lh_datetime_t;

LH_ATTRIBUTE_SYMBOL
void
lh_datetime_pack(lh_datetime_t *self, const lh_date_t *date, const lh_time_t *time);

LH_ATTRIBUTE_SYMBOL
void
lh_datetime_unpack(const lh_datetime_t *self, lh_date_t *date, lh_time_t *time);

LH_ATTRIBUTE_SYMBOL
void
lh_datetime_assign(lh_datetime_t *self, const lh_datetime_t *other);

LH_ATTRIBUTE_SYMBOL
void
lh_datetime_set(lh_datetime_t *self, const lh_date_t *date, const lh_time_t *time);

LH_ATTRIBUTE_SYMBOL
lh_date_t
lh_datetime_get_date(const lh_datetime_t *self);

LH_ATTRIBUTE_SYMBOL
lh_time_t
lh_datetime_get_time(const lh_datetime_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_datetime_equals(const lh_datetime_t *self, const lh_datetime_t *other);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_datetime_is_at_least(const lh_datetime_t *self, const lh_datetime_t *minimum);

/**
 * @brief Parse `Y/M/D H:M:S` (one space between date and time).
 *
 * All of @p str_size must be consumed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_datetime_parse(lh_str_cptr str, lh_usize_t str_size, lh_datetime_t *out);

/**
 * @brief Format @p self as `YYYY/MM/DD HH:MM:SS`.
 *
 * No NUL terminator. Returns 0 if @p str_size is too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_datetime_format(const lh_datetime_t *self, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATETIME_H */
