#include <lh/date/year.h>
#include <lh/assert.h>
#include <lh/util/interval.h>

lh_bool_t
lh_date_year_is_leap(lh_date_year_t self)
{
    if (self % 4U != 0)
    {
        return lh_bool_false;
    }
    if (self % 100U != 0)
    {
        return lh_bool_true;
    }
    return (self % 400U == 0) ? lh_bool_true : lh_bool_false;
}

lh_uint_t
lh_date_year_add(lh_date_year_t *self, lh_uint_t value)
{
    lh_ullong_t total;

    lh_assert_runtime_ref(self);
    total = (lh_ullong_t)(*self) + (lh_ullong_t)value;
    *self = (lh_date_year_t)lh_interval_closed_wrap_value(total, (lh_ullong_t)0,
                                                          (lh_ullong_t)LH_DATE_YEAR_MAX);
    return (lh_uint_t)lh_interval_closed_wrap_overflow(total, (lh_ullong_t)0,
                                                       (lh_ullong_t)LH_DATE_YEAR_MAX);
}

lh_uint_t
lh_date_year_sub(lh_date_year_t *self, lh_uint_t value)
{
    lh_ullong_t cur;

    lh_assert_runtime_ref(self);
    cur = (lh_ullong_t)(*self);
    *self = (lh_date_year_t)lh_interval_closed_unsigned_sub_wrap_value(
        cur, (lh_ullong_t)value, (lh_ullong_t)0, (lh_ullong_t)LH_DATE_YEAR_MAX);
    return (lh_uint_t)lh_interval_closed_unsigned_sub_wrap_overflow(
        cur, (lh_ullong_t)value, (lh_ullong_t)0, (lh_ullong_t)LH_DATE_YEAR_MAX);
}
