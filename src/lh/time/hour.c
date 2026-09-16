#include <lh/time/hour.h>
#include <lh/assert.h>
#include <lh/numeric/types.h>
#include <lh/util/interval.h>
#include <lh/util/math.h>

static lh_uint_t
lh_time_hour_wrap(lh_time_hour_t *self, lh_ullong_t total)
{
    *self = (lh_time_hour_t)lh_interval_closed_wrap_value(total, (lh_ullong_t)0,
                                                         (lh_ullong_t)LH_TIME_HOUR_MAX);
    return (lh_uint_t)lh_interval_closed_wrap_overflow(total, (lh_ullong_t)0,
                                                      (lh_ullong_t)LH_TIME_HOUR_MAX);
}

static lh_uint_t
lh_time_hour_wrap_sub(lh_time_hour_t *self, lh_uint_t value)
{
    lh_ullong_t cur = (lh_ullong_t)(*self);
    lh_ullong_t size = (lh_ullong_t)LH_TIME_HOUR_RADIX;
    lh_ullong_t sub = (lh_ullong_t)value;

    if (sub <= cur)
    {
        *self = (lh_time_hour_t)(cur - sub);
        return 0;
    }

    {
        lh_ullong_t need = sub - cur;
        lh_ullong_t borrow = (need + size - 1U) / size;

        *self = (lh_time_hour_t)(borrow * size + cur - sub);
        return (lh_uint_t)borrow;
    }
}

lh_uint_t
lh_time_hour_set(lh_time_hour_t *self, lh_uint_t value)
{
    lh_assert_runtime_ref(self);
    return lh_time_hour_wrap(self, (lh_ullong_t)value);
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
    return lh_time_hour_wrap(self, (lh_ullong_t)lh_time_hour_get(self) + (lh_ullong_t)value);
}

lh_uint_t
lh_time_hour_sub(lh_time_hour_t *self, lh_uint_t value)
{
    lh_assert_runtime_ref(self);
    return lh_time_hour_wrap_sub(self, value);
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
    return lh_math_eq(lh_time_hour_get(self), lh_time_hour_get(other)) ? lh_bool_true
                                                                      : lh_bool_false;
}

lh_bool_t
lh_time_hour_is_at_least(const lh_time_hour_t *self, const lh_time_hour_t *minimum)
{
    return lh_math_ge(lh_time_hour_get(self), lh_time_hour_get(minimum)) ? lh_bool_true
                                                                        : lh_bool_false;
}

lh_bool_t
lh_time_hour_is_less(const lh_time_hour_t *self, const lh_time_hour_t *other)
{
    return lh_math_lt(lh_time_hour_get(self), lh_time_hour_get(other)) ? lh_bool_true
                                                                      : lh_bool_false;
}

lh_bool_t
lh_time_hour_is_greater(const lh_time_hour_t *self, const lh_time_hour_t *other)
{
    return lh_time_hour_is_less(other, self);
}
