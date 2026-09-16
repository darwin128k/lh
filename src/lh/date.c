#include <lh/date.h>
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
lh_date_parse_component(lh_str_cptr field, lh_usize_t field_size, lh_uint_t max, lh_uint_t *out)
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

static lh_bool_t
lh_date_is_leap_year(lh_date_year_t year)
{
    if (year % 4U != 0)
    {
        return lh_bool_false;
    }
    if (year % 100U != 0)
    {
        return lh_bool_true;
    }
    return (year % 400U == 0) ? lh_bool_true : lh_bool_false;
}

static lh_date_day_t
lh_date_days_in_month(lh_date_year_t year, lh_date_month_t month)
{
    static const lh_date_day_t days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month < LH_DATE_MONTH_MIN || month > LH_DATE_MONTH_MAX)
    {
        return 0;
    }
    if (month == 2 && lh_date_is_leap_year(year))
    {
        return 29;
    }
    return days[month - 1U];
}

void
lh_date_pack(lh_date_t *self, const lh_date_year_t *year, const lh_date_month_t *month,
             const lh_date_day_t *day)
{
    lh_assert_runtime_ref(self);

    lh_optional_ref(year)
    {
        self->year = lh_ptr_deref(year);
    }
    lh_optional_ref(month)
    {
        self->month = lh_ptr_deref(month);
    }
    lh_optional_ref(day)
    {
        self->day = lh_ptr_deref(day);
    }
}

void
lh_date_unpack(const lh_date_t *self, lh_date_year_t *year, lh_date_month_t *month,
               lh_date_day_t *day)
{
    lh_assert_runtime_ref(self);

    lh_optional_ref(year)
    {
        lh_ptr_deref(year) = self->year;
    }
    lh_optional_ref(month)
    {
        lh_ptr_deref(month) = self->month;
    }
    lh_optional_ref(day)
    {
        lh_ptr_deref(day) = self->day;
    }
}

void
lh_date_set(lh_date_t *self, lh_date_year_t year, lh_date_month_t month, lh_date_day_t day)
{
    lh_date_pack(self, lh_addr_of(year), lh_addr_of(month), lh_addr_of(day));
}

void
lh_date_assign(lh_date_t *self, const lh_date_t *other)
{
    lh_date_year_t year;
    lh_date_month_t month;
    lh_date_day_t day;

    lh_date_unpack(other, lh_addr_of(year), lh_addr_of(month), lh_addr_of(day));
    lh_date_set(self, year, month, day);
}

lh_date_year_t
lh_date_get_year(const lh_date_t *self)
{
    lh_date_year_t year;
    lh_date_unpack(self, lh_addr_of(year), lh_null, lh_null);
    return year;
}

lh_date_month_t
lh_date_get_month(const lh_date_t *self)
{
    lh_date_month_t month;
    lh_date_unpack(self, lh_null, lh_addr_of(month), lh_null);
    return month;
}

lh_date_day_t
lh_date_get_day(const lh_date_t *self)
{
    lh_date_day_t day;
    lh_date_unpack(self, lh_null, lh_null, lh_addr_of(day));
    return day;
}

lh_bool_t
lh_date_equals(const lh_date_t *self, const lh_date_t *other)
{
    if (lh_date_get_year(self) != lh_date_get_year(other))
    {
        return lh_bool_false;
    }
    if (lh_date_get_month(self) != lh_date_get_month(other))
    {
        return lh_bool_false;
    }
    return lh_date_get_day(self) == lh_date_get_day(other) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_date_is_at_least(const lh_date_t *self, const lh_date_t *minimum)
{
    lh_date_year_t self_year;
    lh_date_year_t min_year;
    lh_date_month_t self_month;
    lh_date_month_t min_month;

    self_year = lh_date_get_year(self);
    min_year = lh_date_get_year(minimum);
    if (self_year != min_year)
    {
        return self_year > min_year ? lh_bool_true : lh_bool_false;
    }

    self_month = lh_date_get_month(self);
    min_month = lh_date_get_month(minimum);
    if (self_month != min_month)
    {
        return self_month > min_month ? lh_bool_true : lh_bool_false;
    }

    return lh_date_get_day(self) >= lh_date_get_day(minimum) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_date_parse(lh_str_cptr str, lh_usize_t str_size, lh_date_t *out)
{
    lh_uint_t year;
    lh_uint_t month;
    lh_uint_t day;
    lh_usize_t pos = 0;
    lh_usize_t i;
    lh_uint_t component[3];
    const lh_uint_t max[3] = {LH_DATE_YEAR_MAX, LH_DATE_MONTH_MAX, 31U};

    for (i = 0; i < 3U; i++)
    {
        lh_bool_t is_last = i == 2U;
        lh_str_cptr field;
        lh_usize_t field_size;
        lh_bool_t had_delim;

        if (!lh_str_ptr_split_next(str, str_size, '/', lh_addr_of(pos), lh_addr_of(field),
                                   lh_addr_of(field_size), lh_addr_of(had_delim)))
        {
            return lh_bool_false;
        }
        if (had_delim == is_last)
        {
            return lh_bool_false;
        }
        if (!lh_date_parse_component(field, field_size, max[i], lh_addr_of(component[i])))
        {
            return lh_bool_false;
        }
    }

    year = component[0];
    month = component[1];
    day = component[2];
    if (month < LH_DATE_MONTH_MIN || day < LH_DATE_DAY_MIN)
    {
        return lh_bool_false;
    }
    if (day > lh_date_days_in_month((lh_date_year_t)year, (lh_date_month_t)month))
    {
        return lh_bool_false;
    }

    lh_date_set(out, (lh_date_year_t)year, (lh_date_month_t)month, (lh_date_day_t)day);
    return lh_bool_true;
}

lh_usize_t
lh_date_format(const lh_date_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    return lh_str_ptr_format_text(str, str_size, "%u/%02u/%02u", (lh_uint_t)lh_date_get_year(self),
                                  (lh_uint_t)lh_date_get_month(self),
                                  (lh_uint_t)lh_date_get_day(self));
}
