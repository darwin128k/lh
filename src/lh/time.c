#include <lh/time.h>
#include <lh/assert.h>
#include <lh/char/digit.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/optional/ref.h>
#include <lh/str/format/text.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

static lh_bool_t
lh_time_parse_component(lh_str_cptr field, lh_usize_t field_size, lh_uint_t max, lh_uint_t *out)
{
    lh_uint_t value = 0;
    lh_usize_t pos = 0;

    if (field_size == 0)
    {
        return lh_bool_false;
    }

    while (pos < field_size && lh_char_is_digit(field[pos]))
    {
        if (!lh_char_digit_accumulate(lh_addr_of(value), lh_char_to_digit(field[pos])))
        {
            return lh_bool_false;
        }
        if (value > max)
        {
            return lh_bool_false;
        }
        pos++;
    }

    if (pos != field_size)
    {
        return lh_bool_false;
    }

    *out = value;
    return lh_bool_true;
}

void
lh_time_pack(lh_time_t *self, const lh_time_hour_t *hour, const lh_time_minute_t *minute,
             const lh_time_second_t *second)
{
    lh_optional_ref(hour)
    {
        self->hour = lh_ptr_deref(hour);
    }
    lh_optional_ref(minute)
    {
        self->minute = lh_ptr_deref(minute);
    }
    lh_optional_ref(second)
    {
        self->second = lh_ptr_deref(second);
    }
}

void
lh_time_unpack(const lh_time_t *self, lh_time_hour_t *hour, lh_time_minute_t *minute,
               lh_time_second_t *second)
{
    lh_optional_ref(hour)
    {
        lh_ptr_deref(hour) = self->hour;
    }
    lh_optional_ref(minute)
    {
        lh_ptr_deref(minute) = self->minute;
    }
    lh_optional_ref(second)
    {
        lh_ptr_deref(second) = self->second;
    }
}

void
lh_time_set(lh_time_t *self, lh_time_hour_t hour, lh_time_minute_t minute, lh_time_second_t second)
{
    lh_time_pack(self, lh_addr_of(hour), lh_addr_of(minute), lh_addr_of(second));
}

void
lh_time_assign(lh_time_t *self, const lh_time_t *other)
{
    lh_time_hour_t hour;
    lh_time_minute_t minute;
    lh_time_second_t second;

    lh_time_unpack(other, lh_addr_of(hour), lh_addr_of(minute), lh_addr_of(second));
    lh_time_set(self, hour, minute, second);
}

lh_time_hour_t
lh_time_get_hour(const lh_time_t *self)
{
    lh_time_hour_t hour;
    lh_time_unpack(self, lh_addr_of(hour), lh_null, lh_null);
    return hour;
}

lh_time_minute_t
lh_time_get_minute(const lh_time_t *self)
{
    lh_time_minute_t minute;
    lh_time_unpack(self, lh_null, lh_addr_of(minute), lh_null);
    return minute;
}

lh_time_second_t
lh_time_get_second(const lh_time_t *self)
{
    lh_time_second_t second;
    lh_time_unpack(self, lh_null, lh_null, lh_addr_of(second));
    return second;
}

lh_bool_t
lh_time_equals(const lh_time_t *self, const lh_time_t *other)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    if (lh_time_get_hour(self) != lh_time_get_hour(other))
    {
        return lh_bool_false;
    }
    if (lh_time_get_minute(self) != lh_time_get_minute(other))
    {
        return lh_bool_false;
    }
    return lh_time_get_second(self) == lh_time_get_second(other) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_time_is_at_least(const lh_time_t *self, const lh_time_t *minimum)
{
    lh_time_hour_t self_hour;
    lh_time_hour_t min_hour;
    lh_time_minute_t self_minute;
    lh_time_minute_t min_minute;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(minimum);

    self_hour = lh_time_get_hour(self);
    min_hour = lh_time_get_hour(minimum);
    if (self_hour != min_hour)
    {
        return self_hour > min_hour ? lh_bool_true : lh_bool_false;
    }

    self_minute = lh_time_get_minute(self);
    min_minute = lh_time_get_minute(minimum);
    if (self_minute != min_minute)
    {
        return self_minute > min_minute ? lh_bool_true : lh_bool_false;
    }

    return lh_time_get_second(self) >= lh_time_get_second(minimum) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_time_parse(lh_str_cptr str, lh_usize_t str_size, lh_time_t *out)
{
    lh_uint_t component[3];
    const lh_uint_t max[3] = {LH_TIME_HOUR_MAX, LH_TIME_MINUTE_MAX, LH_TIME_SECOND_MAX};
    lh_usize_t pos = 0;
    lh_usize_t i;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    for (i = 0; i < 3U; i++)
    {
        lh_bool_t is_last = i == 2U;
        lh_str_cptr field;
        lh_usize_t field_size;
        lh_bool_t had_delim;

        if (!lh_str_ptr_split_next(str, str_size, ':', lh_addr_of(pos), lh_addr_of(field),
                                   lh_addr_of(field_size), lh_addr_of(had_delim)))
        {
            return lh_bool_false;
        }
        if (had_delim == is_last)
        {
            return lh_bool_false;
        }
        if (!lh_time_parse_component(field, field_size, max[i], lh_addr_of(component[i])))
        {
            return lh_bool_false;
        }
    }

    lh_time_set(out, (lh_time_hour_t)component[0], (lh_time_minute_t)component[1],
                (lh_time_second_t)component[2]);
    return lh_bool_true;
}

lh_usize_t
lh_time_format(const lh_time_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(str);

    return lh_str_ptr_format_text(str, str_size, "%02u:%02u:%02u",
                                  (lh_uint_t)lh_time_get_hour(self),
                                  (lh_uint_t)lh_time_get_minute(self),
                                  (lh_uint_t)lh_time_get_second(self));
}
