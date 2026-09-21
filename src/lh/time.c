#include <lh/time.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/numeric/types.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/uint.h>
#include <lh/timestamp.h>
#include <lh/util/addr.h>

void
lh_time_set_hour(lh_time_t *self, lh_time_hour_t hour)
{
    lh_assert_runtime_ref(self);
    self->hour = hour;
}

void
lh_time_set_minute(lh_time_t *self, lh_time_minute_t minute)
{
    lh_assert_runtime_ref(self);
    self->minute = minute;
}

void
lh_time_set_second(lh_time_t *self, lh_time_second_t second)
{
    lh_assert_runtime_ref(self);
    self->second = second;
}

void
lh_time_set(lh_time_t *self, lh_time_hour_t hour, lh_time_minute_t minute, lh_time_second_t second)
{
    lh_time_set_hour(self, hour);
    lh_time_set_minute(self, minute);
    lh_time_set_second(self, second);
}

void
lh_time_assign(lh_time_t *self, const lh_time_t *other)
{
    lh_time_set(self, lh_time_get_hour(other), lh_time_get_minute(other), lh_time_get_second(other));
}

lh_uint_t
lh_time_add_custom(lh_time_t *self, lh_uint_t hour, lh_uint_t minute, lh_uint_t second)
{
    lh_timestamp_t duration = lh_cast_static(lh_timestamp_t, hour) * LH_TIMESTAMP_SECONDS_PER_HOUR +
                              lh_cast_static(lh_timestamp_t, minute) * LH_TIMESTAMP_SECONDS_PER_MINUTE +
                              lh_cast_static(lh_timestamp_t, second);
    lh_timestamp_t total = lh_timestamp_from_time(self) + duration;
    lh_time_t result = lh_timestamp_get_time(total);

    lh_time_assign(self, lh_addr_of(result));
    return lh_cast_static(lh_uint_t, lh_timestamp_floor_div(total, LH_TIMESTAMP_SECONDS_PER_DAY));
}

lh_uint_t
lh_time_sub_custom(lh_time_t *self, lh_uint_t hour, lh_uint_t minute, lh_uint_t second)
{
    lh_timestamp_t duration = lh_cast_static(lh_timestamp_t, hour) * LH_TIMESTAMP_SECONDS_PER_HOUR +
                              lh_cast_static(lh_timestamp_t, minute) * LH_TIMESTAMP_SECONDS_PER_MINUTE +
                              lh_cast_static(lh_timestamp_t, second);
    lh_timestamp_t total = lh_timestamp_from_time(self) - duration;
    lh_time_t result = lh_timestamp_get_time(total);

    lh_time_assign(self, lh_addr_of(result));
    return lh_cast_static(lh_uint_t, -lh_timestamp_floor_div(total, LH_TIMESTAMP_SECONDS_PER_DAY));
}

lh_uint_t
lh_time_add_hour(lh_time_t *self, lh_uint_t value)
{
    return lh_time_add_custom(self, value, 0U, 0U);
}

lh_uint_t
lh_time_sub_hour(lh_time_t *self, lh_uint_t value)
{
    return lh_time_sub_custom(self, value, 0U, 0U);
}

lh_uint_t
lh_time_add_minute(lh_time_t *self, lh_uint_t value)
{
    return lh_time_add_custom(self, 0U, value, 0U);
}

lh_uint_t
lh_time_sub_minute(lh_time_t *self, lh_uint_t value)
{
    return lh_time_sub_custom(self, 0U, value, 0U);
}

lh_uint_t
lh_time_add_second(lh_time_t *self, lh_uint_t value)
{
    return lh_time_add_custom(self, 0U, 0U, value);
}

lh_uint_t
lh_time_sub_second(lh_time_t *self, lh_uint_t value)
{
    return lh_time_sub_custom(self, 0U, 0U, value);
}

lh_uint_t
lh_time_add(lh_time_t *self, const lh_time_t *other)
{
    return lh_time_add_custom(self, lh_cast_static(lh_uint_t, lh_time_get_hour(other)),
                              lh_cast_static(lh_uint_t, lh_time_get_minute(other)),
                              lh_cast_static(lh_uint_t, lh_time_get_second(other)));
}

lh_uint_t
lh_time_sub(lh_time_t *self, const lh_time_t *other)
{
    return lh_time_sub_custom(self, lh_cast_static(lh_uint_t, lh_time_get_hour(other)),
                              lh_cast_static(lh_uint_t, lh_time_get_minute(other)),
                              lh_cast_static(lh_uint_t, lh_time_get_second(other)));
}

lh_time_hour_t
lh_time_get_hour(const lh_time_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hour;
}

lh_time_minute_t
lh_time_get_minute(const lh_time_t *self)
{
    lh_assert_runtime_ref(self);
    return self->minute;
}

lh_time_second_t
lh_time_get_second(const lh_time_t *self)
{
    lh_assert_runtime_ref(self);
    return self->second;
}

lh_bool_t
lh_time_equals(const lh_time_t *self, const lh_time_t *other)
{
    return lh_timestamp_equals(lh_timestamp_from_time(self), lh_timestamp_from_time(other));
}

lh_bool_t
lh_time_is_at_least(const lh_time_t *self, const lh_time_t *minimum)
{
    return lh_timestamp_is_at_least(lh_timestamp_from_time(self), lh_timestamp_from_time(minimum));
}

lh_bool_t
lh_time_is_less(const lh_time_t *self, const lh_time_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_time_is_at_least(self, other));
}

lh_bool_t
lh_time_is_greater(const lh_time_t *self, const lh_time_t *other)
{
    return lh_time_is_less(other, self);
}

lh_bool_t
lh_time_parse(lh_str_cptr str, lh_usize_t str_size, lh_time_t *out)
{
    lh_uint_t hour;
    lh_uint_t minute;
    lh_uint_t second;
    lh_usize_t pos = 0;

    if (!lh_str_ptr_split_next_uint_digits(str, str_size, ':', lh_addr_of(pos), LH_TIME_HOUR_MAX,
                                           lh_addr_of(hour), lh_bool_true))
    {
        return lh_bool_false;
    }
    if (!lh_str_ptr_split_next_uint_digits(str, str_size, ':', lh_addr_of(pos), LH_TIME_MINUTE_MAX,
                                           lh_addr_of(minute), lh_bool_true))
    {
        return lh_bool_false;
    }
    if (!lh_str_ptr_split_next_uint_digits(str, str_size, ':', lh_addr_of(pos), LH_TIME_SECOND_MAX,
                                           lh_addr_of(second), lh_bool_false))
    {
        return lh_bool_false;
    }

    lh_time_set(out, lh_cast_static(lh_time_hour_t, hour), lh_cast_static(lh_time_minute_t, minute),
                lh_cast_static(lh_time_second_t, second));
    return lh_bool_true;
}

lh_usize_t
lh_time_format(const lh_time_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    return lh_str_ptr_format_text(str, str_size, "%02u:%02u:%02u",
                                  lh_cast_static(lh_uint_t, lh_time_get_hour(self)),
                                  lh_cast_static(lh_uint_t, lh_time_get_minute(self)),
                                  lh_cast_static(lh_uint_t, lh_time_get_second(self)));
}
