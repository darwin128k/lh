#include <lh/date/month.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/math.h>

lh_date_month_index_t
lh_date_month_to_index(lh_date_month_t month)
{
    return lh_cast_static(lh_date_month_index_t, month - LH_DATE_MONTH_MIN);
}

lh_date_month_t
lh_date_month_from_index(lh_date_month_index_t index)
{
    return lh_cast_static(lh_date_month_t, index + LH_DATE_MONTH_MIN);
}

lh_uint_t
lh_date_month_add(lh_date_month_t *self, lh_uint_t value)
{
    lh_date_month_index_t index;
    lh_uint_t years;

    lh_assert_runtime_ref(self);
    if (*self < LH_DATE_MONTH_MIN || *self > LH_DATE_MONTH_MAX)
    {
        return 0;
    }
    index = lh_date_month_to_index(*self);
    years = lh_date_month_index_add(&index, value);
    *self = lh_date_month_from_index(index);
    return years;
}

lh_uint_t
lh_date_month_sub(lh_date_month_t *self, lh_uint_t value)
{
    lh_date_month_index_t index;
    lh_uint_t years;

    lh_assert_runtime_ref(self);
    if (*self < LH_DATE_MONTH_MIN || *self > LH_DATE_MONTH_MAX)
    {
        return 0;
    }
    index = lh_date_month_to_index(*self);
    years = lh_date_month_index_sub(&index, value);
    *self = lh_date_month_from_index(index);
    return years;
}

lh_bool_t
lh_date_month_equals(lh_date_month_t self, lh_date_month_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(self, other));
}

lh_bool_t
lh_date_month_is_at_least(lh_date_month_t self, lh_date_month_t minimum)
{
    return lh_cast_static(lh_bool_t, lh_math_ge(self, minimum));
}

lh_bool_t
lh_date_month_is_less(lh_date_month_t self, lh_date_month_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_lt(self, other));
}

lh_bool_t
lh_date_month_is_greater(lh_date_month_t self, lh_date_month_t other)
{
    return lh_date_month_is_less(other, self);
}
