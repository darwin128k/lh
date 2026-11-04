#include <lh/core/runtime/terminate/handler.h>
#include <lh/core/assert/runtime.h>

lh_runtime_terminate_cb
lh_runtime_terminate_handler_get_call_cb(const lh_runtime_terminate_handler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->call_cb;
}

lh_void
lh_runtime_terminate_handler_set_call_cb(lh_runtime_terminate_handler_t *self,
                                         lh_runtime_terminate_cb call_cb)
{
    lh_assert_runtime_ref(self);
    self->call_cb = call_cb;
}

lh_ptr
lh_runtime_terminate_handler_get_context(const lh_runtime_terminate_handler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_runtime_terminate_handler_set_context(lh_runtime_terminate_handler_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_void
lh_runtime_terminate_handler_init(lh_runtime_terminate_handler_t *self,
                                  lh_runtime_terminate_cb call_cb, lh_ptr context)
{
    lh_runtime_terminate_handler_set_call_cb(self, call_cb);
    lh_runtime_terminate_handler_set_context(self, context);
}

lh_void
lh_runtime_terminate_handler_call(const lh_runtime_terminate_handler_t *self)
{
    lh_runtime_terminate_cb call_cb = lh_runtime_terminate_handler_get_call_cb(self);
    if (lh_ptr_is_set(call_cb))
    {
        call_cb(lh_runtime_terminate_handler_get_context(self));
    }
}

lh_void
lh_runtime_terminate_handler_assign(lh_runtime_terminate_handler_t *self,
                                    const lh_runtime_terminate_handler_t *other)
{
    lh_runtime_terminate_handler_init(self, lh_runtime_terminate_handler_get_call_cb(other),
                                      lh_runtime_terminate_handler_get_context(other));
}
