#include <lh/date.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/optional/ref.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/uint.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

void
lh_date_set_day(lh_date_t *self, lh_date_day_t day)
{
    lh_date_pack(self, lh_null, lh_null, lh_addr_of(day));
}

void
lh_date_clamp_day(lh_date_t *self)
{
    lh_date_day_t dim = lh_date_max_days(self);

    if (dim != 0 && lh_date_get_day(self) > dim)
    {
        lh_date_set_day(self, dim);
    }
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

lh_uint_t
lh_date_add_within_month(lh_date_t *self, lh_uint_t value)
{
    lh_date_day_t day = lh_cast_static(
        lh_date_day_t, lh_cast_static(lh_uint_t, lh_date_get_day(self)) + value);

    lh_date_set_day(self, day);
    return 0;
}

lh_uint_t
lh_date_roll_to_next_month(lh_date_t *self, lh_uint_t *value)
{
    *value -= lh_date_left_days_with_today(self);
    lh_date_set_day(self, LH_DATE_DAY_MIN);
    return lh_date_add_month(self, 1U);
}

lh_uint_t
lh_date_sub_within_month(lh_date_t *self, lh_date_day_t day, lh_uint_t value)
{
    lh_date_set_day(self, lh_cast_static(lh_date_day_t, lh_cast_static(lh_uint_t, day) - value));
    return 0;
}

lh_uint_t
lh_date_roll_to_prev_month(lh_date_t *self, lh_uint_t *value, lh_date_day_t day)
{
    lh_uint_t overflow;
    lh_date_day_t dim;

    *value -= day;
    overflow = lh_date_sub_month(self, 1U);
    dim = lh_date_max_days(self);
    if (dim == 0)
    {
        dim = LH_DATE_DAY_MIN;
    }
    lh_date_set_day(self, dim);
    return overflow;
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

lh_date_day_t
lh_date_max_days(const lh_date_t *self)
{
    return lh_date_days_in_month(lh_date_get_year(self), lh_date_get_month(self));
}

lh_date_day_t
lh_date_left_days(const lh_date_t *self)
{
    return lh_date_days_left(lh_date_get_year(self), lh_date_get_month(self),
                             lh_date_get_day(self));
}

lh_date_day_t
lh_date_left_days_with_today(const lh_date_t *self)
{
    return lh_date_days_left_with_today(lh_date_get_year(self), lh_date_get_month(self),
                                        lh_date_get_day(self));
}

lh_uint_t
lh_date_add_year(lh_date_t *self, lh_uint_t value)
{
    lh_date_year_t year = lh_date_get_year(self);
    lh_uint_t overflow = lh_date_year_add(lh_addr_of(year), value);

    lh_date_pack(self, lh_addr_of(year), lh_null, lh_null);
    lh_date_clamp_day(self);
    return overflow;
}

lh_uint_t
lh_date_sub_year(lh_date_t *self, lh_uint_t value)
{
    lh_date_year_t year = lh_date_get_year(self);
    lh_uint_t overflow = lh_date_year_sub(lh_addr_of(year), value);

    lh_date_pack(self, lh_addr_of(year), lh_null, lh_null);
    lh_date_clamp_day(self);
    return overflow;
}

lh_uint_t
lh_date_add_month(lh_date_t *self, lh_uint_t value)
{
    lh_date_month_t month = lh_date_get_month(self);
    lh_uint_t years = lh_date_month_add(lh_addr_of(month), value);

    lh_date_pack(self, lh_null, lh_addr_of(month), lh_null);
    return lh_date_add_year(self, years);
}

lh_uint_t
lh_date_sub_month(lh_date_t *self, lh_uint_t value)
{
    lh_date_month_t month = lh_date_get_month(self);
    lh_uint_t years = lh_date_month_sub(lh_addr_of(month), value);

    lh_date_pack(self, lh_null, lh_addr_of(month), lh_null);
    return lh_date_sub_year(self, years);
}

lh_uint_t
lh_date_add_day(lh_date_t *self, lh_uint_t value)
{
    lh_uint_t overflow = 0;

    while (value > 0)
    {
        lh_date_day_t left = lh_date_left_days_with_today(self);

        if (left == 0)
        {
            break;
        }
        if (value < left)
        {
            return overflow + lh_date_add_within_month(self, value);
        }
        overflow += lh_date_roll_to_next_month(self, lh_addr_of(value));
    }
    return overflow;
}

lh_uint_t
lh_date_sub_day(lh_date_t *self, lh_uint_t value)
{
    lh_uint_t overflow = 0;

    while (value > 0)
    {
        lh_date_day_t day = lh_date_get_day(self);

        if (day < LH_DATE_DAY_MIN)
        {
            day = LH_DATE_DAY_MIN;
        }
        if (value < lh_cast_static(lh_uint_t, day))
        {
            return overflow + lh_date_sub_within_month(self, day, value);
        }
        overflow += lh_date_roll_to_prev_month(self, lh_addr_of(value), day);
    }
    return overflow;
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
    return lh_cast_static(lh_bool_t, lh_date_get_day(self) == lh_date_get_day(other));
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
        return lh_cast_static(lh_bool_t, self_year > min_year);
    }

    self_month = lh_date_get_month(self);
    min_month = lh_date_get_month(minimum);
    if (self_month != min_month)
    {
        return lh_cast_static(lh_bool_t, self_month > min_month);
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
