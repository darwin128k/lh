#include <lh/date/month/index.h>
#include <lh/assert.h>
#include <lh/util/interval.h>

lh_uint_t
lh_date_month_index_add(lh_date_month_index_t *self, lh_uint_t value)
{
    lh_ullong_t total;

    lh_assert_runtime_ref(self);
    total = (lh_ullong_t)(*self) + (lh_ullong_t)value;
    *self = (lh_date_month_index_t)lh_interval_closed_wrap_value(
        total, (lh_ullong_t)0, (lh_ullong_t)LH_DATE_MONTH_INDEX_MAX);
    return (lh_uint_t)lh_interval_closed_wrap_overflow(total, (lh_ullong_t)0,
                                                       (lh_ullong_t)LH_DATE_MONTH_INDEX_MAX);
}

lh_uint_t
lh_date_month_index_sub(lh_date_month_index_t *self, lh_uint_t value)
{
    lh_ullong_t cur;

    lh_assert_runtime_ref(self);
    cur = (lh_ullong_t)(*self);
    *self = (lh_date_month_index_t)lh_interval_closed_unsigned_sub_wrap_value(
        cur, (lh_ullong_t)value, (lh_ullong_t)0, (lh_ullong_t)LH_DATE_MONTH_INDEX_MAX);
    return (lh_uint_t)lh_interval_closed_unsigned_sub_wrap_overflow(
        cur, (lh_ullong_t)value, (lh_ullong_t)0, (lh_ullong_t)LH_DATE_MONTH_INDEX_MAX);
}
