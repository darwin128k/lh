#include <lh/rail/context.h>
#include <lh/core/assert/runtime.h>

lh_rail_context_open_cb
lh_rail_context_get_open_cb(const lh_rail_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->open_cb;
}

lh_void
lh_rail_context_set_open_cb(lh_rail_context_t *self, lh_rail_context_open_cb open_cb)
{
    lh_assert_runtime_ref(self);
    self->open_cb = open_cb;
}

lh_rail_context_close_cb
lh_rail_context_get_close_cb(const lh_rail_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->close_cb;
}

lh_void
lh_rail_context_set_close_cb(lh_rail_context_t *self, lh_rail_context_close_cb close_cb)
{
    lh_assert_runtime_ref(self);
    self->close_cb = close_cb;
}

lh_ptr
lh_rail_context_get_user(const lh_rail_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_rail_context_set_user(lh_rail_context_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

lh_void
lh_rail_context_init(lh_rail_context_t *self, lh_rail_context_open_cb open_cb,
                     lh_rail_context_close_cb close_cb, lh_ptr user)
{
    lh_rail_context_set_open_cb(self, open_cb);
    lh_rail_context_set_close_cb(self, close_cb);
    lh_rail_context_set_user(self, user);
}

lh_ptr
lh_rail_context_run_open(const lh_rail_context_t *self)
{
    lh_rail_context_open_cb open_cb = lh_rail_context_get_open_cb(self);
    if (lh_null_eq(open_cb))
    {
        return lh_null;
    }
    return open_cb(lh_rail_context_get_user(self));
}

lh_void
lh_rail_context_run_close(const lh_rail_context_t *self, lh_ptr local)
{
    lh_rail_context_close_cb close_cb = lh_rail_context_get_close_cb(self);
    if (lh_ptr_is_set(close_cb))
    {
        close_cb(lh_rail_context_get_user(self), local);
    }
}

lh_void
lh_rail_context_assign(lh_rail_context_t *self, const lh_rail_context_t *other)
{
    lh_rail_context_init(self, lh_rail_context_get_open_cb(other),
                         lh_rail_context_get_close_cb(other), lh_rail_context_get_user(other));
}
