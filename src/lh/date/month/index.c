#include <lh/date/month/index.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/util/interval/wrap.h>

lh_uint_t
lh_date_month_index_add(lh_date_month_index_t *self, lh_uint_t value)
{
    lh_ullong_t cur;
    lh_uint_t overflow;

    lh_assert_runtime_ref(self);
    cur = lh_cast_static(lh_ullong_t, *self);
    overflow = lh_interval_closed_wrap_add(&cur, lh_cast_static(lh_ullong_t, value), 0,
                                           lh_cast_static(lh_ullong_t, LH_DATE_MONTH_INDEX_MAX));
    *self = lh_cast_static(lh_date_month_index_t, cur);
    return overflow;
}

lh_uint_t
lh_date_month_index_sub(lh_date_month_index_t *self, lh_uint_t value)
{
    lh_ullong_t cur;
    lh_uint_t overflow;

    lh_assert_runtime_ref(self);
    cur = lh_cast_static(lh_ullong_t, *self);
    overflow = lh_interval_closed_wrap_sub(&cur, lh_cast_static(lh_ullong_t, value), 0,
                                           lh_cast_static(lh_ullong_t, LH_DATE_MONTH_INDEX_MAX));
    *self = lh_cast_static(lh_date_month_index_t, cur);
    return overflow;
}

lh_bool_t
lh_date_month_index_equals(lh_date_month_index_t self, lh_date_month_index_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(self, other));
}

lh_bool_t
lh_date_month_index_is_at_least(lh_date_month_index_t self, lh_date_month_index_t minimum)
{
    return lh_cast_static(lh_bool_t, lh_math_ge(self, minimum));
}

lh_bool_t
lh_date_month_index_is_less(lh_date_month_index_t self, lh_date_month_index_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_lt(self, other));
}

lh_bool_t
lh_date_month_index_is_greater(lh_date_month_index_t self, lh_date_month_index_t other)
{
    return lh_date_month_index_is_less(other, self);
}
