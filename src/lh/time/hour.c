#include <lh/time/hour.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/util/interval/wrap.h>
#include <lh/util/math.h>

lh_uint_t
lh_time_hour_set(lh_time_hour_t *self, lh_uint_t value)
{
    lh_ullong_t cur = 0;
    lh_uint_t overflow;

    lh_assert_runtime_ref(self);
    overflow = lh_interval_closed_wrap_set(&cur, lh_cast_static(lh_ullong_t, value), 0,
                                           lh_cast_static(lh_ullong_t, LH_TIME_HOUR_MAX));
    *self = lh_cast_static(lh_time_hour_t, cur);
    return overflow;
}

void
lh_time_hour_init(lh_time_hour_t *self, lh_time_hour_t value)
{
    lh_assert_runtime_ref(self);
    *self = value;
}

lh_uint_t
lh_time_hour_add(lh_time_hour_t *self, lh_uint_t value)
{
    lh_ullong_t cur = lh_cast_static(lh_ullong_t, lh_time_hour_get(self));
    lh_uint_t overflow;

    overflow = lh_interval_closed_wrap_add(&cur, lh_cast_static(lh_ullong_t, value), 0,
                                           lh_cast_static(lh_ullong_t, LH_TIME_HOUR_MAX));
    *self = lh_cast_static(lh_time_hour_t, cur);
    return overflow;
}

lh_uint_t
lh_time_hour_sub(lh_time_hour_t *self, lh_uint_t value)
{
    lh_ullong_t cur = lh_cast_static(lh_ullong_t, lh_time_hour_get(self));
    lh_uint_t overflow;

    overflow = lh_interval_closed_wrap_sub(&cur, lh_cast_static(lh_ullong_t, value), 0,
                                           lh_cast_static(lh_ullong_t, LH_TIME_HOUR_MAX));
    *self = lh_cast_static(lh_time_hour_t, cur);
    return overflow;
}

lh_time_hour_t
lh_time_hour_get(const lh_time_hour_t *self)
{
    lh_assert_runtime_ref(self);
    return *self;
}

lh_bool_t
lh_time_hour_equals(const lh_time_hour_t *self, const lh_time_hour_t *other)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_time_hour_get(self), lh_time_hour_get(other)));
}

lh_bool_t
lh_time_hour_is_at_least(const lh_time_hour_t *self, const lh_time_hour_t *minimum)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_ge(lh_time_hour_get(self), lh_time_hour_get(minimum)));
}

lh_bool_t
lh_time_hour_is_less(const lh_time_hour_t *self, const lh_time_hour_t *other)
{
    return lh_cast_static(lh_bool_t, lh_math_lt(lh_time_hour_get(self), lh_time_hour_get(other)));
}

lh_bool_t
lh_time_hour_is_greater(const lh_time_hour_t *self, const lh_time_hour_t *other)
{
    return lh_time_hour_is_less(other, self);
}
