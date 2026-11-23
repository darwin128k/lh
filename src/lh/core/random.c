/**
 * @file random.c
 * @brief Implementation of `lh/core/random.h`.
 */
#include <lh/core/random.h>

#include <lh/core/assert/runtime.h>

lh_u32_t
lh_random_get_state(const lh_random_t *self)
{
    lh_assert_runtime_ref(self);
    return self->state;
}

lh_void
lh_random_set_state(lh_random_t *self, lh_u32_t state)
{
    lh_assert_runtime_ref(self);
    self->state = state;
}

lh_u32_t
lh_random_next(lh_random_t *self)
{
    lh_u32_t x = lh_random_get_state(self);

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    lh_random_set_state(self, x);
    return x;
}

lh_bool_t
lh_random_is_hit(lh_random_t *self, lh_u8_t percent)
{
    return lh_random_next(self) % 100U < percent;
}

lh_void
lh_random_init(lh_random_t *self, lh_u32_t seed)
{
    lh_random_set_state(self, seed == 0U ? LH_RANDOM_ZERO_SEED_STATE : seed);
}
