#include <lh/date.h>
#include <lh/assert.h>
#include <lh/char/digit.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/optional/ref.h>
#include <lh/str/format/text.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>
#include <lh/util/interval.h>
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

static lh_uint_t
lh_date_year_add_amount(lh_date_year_t *year, lh_ullong_t value)
{
    lh_ullong_t total = (lh_ullong_t)(*year) + value;

    *year = (lh_date_year_t)lh_interval_closed_wrap_value(total, (lh_ullong_t)0,
                                                          (lh_ullong_t)LH_DATE_YEAR_MAX);
    return (lh_uint_t)lh_interval_closed_wrap_overflow(total, (lh_ullong_t)0,
                                                       (lh_ullong_t)LH_DATE_YEAR_MAX);
}

static lh_uint_t
lh_date_year_sub_amount(lh_date_year_t *year, lh_ullong_t value)
{
    lh_ullong_t cur = (lh_ullong_t)(*year);
    lh_ullong_t size = lh_interval_closed_get_size((lh_ullong_t)0, (lh_ullong_t)LH_DATE_YEAR_MAX);

    if (value <= cur)
    {
        *year = (lh_date_year_t)(cur - value);
        return 0;
    }

    {
        lh_ullong_t need = value - cur;
        lh_ullong_t borrow = (need + size - 1U) / size;

        *year = (lh_date_year_t)(borrow * size + cur - value);
        return (lh_uint_t)borrow;
    }
}

static lh_uint_t
lh_date_inc_month(lh_date_year_t *year, lh_date_month_t *month)
{
    if (*month < LH_DATE_MONTH_MAX)
    {
        *month = (lh_date_month_t)(*month + 1U);
        return 0;
    }

    *month = LH_DATE_MONTH_MIN;
    return lh_date_year_add_amount(year, 1U);
}

static lh_uint_t
lh_date_dec_month(lh_date_year_t *year, lh_date_month_t *month)
{
    if (*month > LH_DATE_MONTH_MIN)
    {
        *month = (lh_date_month_t)(*month - 1U);
        return 0;
    }

    *month = LH_DATE_MONTH_MAX;
    return lh_date_year_sub_amount(year, 1U);
}

static void
lh_date_clamp_day(lh_date_year_t year, lh_date_month_t month, lh_date_day_t *day)
{
    lh_date_day_t dim = lh_date_days_in_month(year, month);

    if (dim != 0 && *day > dim)
    {
        *day = dim;
    }
}

static lh_uint_t
lh_date_add_amount(lh_date_t *self, lh_ullong_t add_years, lh_ullong_t add_months,
                   lh_ullong_t add_days)
{
    lh_date_year_t year;
    lh_date_month_t month;
    lh_date_day_t day;
    lh_ullong_t overflow = 0;
    lh_ullong_t i;

    lh_date_unpack(self, lh_addr_of(year), lh_addr_of(month), lh_addr_of(day));

    for (i = 0; i < add_days; i++)
    {
        lh_date_day_t dim = lh_date_days_in_month(year, month);

        if (dim == 0)
        {
            break;
        }
        if (day < dim)
        {
            day = (lh_date_day_t)(day + 1U);
        }
        else
        {
            day = LH_DATE_DAY_MIN;
            overflow += lh_date_inc_month(lh_addr_of(year), lh_addr_of(month));
        }
    }

    for (i = 0; i < add_months; i++)
    {
        overflow += lh_date_inc_month(lh_addr_of(year), lh_addr_of(month));
        lh_date_clamp_day(year, month, lh_addr_of(day));
    }

    overflow += lh_date_year_add_amount(lh_addr_of(year), add_years);
    lh_date_clamp_day(year, month, lh_addr_of(day));

    lh_date_set(self, year, month, day);
    return (lh_uint_t)overflow;
}

static lh_uint_t
lh_date_sub_amount(lh_date_t *self, lh_ullong_t sub_years, lh_ullong_t sub_months,
                   lh_ullong_t sub_days)
{
    lh_date_year_t year;
    lh_date_month_t month;
    lh_date_day_t day;
    lh_ullong_t overflow = 0;
    lh_ullong_t i;

    lh_date_unpack(self, lh_addr_of(year), lh_addr_of(month), lh_addr_of(day));

    for (i = 0; i < sub_days; i++)
    {
        if (day > LH_DATE_DAY_MIN)
        {
            day = (lh_date_day_t)(day - 1U);
        }
        else
        {
            overflow += lh_date_dec_month(lh_addr_of(year), lh_addr_of(month));
            day = lh_date_days_in_month(year, month);
            if (day == 0)
            {
                day = LH_DATE_DAY_MIN;
            }
        }
    }

    for (i = 0; i < sub_months; i++)
    {
        overflow += lh_date_dec_month(lh_addr_of(year), lh_addr_of(month));
        lh_date_clamp_day(year, month, lh_addr_of(day));
    }

    overflow += lh_date_year_sub_amount(lh_addr_of(year), sub_years);
    lh_date_clamp_day(year, month, lh_addr_of(day));

    lh_date_set(self, year, month, day);
    return (lh_uint_t)overflow;
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

lh_uint_t
lh_date_add(lh_date_t *self, const lh_date_t *other)
{
    return lh_date_add_amount(self, (lh_ullong_t)lh_date_get_year(other),
                              (lh_ullong_t)lh_date_get_month(other),
                              (lh_ullong_t)lh_date_get_day(other));
}

lh_uint_t
lh_date_sub(lh_date_t *self, const lh_date_t *other)
{
    return lh_date_sub_amount(self, (lh_ullong_t)lh_date_get_year(other),
                              (lh_ullong_t)lh_date_get_month(other),
                              (lh_ullong_t)lh_date_get_day(other));
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
