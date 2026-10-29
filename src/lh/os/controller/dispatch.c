/**
 * @file dispatch.c
 * @brief Level-1 accessors of ::lh_os_controller_dispatch_t.
 */

#include <lh/os/controller/dispatch.h>

#include <lh/assert/runtime.h>

struct lh_os_controller *
lh_os_controller_dispatch_get_controller(const lh_os_controller_dispatch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->controller;
}

lh_void
lh_os_controller_dispatch_set_controller(lh_os_controller_dispatch_t *self,
                                         struct lh_os_controller *controller)
{
    lh_assert_runtime_ref(self);
    self->controller = controller;
}

lh_bool_t
lh_os_controller_dispatch_is_handled(const lh_os_controller_dispatch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handled;
}

lh_void
lh_os_controller_dispatch_set_handled(lh_os_controller_dispatch_t *self, lh_bool_t handled)
{
    lh_assert_runtime_ref(self);
    self->handled = handled;
}
