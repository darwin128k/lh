#include <lh/datetime.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/memory/std.h>
#include <lh/str/split/next.h>
#include <lh/timestamp.h>
#include <lh/util/addr.h>

void
lh_datetime_set_date(lh_datetime_t *self, const lh_date_t *date)
{
    lh_assert_runtime_ref(self);
    lh_date_assign(lh_addr_of(self->date), date);
}

void
lh_datetime_set_time(lh_datetime_t *self, const lh_time_t *time)
{
    lh_assert_runtime_ref(self);
    lh_time_assign(lh_addr_of(self->time), time);
}

void
lh_datetime_set(lh_datetime_t *self, const lh_date_t *date, const lh_time_t *time)
{
    lh_datetime_set_date(self, date);
    lh_datetime_set_time(self, time);
}

void
lh_datetime_assign(lh_datetime_t *self, const lh_datetime_t *other)
{
    lh_date_t date = lh_datetime_get_date(other);
    lh_time_t time = lh_datetime_get_time(other);

    lh_datetime_set(self, lh_addr_of(date), lh_addr_of(time));
}

lh_uint_t
lh_datetime_add(lh_datetime_t *self, const lh_datetime_t *other)
{
    lh_date_t date;
    lh_time_t time;
    lh_date_t other_date;
    lh_time_t other_time;
    lh_uint_t day_carry;
    lh_uint_t overflow;

    date = lh_datetime_get_date(self);
    time = lh_datetime_get_time(self);
    other_date = lh_datetime_get_date(other);
    other_time = lh_datetime_get_time(other);
    day_carry = lh_time_add(lh_addr_of(time), lh_addr_of(other_time));
    overflow = lh_date_add_day(lh_addr_of(date), day_carry);
    overflow += lh_date_add(lh_addr_of(date), lh_addr_of(other_date));
    lh_datetime_set(self, lh_addr_of(date), lh_addr_of(time));
    return overflow;
}

lh_uint_t
lh_datetime_sub(lh_datetime_t *self, const lh_datetime_t *other)
{
    lh_date_t date;
    lh_time_t time;
    lh_date_t other_date;
    lh_time_t other_time;
    lh_uint_t day_borrow;
    lh_uint_t overflow;

    date = lh_datetime_get_date(self);
    time = lh_datetime_get_time(self);
    other_date = lh_datetime_get_date(other);
    other_time = lh_datetime_get_time(other);
    day_borrow = lh_time_sub(lh_addr_of(time), lh_addr_of(other_time));
    overflow = lh_date_sub(lh_addr_of(date), lh_addr_of(other_date));
    overflow += lh_date_sub_day(lh_addr_of(date), day_borrow);
    lh_datetime_set(self, lh_addr_of(date), lh_addr_of(time));
    return overflow;
}

lh_date_t
lh_datetime_get_date(const lh_datetime_t *self)
{
    lh_assert_runtime_ref(self);
    return self->date;
}

lh_time_t
lh_datetime_get_time(const lh_datetime_t *self)
{
    lh_assert_runtime_ref(self);
    return self->time;
}

lh_bool_t
lh_datetime_equals(const lh_datetime_t *self, const lh_datetime_t *other)
{
    return lh_timestamp_equals(lh_timestamp_from_datetime(self), lh_timestamp_from_datetime(other));
}

lh_bool_t
lh_datetime_is_at_least(const lh_datetime_t *self, const lh_datetime_t *minimum)
{
    return lh_timestamp_is_at_least(lh_timestamp_from_datetime(self), lh_timestamp_from_datetime(minimum));
}

lh_bool_t
lh_datetime_is_less(const lh_datetime_t *self, const lh_datetime_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_datetime_is_at_least(self, other));
}

lh_bool_t
lh_datetime_is_greater(const lh_datetime_t *self, const lh_datetime_t *other)
{
    return lh_datetime_is_less(other, self);
}

lh_bool_t
lh_datetime_parse(lh_str_cptr str, lh_usize_t str_size, lh_datetime_t *out)
{
    lh_str_cptr date_field;
    lh_usize_t date_size;
    lh_str_cptr time_field;
    lh_usize_t time_size;
    lh_bool_t had_delim;
    lh_usize_t pos = 0;
    lh_date_t date;
    lh_time_t time;

    if (!lh_str_ptr_split_next(str, str_size, ' ', lh_addr_of(pos), lh_addr_of(date_field),
                               lh_addr_of(date_size), lh_addr_of(had_delim)))
    {
        return lh_bool_false;
    }
    if (!had_delim)
    {
        return lh_bool_false;
    }
    if (!lh_str_ptr_split_next(str, str_size, ' ', lh_addr_of(pos), lh_addr_of(time_field),
                               lh_addr_of(time_size), lh_addr_of(had_delim)))
    {
        return lh_bool_false;
    }
    if (had_delim)
    {
        return lh_bool_false;
    }
    if (!lh_date_parse(date_field, date_size, lh_addr_of(date)))
    {
        return lh_bool_false;
    }
    if (!lh_time_parse(time_field, time_size, lh_addr_of(time)))
    {
        return lh_bool_false;
    }

    lh_datetime_set(out, lh_addr_of(date), lh_addr_of(time));
    return lh_bool_true;
}

lh_usize_t
lh_datetime_format(const lh_datetime_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    lh_char_t scratch[LH_DATETIME_TEXT_MAX];
    lh_date_t date;
    lh_time_t time;
    lh_usize_t date_len;
    lh_usize_t time_len;
    lh_usize_t total;

    date = lh_datetime_get_date(self);
    time = lh_datetime_get_time(self);
    date_len = lh_date_format(lh_addr_of(date), scratch, LH_DATE_TEXT_MAX);
    if (date_len == 0)
    {
        return 0;
    }
    scratch[date_len] = ' ';
    time_len = lh_time_format(lh_addr_of(time), scratch + date_len + 1U, LH_TIME_TEXT_MAX);
    if (time_len == 0)
    {
        return 0;
    }
    total = date_len + 1U + time_len;
    if (total > str_size)
    {
        return 0;
    }
    lh_memory_std_copy(str, scratch, total);
    return total;
}
