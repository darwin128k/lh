#include <lh/timer/tick.h>
#include <lh/math.h>

lh_bool_t
lh_tick_equals(lh_tick_t self, lh_tick_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(self, other));
}

lh_bool_t
lh_tick_is_at_least(lh_tick_t self, lh_tick_t minimum)
{
    return lh_cast_static(lh_bool_t, lh_math_ge(self, minimum));
}

lh_bool_t
lh_tick_is_less(lh_tick_t self, lh_tick_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_lt(self, other));
}

lh_bool_t
lh_tick_is_greater(lh_tick_t self, lh_tick_t other)
{
    return lh_tick_is_less(other, self);
}

lh_tick_t
lh_tick_elapsed(lh_tick_t since, lh_tick_t now)
{
    return lh_type_cast(lh_tick_t, now - since);
}

lh_bool_t
lh_tick_is_after(lh_tick_t self, lh_tick_t other)
{
    return lh_type_cast(lh_bool_t, lh_type_cast(lh_s32_t, lh_tick_elapsed(other, self)) > 0);
}
