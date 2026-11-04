#include <lh/core/runtime/check/handler.h>
#include <lh/core/assert/runtime.h>

lh_runtime_check_fail_cb
lh_runtime_check_handler_get_fail_cb(const lh_runtime_check_handler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fail_cb;
}

lh_void
lh_runtime_check_handler_set_fail_cb(lh_runtime_check_handler_t *self,
                                     lh_runtime_check_fail_cb fail_cb)
{
    lh_assert_runtime_ref(self);
    self->fail_cb = fail_cb;
}

lh_ptr
lh_runtime_check_handler_get_context(const lh_runtime_check_handler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_runtime_check_handler_set_context(lh_runtime_check_handler_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_void
lh_runtime_check_handler_init(lh_runtime_check_handler_t *self, lh_runtime_check_fail_cb fail_cb,
                              lh_ptr context)
{
    lh_runtime_check_handler_set_fail_cb(self, fail_cb);
    lh_runtime_check_handler_set_context(self, context);
}

lh_void
lh_runtime_check_handler_call(const lh_runtime_check_handler_t *self,
                              const lh_exception_t *exception)
{
    lh_runtime_check_fail_cb fail_cb = lh_runtime_check_handler_get_fail_cb(self);
    if (lh_ptr_is_set(fail_cb))
    {
        fail_cb(lh_runtime_check_handler_get_context(self), exception);
    }
}

lh_void
lh_runtime_check_handler_assign(lh_runtime_check_handler_t *self,
                                const lh_runtime_check_handler_t *other)
{
    lh_runtime_check_handler_init(self, lh_runtime_check_handler_get_fail_cb(other),
                                  lh_runtime_check_handler_get_context(other));
}
