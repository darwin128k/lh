#include <lh/date.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/numeric/types.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/uint.h>
#include <lh/util/addr.h>

#define LH_DATE_EPOCH_YEAR 1970U

/**
 * Floor division: rounds toward negative infinity, unlike `/`. Local to this
 * file so date.c has no dependency on lh/timestamp.h (which is the derived
 * layer, not the other way around).
 */
static lh_s64_t
lh_date_epoch_floor_div(lh_s64_t a, lh_s64_t b)
{
    lh_s64_t q = a / b;
    return (a % b != 0 && (a % b < 0) != (b < 0)) ? q - 1 : q;
}

/**
 * Days from proleptic year 0 (Jan 1, itself a leap year) to @p year (Jan 1).
 * Closed-form Gregorian leap-year count - O(1), no loop.
 */
static lh_s64_t
lh_date_days_from_year_zero(lh_date_year_t year)
{
    lh_s64_t y = lh_cast_static(lh_s64_t, year) - 1;
    lh_s64_t leap_count = lh_date_epoch_floor_div(y, 4) - lh_date_epoch_floor_div(y, 100) +
                         lh_date_epoch_floor_div(y, 400) + 1;

    return lh_cast_static(lh_s64_t, LH_DATE_YEAR_DAYS_COMMON) * lh_cast_static(lh_s64_t, year) + leap_count;
}

void
lh_date_set_year(lh_date_t *self, lh_date_year_t year)
{
    lh_assert_runtime_ref(self);
    self->year = year;
}

void
lh_date_set_month(lh_date_t *self, lh_date_month_t month)
{
    lh_assert_runtime_ref(self);
    self->month = month;
}

void
lh_date_set_day(lh_date_t *self, lh_date_day_t day)
{
    lh_assert_runtime_ref(self);
    self->day = day;
}

lh_bool_t
lh_date_ymd_is_valid(lh_uint_t year, lh_uint_t month, lh_uint_t day)
{
    if (month < LH_DATE_MONTH_MIN || day < LH_DATE_DAY_MIN)
    {
        return lh_bool_false;
    }

    return lh_cast_static(lh_bool_t, day <= lh_date_days_in_month(lh_cast_static(lh_date_year_t, year),
                                                                  lh_cast_static(lh_date_month_t, month)));
}

void
lh_date_set(lh_date_t *self, lh_date_year_t year, lh_date_month_t month, lh_date_day_t day)
{
    lh_date_set_year(self, year);
    lh_date_set_month(self, month);
    lh_date_set_day(self, day);
}

void
lh_date_assign(lh_date_t *self, const lh_date_t *other)
{
    lh_date_set(self, lh_date_get_year(other), lh_date_get_month(other), lh_date_get_day(other));
}

lh_date_day_t
lh_date_max_days(const lh_date_t *self)
{
    return lh_date_days_in_month(lh_date_get_year(self), lh_date_get_month(self));
}

lh_s64_t
lh_date_days_since_epoch(const lh_date_t *self)
{
    lh_date_year_t year;
    lh_date_month_t month;
    lh_date_month_t m;
    lh_s64_t days;

    year = lh_date_get_year(self);
    month = lh_date_get_month(self);
    days = lh_date_days_from_year_zero(year) - lh_date_days_from_year_zero(LH_DATE_EPOCH_YEAR);
    for (m = 1U; m < month; ++m)
    {
        days += lh_date_days_in_month(year, m);
    }
    days += lh_cast_static(lh_s64_t, lh_date_get_day(self)) - 1;
    return days;
}

lh_date_t
lh_date_from_epoch_days(lh_s64_t days)
{
    lh_date_year_t year = LH_DATE_EPOCH_YEAR;
    lh_date_month_t month;
    lh_date_t date;

    while (days < 0)
    {
        --year;
        days += lh_date_year_days(year);
    }
    while (days >= lh_date_year_days(year))
    {
        days -= lh_date_year_days(year);
        ++year;
    }

    month = 1U;
    while (days >= lh_date_days_in_month(year, month))
    {
        days -= lh_date_days_in_month(year, month);
        ++month;
    }

    lh_date_set(lh_addr_of(date), year, month, lh_cast_static(lh_date_day_t, days + 1));
    return date;
}

lh_uint_t
lh_date_add_month(lh_date_t *self, lh_uint_t value)
{
    lh_date_month_t month;
    lh_date_year_t year;
    lh_uint_t overflow;
    lh_date_day_t max_day;

    month = lh_date_get_month(self);
    year = lh_date_get_year(self);
    overflow = lh_date_year_add(lh_addr_of(year), lh_date_month_add(lh_addr_of(month), value));
    max_day = lh_date_days_in_month(year, month);
    lh_date_set(self, year, month, (lh_date_get_day(self) > max_day) ? max_day : lh_date_get_day(self));
    return overflow;
}

lh_uint_t
lh_date_sub_month(lh_date_t *self, lh_uint_t value)
{
    lh_date_month_t month;
    lh_date_year_t year;
    lh_uint_t overflow;
    lh_date_day_t max_day;

    month = lh_date_get_month(self);
    year = lh_date_get_year(self);
    overflow = lh_date_year_sub(lh_addr_of(year), lh_date_month_sub(lh_addr_of(month), value));
    max_day = lh_date_days_in_month(year, month);
    lh_date_set(self, year, month, (lh_date_get_day(self) > max_day) ? max_day : lh_date_get_day(self));
    return overflow;
}

lh_uint_t
lh_date_add_year(lh_date_t *self, lh_uint_t value)
{
    return lh_date_add_month(self, value * LH_DATE_MONTHS_PER_YEAR);
}

lh_uint_t
lh_date_sub_year(lh_date_t *self, lh_uint_t value)
{
    return lh_date_sub_month(self, value * LH_DATE_MONTHS_PER_YEAR);
}

lh_uint_t
lh_date_add_day(lh_date_t *self, lh_uint_t value)
{
    lh_date_year_t before = lh_date_get_year(self);
    lh_date_t result = lh_date_from_epoch_days(lh_date_days_since_epoch(self) + lh_cast_static(lh_s64_t, value));

    lh_date_assign(self, lh_addr_of(result));
    return lh_cast_static(lh_uint_t, lh_date_get_year(self) < before);
}

lh_uint_t
lh_date_sub_day(lh_date_t *self, lh_uint_t value)
{
    lh_date_year_t before = lh_date_get_year(self);
    lh_date_t result = lh_date_from_epoch_days(lh_date_days_since_epoch(self) - lh_cast_static(lh_s64_t, value));

    lh_date_assign(self, lh_addr_of(result));
    return lh_cast_static(lh_uint_t, lh_date_get_year(self) > before);
}

lh_uint_t
lh_date_add_custom(lh_date_t *self, lh_uint_t year, lh_uint_t month, lh_uint_t day)
{
    lh_uint_t overflow = lh_date_add_day(self, day);

    overflow += lh_date_add_month(self, month);
    overflow += lh_date_add_year(self, year);
    return overflow;
}

lh_uint_t
lh_date_sub_custom(lh_date_t *self, lh_uint_t year, lh_uint_t month, lh_uint_t day)
{
    lh_uint_t overflow = lh_date_sub_year(self, year);

    overflow += lh_date_sub_month(self, month);
    overflow += lh_date_sub_day(self, day);
    return overflow;
}

lh_uint_t
lh_date_add(lh_date_t *self, const lh_date_t *other)
{
    return lh_date_add_custom(self, lh_cast_static(lh_uint_t, lh_date_get_year(other)),
                              lh_cast_static(lh_uint_t, lh_date_get_month(other)),
                              lh_cast_static(lh_uint_t, lh_date_get_day(other)));
}

lh_uint_t
lh_date_sub(lh_date_t *self, const lh_date_t *other)
{
    return lh_date_sub_custom(self, lh_cast_static(lh_uint_t, lh_date_get_year(other)),
                              lh_cast_static(lh_uint_t, lh_date_get_month(other)),
                              lh_cast_static(lh_uint_t, lh_date_get_day(other)));
}

lh_date_year_t
lh_date_get_year(const lh_date_t *self)
{
    lh_assert_runtime_ref(self);
    return self->year;
}

lh_date_month_t
lh_date_get_month(const lh_date_t *self)
{
    lh_assert_runtime_ref(self);
    return self->month;
}

lh_date_day_t
lh_date_get_day(const lh_date_t *self)
{
    lh_assert_runtime_ref(self);
    return self->day;
}

lh_bool_t
lh_date_equals(const lh_date_t *self, const lh_date_t *other)
{
    return lh_cast_static(lh_bool_t, lh_date_get_year(self) == lh_date_get_year(other) &&
                                     lh_date_get_month(self) == lh_date_get_month(other) &&
                                     lh_date_get_day(self) == lh_date_get_day(other));
}

lh_bool_t
lh_date_is_at_least(const lh_date_t *self, const lh_date_t *minimum)
{
    lh_date_year_t self_year;
    lh_date_year_t minimum_year;
    lh_date_month_t self_month;
    lh_date_month_t minimum_month;

    self_year = lh_date_get_year(self);
    minimum_year = lh_date_get_year(minimum);
    if (self_year != minimum_year)
    {
        return lh_cast_static(lh_bool_t, self_year > minimum_year);
    }
    self_month = lh_date_get_month(self);
    minimum_month = lh_date_get_month(minimum);
    if (self_month != minimum_month)
    {
        return lh_cast_static(lh_bool_t, self_month > minimum_month);
    }
    return lh_cast_static(lh_bool_t, lh_date_get_day(self) >= lh_date_get_day(minimum));
}

lh_bool_t
lh_date_is_less(const lh_date_t *self, const lh_date_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_date_is_at_least(self, other));
}

lh_bool_t
lh_date_is_greater(const lh_date_t *self, const lh_date_t *other)
{
    return lh_date_is_less(other, self);
}

lh_bool_t
lh_date_parse(lh_str_cptr str, lh_usize_t str_size, lh_date_t *out)
{
    lh_uint_t year;
    lh_uint_t month;
    lh_uint_t day;
    lh_usize_t pos = 0;

    if (!lh_str_ptr_split_next_uint_digits(str, str_size, '/', lh_addr_of(pos), LH_DATE_YEAR_MAX,
                                           lh_addr_of(year), lh_bool_true))
    {
        return lh_bool_false;
    }
    if (!lh_str_ptr_split_next_uint_digits(str, str_size, '/', lh_addr_of(pos), LH_DATE_MONTH_MAX,
                                           lh_addr_of(month), lh_bool_true))
    {
        return lh_bool_false;
    }
    if (!lh_str_ptr_split_next_uint_digits(str, str_size, '/', lh_addr_of(pos), LH_DATE_DAY_MAX,
                                           lh_addr_of(day), lh_bool_false))
    {
        return lh_bool_false;
    }
    if (!lh_date_ymd_is_valid(year, month, day))
    {
        return lh_bool_false;
    }

    lh_date_set(out, lh_cast_static(lh_date_year_t, year), lh_cast_static(lh_date_month_t, month),
                lh_cast_static(lh_date_day_t, day));
    return lh_bool_true;
}

lh_usize_t
lh_date_format(const lh_date_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    return lh_str_ptr_format_text(str, str_size, "%u/%02u/%02u",
                                  lh_cast_static(lh_uint_t, lh_date_get_year(self)),
                                  lh_cast_static(lh_uint_t, lh_date_get_month(self)),
                                  lh_cast_static(lh_uint_t, lh_date_get_day(self)));
}
