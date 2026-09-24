#include <lh/time.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/numeric/types.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/uint.h>
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
    lh_time_hour_t self_hour;
    lh_time_minute_t self_minute;
    lh_time_second_t self_second;
    lh_uint_t overflow;

    self_hour = lh_time_get_hour(self);
    self_minute = lh_time_get_minute(self);
    self_second = lh_time_get_second(self);
    overflow = lh_time_second_add(lh_addr_of(self_second), second);
    overflow = lh_time_minute_add(lh_addr_of(self_minute), minute + overflow);
    overflow = lh_time_hour_add(lh_addr_of(self_hour), hour + overflow);
    lh_time_set(self, self_hour, self_minute, self_second);
    return overflow;
}

lh_uint_t
lh_time_sub_custom(lh_time_t *self, lh_uint_t hour, lh_uint_t minute, lh_uint_t second)
{
    lh_time_hour_t self_hour;
    lh_time_minute_t self_minute;
    lh_time_second_t self_second;
    lh_uint_t overflow;

    self_hour = lh_time_get_hour(self);
    self_minute = lh_time_get_minute(self);
    self_second = lh_time_get_second(self);
    overflow = lh_time_second_sub(lh_addr_of(self_second), second);
    overflow = lh_time_minute_sub(lh_addr_of(self_minute), minute + overflow);
    overflow = lh_time_hour_sub(lh_addr_of(self_hour), hour + overflow);
    lh_time_set(self, self_hour, self_minute, self_second);
    return overflow;
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

lh_uint_t
lh_time_seconds_of_day(const lh_time_t *self)
{
    return lh_cast_static(lh_uint_t, lh_time_get_hour(self)) * LH_TIME_SECONDS_PER_HOUR +
           lh_cast_static(lh_uint_t, lh_time_get_minute(self)) * LH_TIME_SECONDS_PER_MINUTE +
           lh_cast_static(lh_uint_t, lh_time_get_second(self));
}

lh_time_t
lh_time_from_seconds_of_day(lh_uint_t seconds)
{
    lh_time_t time;

    lh_time_set(lh_addr_of(time), lh_cast_static(lh_time_hour_t, seconds / LH_TIME_SECONDS_PER_HOUR),
               lh_cast_static(lh_time_minute_t, (seconds % LH_TIME_SECONDS_PER_HOUR) / LH_TIME_SECONDS_PER_MINUTE),
               lh_cast_static(lh_time_second_t, seconds % LH_TIME_SECONDS_PER_MINUTE));
    return time;
}

lh_bool_t
lh_time_equals(const lh_time_t *self, const lh_time_t *other)
{
    return lh_cast_static(lh_bool_t, lh_time_get_hour(self) == lh_time_get_hour(other) &&
                                     lh_time_get_minute(self) == lh_time_get_minute(other) &&
                                     lh_time_get_second(self) == lh_time_get_second(other));
}

lh_bool_t
lh_time_is_at_least(const lh_time_t *self, const lh_time_t *minimum)
{
    lh_time_hour_t self_hour;
    lh_time_hour_t minimum_hour;
    lh_time_minute_t self_minute;
    lh_time_minute_t minimum_minute;

    self_hour = lh_time_get_hour(self);
    minimum_hour = lh_time_get_hour(minimum);
    if (self_hour != minimum_hour)
    {
        return lh_cast_static(lh_bool_t, self_hour > minimum_hour);
    }
    self_minute = lh_time_get_minute(self);
    minimum_minute = lh_time_get_minute(minimum);
    if (self_minute != minimum_minute)
    {
        return lh_cast_static(lh_bool_t, self_minute > minimum_minute);
    }
    return lh_cast_static(lh_bool_t, lh_time_get_second(self) >= lh_time_get_second(minimum));
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
