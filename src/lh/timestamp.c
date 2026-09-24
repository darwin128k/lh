#include <lh/timestamp.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/date.h>
#include <lh/datetime.h>
#include <lh/time.h>
#include <lh/util/addr.h>

lh_s64_t
lh_timestamp_floor_div(lh_s64_t a, lh_s64_t b)
{
    lh_s64_t q = a / b;
    return (a % b != 0 && (a % b < 0) != (b < 0)) ? q - 1 : q;
}

lh_s64_t
lh_timestamp_floor_mod(lh_s64_t a, lh_s64_t b)
{
    return a - b * lh_timestamp_floor_div(a, b);
}

lh_timestamp_t
lh_timestamp_from_date(const lh_date_t *self)
{
    return lh_date_days_since_epoch(self) * LH_TIMESTAMP_SECONDS_PER_DAY;
}

lh_timestamp_t
lh_timestamp_from_time(const lh_time_t *self)
{
    return lh_cast_static(lh_timestamp_t, lh_time_seconds_of_day(self));
}

lh_timestamp_t
lh_timestamp_from_datetime(const lh_datetime_t *self)
{
    lh_date_t date;
    lh_time_t time;

    lh_assert_runtime_ref(self);
    date = lh_datetime_get_date(self);
    time = lh_datetime_get_time(self);
    return lh_timestamp_from_date(lh_addr_of(date)) + lh_timestamp_from_time(lh_addr_of(time));
}

lh_s64_t
lh_timestamp_get_days(lh_timestamp_t self)
{
    return lh_timestamp_floor_div(self, LH_TIMESTAMP_SECONDS_PER_DAY);
}

lh_s64_t
lh_timestamp_get_seconds_of_day(lh_timestamp_t self)
{
    return lh_timestamp_floor_mod(self, LH_TIMESTAMP_SECONDS_PER_DAY);
}

lh_date_t
lh_timestamp_get_date(lh_timestamp_t self)
{
    return lh_date_from_epoch_days(lh_timestamp_get_days(self));
}

lh_time_t
lh_timestamp_get_time(lh_timestamp_t self)
{
    return lh_time_from_seconds_of_day(lh_cast_static(lh_uint_t, lh_timestamp_get_seconds_of_day(self)));
}

void
lh_timestamp_to_datetime(lh_timestamp_t self, lh_datetime_t *out)
{
    lh_date_t date = lh_timestamp_get_date(self);
    lh_time_t time = lh_timestamp_get_time(self);

    lh_assert_runtime_ref(out);
    lh_datetime_set(out, lh_addr_of(date), lh_addr_of(time));
}

lh_date_year_t
lh_timestamp_get_year(lh_timestamp_t self)
{
    lh_date_t date = lh_timestamp_get_date(self);
    return lh_date_get_year(lh_addr_of(date));
}

lh_date_month_t
lh_timestamp_get_month(lh_timestamp_t self)
{
    lh_date_t date = lh_timestamp_get_date(self);
    return lh_date_get_month(lh_addr_of(date));
}

lh_date_day_t
lh_timestamp_get_day(lh_timestamp_t self)
{
    lh_date_t date = lh_timestamp_get_date(self);
    return lh_date_get_day(lh_addr_of(date));
}

lh_time_hour_t
lh_timestamp_get_hour(lh_timestamp_t self)
{
    lh_time_t time = lh_timestamp_get_time(self);
    return lh_time_get_hour(lh_addr_of(time));
}

lh_time_minute_t
lh_timestamp_get_minute(lh_timestamp_t self)
{
    lh_time_t time = lh_timestamp_get_time(self);
    return lh_time_get_minute(lh_addr_of(time));
}

lh_time_second_t
lh_timestamp_get_second(lh_timestamp_t self)
{
    lh_time_t time = lh_timestamp_get_time(self);
    return lh_time_get_second(lh_addr_of(time));
}

lh_timestamp_t
lh_timestamp_add_seconds(lh_timestamp_t self, lh_timestamp_t seconds)
{
    return self + seconds;
}

lh_timestamp_t
lh_timestamp_sub_seconds(lh_timestamp_t self, lh_timestamp_t seconds)
{
    return self - seconds;
}

lh_timestamp_t
lh_timestamp_add_days(lh_timestamp_t self, lh_s64_t days)
{
    return lh_timestamp_add_seconds(self, days * LH_TIMESTAMP_SECONDS_PER_DAY);
}

lh_timestamp_t
lh_timestamp_sub_days(lh_timestamp_t self, lh_s64_t days)
{
    return lh_timestamp_sub_seconds(self, days * LH_TIMESTAMP_SECONDS_PER_DAY);
}

lh_timestamp_t
lh_timestamp_add_months(lh_timestamp_t self, lh_uint_t months)
{
    lh_date_t date = lh_timestamp_get_date(self);
    lh_s64_t seconds_of_day = lh_timestamp_get_seconds_of_day(self);

    lh_date_add_month(lh_addr_of(date), months);
    return lh_timestamp_from_date(lh_addr_of(date)) + seconds_of_day;
}

lh_timestamp_t
lh_timestamp_sub_months(lh_timestamp_t self, lh_uint_t months)
{
    lh_date_t date = lh_timestamp_get_date(self);
    lh_s64_t seconds_of_day = lh_timestamp_get_seconds_of_day(self);

    lh_date_sub_month(lh_addr_of(date), months);
    return lh_timestamp_from_date(lh_addr_of(date)) + seconds_of_day;
}

lh_timestamp_t
lh_timestamp_add_years(lh_timestamp_t self, lh_uint_t years)
{
    lh_date_t date = lh_timestamp_get_date(self);
    lh_s64_t seconds_of_day = lh_timestamp_get_seconds_of_day(self);

    lh_date_add_year(lh_addr_of(date), years);
    return lh_timestamp_from_date(lh_addr_of(date)) + seconds_of_day;
}

lh_timestamp_t
lh_timestamp_sub_years(lh_timestamp_t self, lh_uint_t years)
{
    lh_date_t date = lh_timestamp_get_date(self);
    lh_s64_t seconds_of_day = lh_timestamp_get_seconds_of_day(self);

    lh_date_sub_year(lh_addr_of(date), years);
    return lh_timestamp_from_date(lh_addr_of(date)) + seconds_of_day;
}

lh_timestamp_t
lh_timestamp_diff(lh_timestamp_t self, lh_timestamp_t other)
{
    return self - other;
}

lh_bool_t
lh_timestamp_equals(lh_timestamp_t self, lh_timestamp_t other)
{
    return (self == other) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_timestamp_is_at_least(lh_timestamp_t self, lh_timestamp_t minimum)
{
    return (self >= minimum) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_timestamp_is_less(lh_timestamp_t self, lh_timestamp_t other)
{
    return lh_cast_static(lh_bool_t, !lh_timestamp_is_at_least(self, other));
}

lh_bool_t
lh_timestamp_is_greater(lh_timestamp_t self, lh_timestamp_t other)
{
    return lh_timestamp_is_less(other, self);
}
