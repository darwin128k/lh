#include <lh/datetime.h>
#include <lh/assert.h>
#include <lh/memory/std.h>
#include <lh/null.h>
#include <lh/optional/ref.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

void
lh_datetime_pack(lh_datetime_t *self, const lh_date_t *date, const lh_time_t *time)
{
    lh_assert_runtime_ref(self);

    lh_optional_ref(date)
    {
        lh_date_assign(lh_addr_of(self->date), date);
    }
    lh_optional_ref(time)
    {
        lh_time_assign(lh_addr_of(self->time), time);
    }
}

void
lh_datetime_unpack(const lh_datetime_t *self, lh_date_t *date, lh_time_t *time)
{
    lh_assert_runtime_ref(self);

    lh_optional_ref(date)
    {
        lh_date_assign(date, lh_addr_of(self->date));
    }
    lh_optional_ref(time)
    {
        lh_time_assign(time, lh_addr_of(self->time));
    }
}

void
lh_datetime_set(lh_datetime_t *self, const lh_date_t *date, const lh_time_t *time)
{
    lh_datetime_pack(self, date, time);
}

void
lh_datetime_assign(lh_datetime_t *self, const lh_datetime_t *other)
{
    lh_date_t date;
    lh_time_t time;

    lh_datetime_unpack(other, lh_addr_of(date), lh_addr_of(time));
    lh_datetime_set(self, lh_addr_of(date), lh_addr_of(time));
}

lh_date_t
lh_datetime_get_date(const lh_datetime_t *self)
{
    lh_date_t date;
    lh_datetime_unpack(self, lh_addr_of(date), lh_null);
    return date;
}

lh_time_t
lh_datetime_get_time(const lh_datetime_t *self)
{
    lh_time_t time;
    lh_datetime_unpack(self, lh_null, lh_addr_of(time));
    return time;
}

lh_bool_t
lh_datetime_equals(const lh_datetime_t *self, const lh_datetime_t *other)
{
    lh_date_t self_date;
    lh_date_t other_date;
    lh_time_t self_time;
    lh_time_t other_time;

    self_date = lh_datetime_get_date(self);
    other_date = lh_datetime_get_date(other);
    if (!lh_date_equals(lh_addr_of(self_date), lh_addr_of(other_date)))
    {
        return lh_bool_false;
    }

    self_time = lh_datetime_get_time(self);
    other_time = lh_datetime_get_time(other);
    return lh_time_equals(lh_addr_of(self_time), lh_addr_of(other_time));
}

lh_bool_t
lh_datetime_is_at_least(const lh_datetime_t *self, const lh_datetime_t *minimum)
{
    lh_date_t self_date;
    lh_date_t min_date;
    lh_time_t self_time;
    lh_time_t min_time;

    self_date = lh_datetime_get_date(self);
    min_date = lh_datetime_get_date(minimum);
    if (!lh_date_equals(lh_addr_of(self_date), lh_addr_of(min_date)))
    {
        return lh_date_is_at_least(lh_addr_of(self_date), lh_addr_of(min_date));
    }

    self_time = lh_datetime_get_time(self);
    min_time = lh_datetime_get_time(minimum);
    return lh_time_is_at_least(lh_addr_of(self_time), lh_addr_of(min_time));
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
