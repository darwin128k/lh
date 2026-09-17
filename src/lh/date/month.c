#include <lh/date/month.h>
#include <lh/assert.h>
#include <lh/cast/static.h>

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
