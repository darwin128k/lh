/**
 * @file gate.c
 * @brief Implementation of `lh/ui/web/host/gate.h`.
 */

#include <lh/ui/web/host/gate.h>
#include <lh/core/assert/runtime.h>

lh_os_net_socket_t *
lh_ui_web_host_gate_get_socket(const lh_ui_web_host_gate_t *self)
{
    lh_assert_runtime_ref(self);
    return self->socket;
}

lh_void
lh_ui_web_host_gate_set_socket(lh_ui_web_host_gate_t *self, lh_os_net_socket_t *value)
{
    lh_assert_runtime_ref(self);
    self->socket = value;
}

lh_bool_t
lh_ui_web_host_gate_is_ready(const lh_ui_web_host_gate_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ready;
}

lh_void
lh_ui_web_host_gate_set_ready(lh_ui_web_host_gate_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->ready = value;
}

lh_void
lh_ui_web_host_gate_get_reader(lh_ui_web_host_gate_t *self, lh_io_reader_t *out)
{
    lh_io_reader_init(out, lh_ui_web_host_gate_read, self);
}

lh_void
lh_ui_web_host_gate_init(lh_ui_web_host_gate_t *self, lh_os_net_socket_t *socket)
{
    lh_ui_web_host_gate_set_socket(self, socket);
    lh_ui_web_host_gate_set_ready(self, lh_bool_false);
}

lh_ssize_t
lh_ui_web_host_gate_read(lh_self_ptr context, lh_ptr out, lh_usize_t size)
{
    lh_ui_web_host_gate_t *self = lh_ptr_cast(lh_ui_web_host_gate_t, context);
    if (lh_math_is_zero(size))
    {
        return 0;
    }
    if (!lh_ui_web_host_gate_is_ready(self))
    {
        return LH_IO_PENDING;
    }
    /* One receive per readiness: a second one could block the host. */
    lh_ui_web_host_gate_set_ready(self, lh_bool_false);
    return lh_os_net_socket_recv(lh_ui_web_host_gate_get_socket(self), out, size);
}
