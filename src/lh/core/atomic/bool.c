#include <lh/core/atomic/bool.h>
#include <lh/core/assert/runtime.h>

const lh_atomic_uint_t *
lh_atomic_bool_get_value(const lh_atomic_bool_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->value);
}

lh_atomic_uint_t *
lh_atomic_bool_get_value_mut(lh_atomic_bool_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->value);
}

lh_bool_t
lh_atomic_bool_get_acquire(const lh_atomic_bool_t *self)
{
    return lh_atomic_uint_get_acquire(lh_atomic_bool_get_value(self)) != 0U ? lh_bool_true
                                                                            : lh_bool_false;
}

lh_void
lh_atomic_bool_set_relaxed(lh_atomic_bool_t *self, lh_bool_t flag)
{
    lh_atomic_uint_set_relaxed(lh_atomic_bool_get_value_mut(self), flag ? 1U : 0U);
}

lh_void
lh_atomic_bool_set_release(lh_atomic_bool_t *self, lh_bool_t flag)
{
    lh_atomic_uint_set_release(lh_atomic_bool_get_value_mut(self), flag ? 1U : 0U);
}

lh_bool_t
lh_atomic_bool_exchange_acq_rel(lh_atomic_bool_t *self, lh_bool_t flag)
{
    return lh_atomic_uint_exchange_acq_rel(lh_atomic_bool_get_value_mut(self), flag ? 1U : 0U) != 0U
               ? lh_bool_true
               : lh_bool_false;
}
