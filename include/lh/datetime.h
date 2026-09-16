/**
 * @file datetime.h
 * @brief Date and time of day together (::lh_date_t + ::lh_time_t).
 *
 * Pure value — no clock, no OS, no timezone.
 *
 * Brace init: ::lh_datetime_initializer in `lh/datetime/initializer.h`.
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
 *
 * Fields are injected via ::lh_datetime_fields.
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

/**
 * @brief Write date and/or time into a datetime struct.
 *
 * Each pointer is optional: pass ::lh_null to leave that field unchanged.
 *
 * @param self Date-time to modify (not null).
 * @param date New date, or ::lh_null to skip.
 * @param time New time, or ::lh_null to skip.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_datetime_pack(lh_datetime_t *self, const lh_date_t *date, const lh_time_t *time);

/**
 * @brief Read date and/or time out of a datetime struct.
 *
 * Each pointer is optional: pass ::lh_null to skip that field.
 *
 * @param self Date-time to read (not null).
 * @param date Output for date, or ::lh_null to skip.
 * @param time Output for time, or ::lh_null to skip.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_datetime_unpack(const lh_datetime_t *self, lh_date_t *date, lh_time_t *time);

/**
 * @brief Copy @p other into @p self.
 *
 * @param self  Destination (not null).
 * @param other Source (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_datetime_assign(lh_datetime_t *self, const lh_datetime_t *other);

/**
 * @brief Replace @p self with @p date and @p time.
 *
 * Equivalent to ::lh_datetime_pack with both pointers provided.
 *
 * @param self Date-time to modify (not null).
 * @param date Date (not null).
 * @param time Time (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_datetime_set(lh_datetime_t *self, const lh_date_t *date, const lh_time_t *time);

/**
 * @brief Return the date of @p self.
 *
 * @param self Date-time to read (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_date_t
lh_datetime_get_date(const lh_datetime_t *self);

/**
 * @brief Return the time of @p self.
 *
 * @param self Date-time to read (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_time_t
lh_datetime_get_time(const lh_datetime_t *self);

/**
 * @brief True if @p self and @p other hold the same date and time.
 *
 * @param self  Left (not null).
 * @param other Right (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_datetime_equals(const lh_datetime_t *self, const lh_datetime_t *other);

/**
 * @brief True if @p self is not earlier than @p minimum.
 *
 * Order: date, then time.
 *
 * @param self    Value under test (not null).
 * @param minimum Floor (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_datetime_is_at_least(const lh_datetime_t *self, const lh_datetime_t *minimum);

/**
 * @brief Parse `Y/M/D H:M:S` (one space between date and time).
 *
 * All of @p str_size must be consumed. Date and time rules are those of
 * ::lh_date_parse and ::lh_time_parse.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param out      Receives the parsed value on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a valid datetime.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_datetime_parse(lh_str_cptr str, lh_usize_t str_size, lh_datetime_t *out);

/**
 * @brief Format @p self as `YYYY/MM/DD HH:MM:SS`.
 *
 * No NUL terminator. A buffer of ::LH_DATETIME_TEXT_MAX + 1 always has room
 * to add one after the returned length.
 *
 * @param self     Date-time to format (not null).
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written, or 0 if @p str_size was too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_datetime_format(const lh_datetime_t *self, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATETIME_H */
