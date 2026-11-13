#include <lh/core/net/modbus/health.h>

#include <lh/core/assert/runtime.h>

lh_u16_t
lh_mb_health_get_failures(const lh_mb_health_t *self)
{
    lh_assert_runtime_ref(self);
    return self->failures;
}

lh_void
lh_mb_health_set_failures(lh_mb_health_t *self, lh_u16_t failures)
{
    lh_assert_runtime_ref(self);
    self->failures = failures;
}

lh_u16_t
lh_mb_health_get_limit(const lh_mb_health_t *self)
{
    lh_assert_runtime_ref(self);
    return self->limit;
}

lh_void
lh_mb_health_set_limit(lh_mb_health_t *self, lh_u16_t limit)
{
    lh_assert_runtime_ref(self);
    self->limit = lh_math_max(limit, lh_cast_static(lh_u16_t, 1U));
}

lh_bool_t
lh_mb_health_is_lost(const lh_mb_health_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_mb_health_get_failures(self) >= lh_mb_health_get_limit(self));
}

lh_void
lh_mb_health_count_answer(lh_mb_health_t *self)
{
    lh_mb_health_set_failures(self, 0);
}

lh_void
lh_mb_health_count_failure(lh_mb_health_t *self)
{
    /* Up to the limit and no further: the count says "gone", not for how long. */
    lh_mb_health_set_failures(
        self, lh_math_min(lh_cast_static(lh_u16_t, lh_mb_health_get_failures(self) + 1U),
                          lh_mb_health_get_limit(self)));
}

lh_void
lh_mb_health_init(lh_mb_health_t *self, lh_u16_t limit)
{
    lh_mb_health_set_failures(self, 0);
    lh_mb_health_set_limit(self, limit);
}
