/**
 * @file dispatch.c
 * @brief Level-1 accessors of ::lh_controller_dispatch_t.
 */

#include <lh/core/controller/dispatch.h>

#include <lh/core/assert/runtime.h>

struct lh_controller *
lh_controller_dispatch_get_controller(const lh_controller_dispatch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->controller;
}

lh_void
lh_controller_dispatch_set_controller(lh_controller_dispatch_t *self,
                                      struct lh_controller *controller)
{
    lh_assert_runtime_ref(self);
    self->controller = controller;
}

lh_bool_t
lh_controller_dispatch_is_handled(const lh_controller_dispatch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handled;
}

lh_void
lh_controller_dispatch_set_handled(lh_controller_dispatch_t *self, lh_bool_t handled)
{
    lh_assert_runtime_ref(self);
    self->handled = handled;
}
