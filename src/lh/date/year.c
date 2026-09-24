#include <lh/date/year.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/util/interval/wrap.h>
#include <lh/util/math.h>

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
