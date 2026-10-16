#include <lh/date/year.h>
#include <lh/assert.h>
#include <lh/util/interval/wrap.h>
#include <lh/math.h>

lh_bool_t
lh_date_year_is_leap(lh_date_year_t self)
{
    if (self % LH_DATE_YEAR_LEAP_CYCLE != 0)
    {
        return lh_bool_false;
    }
    if (self % LH_DATE_YEAR_CENTURY != 0)
    {
        return lh_bool_true;
    }
    return lh_cast_static(lh_bool_t, self % LH_DATE_YEAR_GREGORIAN_CYCLE == 0);
}

lh_s64_t
lh_date_year_days_from_zero(lh_s64_t year)
{
    const lh_s64_t y = lh_math_sub_one(year);
    const lh_s64_t leap_4 = lh_math_floor_div(y, lh_cast_static(lh_s64_t, LH_DATE_YEAR_LEAP_CYCLE));
    const lh_s64_t leap_100 = lh_math_floor_div(y, lh_cast_static(lh_s64_t, LH_DATE_YEAR_CENTURY));
    const lh_s64_t leap_400 = lh_math_floor_div(y, lh_cast_static(lh_s64_t, LH_DATE_YEAR_GREGORIAN_CYCLE));
    const lh_s64_t leap_count = lh_math_add_one(lh_math_add(lh_math_sub(leap_4, leap_100), leap_400));

    return lh_math_add(lh_math_mul(lh_cast_static(lh_s64_t, LH_DATE_YEAR_DAYS_COMMON), year), leap_count);
}

lh_s64_t
lh_date_year_from_days_from_zero(lh_s64_t days)
{
    lh_s64_t year = lh_math_floor_div(lh_math_mul(days, lh_cast_static(lh_s64_t, LH_DATE_YEAR_GREGORIAN_CYCLE)),
                                      lh_cast_static(lh_s64_t, LH_DATE_YEAR_GREGORIAN_CYCLE_DAYS));

    while (lh_math_le(lh_date_year_days_from_zero(lh_math_add_one(year)), days))
    {
        year = lh_math_add_one(year);
    }
    while (lh_math_gt(lh_date_year_days_from_zero(year), days))
    {
        year = lh_math_sub_one(year);
    }
    return year;
}

lh_u16_t
lh_date_year_days(lh_date_year_t self)
{
    return lh_cast_static(lh_u16_t, lh_date_year_is_leap(self) ? lh_math_add_one(LH_DATE_YEAR_DAYS_COMMON)
                                                                : LH_DATE_YEAR_DAYS_COMMON);
}

lh_uint_t
lh_date_year_add(lh_date_year_t *self, lh_uint_t value)
{
    lh_ullong_t cur;
    lh_uint_t overflow;

    lh_assert_runtime_ref(self);
    cur = lh_cast_static(lh_ullong_t, *self);
    overflow = lh_interval_closed_wrap_add(&cur, lh_cast_static(lh_ullong_t, value), 0,
                                           lh_cast_static(lh_ullong_t, LH_DATE_YEAR_MAX));
    *self = lh_cast_static(lh_date_year_t, cur);
    return overflow;
}

lh_uint_t
lh_date_year_sub(lh_date_year_t *self, lh_uint_t value)
{
    lh_ullong_t cur;
    lh_uint_t overflow;

    lh_assert_runtime_ref(self);
    cur = lh_cast_static(lh_ullong_t, *self);
    overflow = lh_interval_closed_wrap_sub(&cur, lh_cast_static(lh_ullong_t, value), 0,
                                           lh_cast_static(lh_ullong_t, LH_DATE_YEAR_MAX));
    *self = lh_cast_static(lh_date_year_t, cur);
    return overflow;
}

lh_bool_t
lh_date_year_equals(lh_date_year_t self, lh_date_year_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(self, other));
}

lh_bool_t
lh_date_year_is_at_least(lh_date_year_t self, lh_date_year_t minimum)
{
    return lh_cast_static(lh_bool_t, lh_math_ge(self, minimum));
}

lh_bool_t
lh_date_year_is_less(lh_date_year_t self, lh_date_year_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_lt(self, other));
}

lh_bool_t
lh_date_year_is_greater(lh_date_year_t self, lh_date_year_t other)
{
    return lh_date_year_is_less(other, self);
}
