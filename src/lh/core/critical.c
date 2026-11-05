#include <lh/core/critical.h>
#include <lh/core/assert/runtime.h>

lh_critical_enter_cb
lh_critical_get_enter_cb(const lh_critical_t *self)
{
    lh_assert_runtime_ref(self);
    return self->enter_cb;
}

lh_void
lh_critical_set_enter_cb(lh_critical_t *self, lh_critical_enter_cb enter_cb)
{
    lh_assert_runtime_ref(self);
    self->enter_cb = enter_cb;
}

lh_critical_leave_cb
lh_critical_get_leave_cb(const lh_critical_t *self)
{
    lh_assert_runtime_ref(self);
    return self->leave_cb;
}

lh_void
lh_critical_set_leave_cb(lh_critical_t *self, lh_critical_leave_cb leave_cb)
{
    lh_assert_runtime_ref(self);
    self->leave_cb = leave_cb;
}

lh_bool_t
lh_critical_is_empty(const lh_critical_t *self)
{
    return lh_null_eq(lh_critical_get_enter_cb(self)) || lh_null_eq(lh_critical_get_leave_cb(self));
}

lh_void
lh_critical_assign(lh_critical_t *self, const lh_critical_t *other)
{
    lh_critical_set_enter_cb(self, lh_critical_get_enter_cb(other));
    lh_critical_set_leave_cb(self, lh_critical_get_leave_cb(other));
}

lh_usize_t
lh_critical_enter(const lh_critical_t *self)
{
    if (lh_critical_is_empty(self))
    {
        return 0U;
    }
    return lh_critical_get_enter_cb(self)();
}

lh_void
lh_critical_leave(const lh_critical_t *self, lh_usize_t state)
{
    if (lh_critical_is_empty(self))
    {
        return;
    }
    lh_critical_get_leave_cb(self)(state);
}
