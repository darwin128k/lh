#include <lh/exec/context.h>
#include <lh/core/assert/runtime.h>

lh_exec_context_open_cb
lh_exec_context_get_open_cb(const lh_exec_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->open_cb;
}

lh_void
lh_exec_context_set_open_cb(lh_exec_context_t *self, lh_exec_context_open_cb open_cb)
{
    lh_assert_runtime_ref(self);
    self->open_cb = open_cb;
}

lh_exec_context_close_cb
lh_exec_context_get_close_cb(const lh_exec_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->close_cb;
}

lh_void
lh_exec_context_set_close_cb(lh_exec_context_t *self, lh_exec_context_close_cb close_cb)
{
    lh_assert_runtime_ref(self);
    self->close_cb = close_cb;
}

lh_ptr
lh_exec_context_get_user(const lh_exec_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_exec_context_set_user(lh_exec_context_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

lh_void
lh_exec_context_init(lh_exec_context_t *self, lh_exec_context_open_cb open_cb,
                     lh_exec_context_close_cb close_cb, lh_ptr user)
{
    lh_exec_context_set_open_cb(self, open_cb);
    lh_exec_context_set_close_cb(self, close_cb);
    lh_exec_context_set_user(self, user);
}

lh_ptr
lh_exec_context_run_open(const lh_exec_context_t *self)
{
    lh_exec_context_open_cb open_cb = lh_exec_context_get_open_cb(self);
    if (lh_null_eq(open_cb))
    {
        return lh_null;
    }
    return open_cb(lh_exec_context_get_user(self));
}

lh_void
lh_exec_context_run_close(const lh_exec_context_t *self, lh_ptr local)
{
    lh_exec_context_close_cb close_cb = lh_exec_context_get_close_cb(self);
    if (lh_ptr_is_set(close_cb))
    {
        close_cb(lh_exec_context_get_user(self), local);
    }
}

lh_void
lh_exec_context_assign(lh_exec_context_t *self, const lh_exec_context_t *other)
{
    lh_exec_context_init(self, lh_exec_context_get_open_cb(other),
                         lh_exec_context_get_close_cb(other), lh_exec_context_get_user(other));
}
