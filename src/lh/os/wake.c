#include <lh/os/wake.h>
#include <lh/os/system/net/socket.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/assert.h>
#include <lh/math.h>

lh_void
lh_os_wake_init(lh_os_wake_t *self)
{
    lh_assert_runtime_ref(self);
    self->reader = self->writer = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
}

lh_bool_t
lh_os_wake_open(lh_os_wake_t *self, lh_u32_t timeout_ms)
{
    lh_os_system_net_socket_handle_t pair[2];
    lh_assert_runtime_ref(self);
    if (lh_math_ne(self->reader, LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID) ||
        lh_math_ne(self->writer, LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID))
    {
        lh_os_set_last_error_lit(lh_os_error_code_invalid_argument, "wake signal is already open");
        return lh_bool_false;
    }
    if (!lh_os_system_net_socket_pair(pair, timeout_ms))
    {
        return lh_bool_false;
    }
    self->reader = pair[0];
    self->writer = pair[1];
    return lh_bool_true;
}

lh_bool_t
lh_os_wake_signal(lh_os_wake_t *self)
{
    const lh_byte_t byte = 1;
    lh_assert_runtime_ref(self);
    const lh_ssize_t result = lh_os_system_net_socket_send(self->writer, lh_addr_of(byte), 1);
    return lh_math_eq(result, 1) || lh_math_eq(result, LH_OS_RESULT_PENDING);
}

lh_bool_t
lh_os_wake_reset(lh_os_wake_t *self)
{
    lh_byte_t buffer[64];
    lh_assert_runtime_ref(self);
    for (;;)
    {
        const lh_ssize_t result = lh_os_system_net_socket_recv(self->reader, buffer, sizeof buffer);
        if (lh_math_eq(result, LH_OS_RESULT_PENDING))
        {
            return lh_bool_true;
        }
        if (lh_math_le(result, 0))
        {
            if (lh_math_is_zero(result))
            {
                lh_os_set_last_error_lit(lh_os_error_code_invalid_argument, "wake peer is closed");
            }
            return lh_bool_false;
        }
    }
}

lh_os_system_net_socket_handle_t
lh_os_wake_get_handle(const lh_os_wake_t *self)
{
    lh_assert_runtime_ref(self);
    return self->reader;
}

lh_void
lh_os_wake_close(lh_os_wake_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_math_ne(self->reader, LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID))
    {
        lh_os_system_net_socket_close(self->reader);
    }
    if (lh_math_ne(self->writer, LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID))
    {
        lh_os_system_net_socket_close(self->writer);
    }
    lh_os_wake_init(self);
}
