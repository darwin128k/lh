/**
 * @file hooks.c
 * @brief Level-1 accessors of ::lh_ui_web_host_hooks_t.
 */

#include <lh/ui/web/host/hooks.h>


lh_ptr
lh_ui_web_host_hooks_get_ctx(const lh_ui_web_host_hooks_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ctx;
}

lh_void
lh_ui_web_host_hooks_set_ctx(lh_ui_web_host_hooks_t *self, lh_ptr ctx)
{
    lh_assert_runtime_ref(self);
    self->ctx = ctx;
}

lh_os_input_sink_cb
lh_ui_web_host_hooks_get_input_fn(const lh_ui_web_host_hooks_t *self)
{
    lh_assert_runtime_ref(self);
    return self->input;
}

lh_void
lh_ui_web_host_hooks_set_input_fn(lh_ui_web_host_hooks_t *self, lh_os_input_sink_cb input)
{
    lh_assert_runtime_ref(self);
    self->input = input;
}

lh_os_controller_t *
lh_ui_web_host_hooks_get_controller(const lh_ui_web_host_hooks_t *self)
{
    lh_assert_runtime_ref(self);
    return self->controller;
}

lh_void
lh_ui_web_host_hooks_set_controller(lh_ui_web_host_hooks_t *self, lh_os_controller_t * controller)
{
    lh_assert_runtime_ref(self);
    self->controller = controller;
}
