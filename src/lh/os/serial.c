/**
 * @file serial.c
 * @brief ::lh_os_serial_t over `lh/os/system/serial.h`.
 */

#include <lh/os/serial.h>

#include <lh/assert/runtime.h>
#include <lh/os/system/serial.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

lh_void
lh_os_serial_init(lh_os_serial_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_SYSTEM_SERIAL_HANDLE_INVALID;
}

lh_bool_t
lh_os_serial_open(lh_os_serial_t *self, lh_str_cptr name, lh_u32_t baud, lh_os_serial_parity_t parity,
                  lh_u8_t stop_bits)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(name);
    lh_os_serial_close(self);
    self->handle = lh_os_system_serial_open(name, baud, parity, stop_bits);
    return lh_os_serial_is_open(self);
}

lh_void
lh_os_serial_close(lh_os_serial_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_ifn(lh_os_serial_is_open(self));
    lh_os_system_serial_close(self->handle);
    self->handle = LH_OS_SYSTEM_SERIAL_HANDLE_INVALID;
}

lh_bool_t
lh_os_serial_is_open(const lh_os_serial_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle != LH_OS_SYSTEM_SERIAL_HANDLE_INVALID ? lh_bool_true : lh_bool_false;
}

lh_ssize_t
lh_os_serial_read(lh_self_ptr self, lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_serial_read(lh_ptr_cast(lh_os_serial_t, self)->handle, buf, size);
}

lh_ssize_t
lh_os_serial_write(lh_self_ptr self, const lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_serial_write(lh_ptr_cast(lh_os_serial_t, self)->handle, buf, size);
}

lh_io_stream_t
lh_os_serial_get_stream(lh_os_serial_t *self)
{
    lh_io_reader_t reader;
    lh_io_writer_t writer;
    lh_io_stream_t stream;

    lh_assert_runtime_ref(self);
    lh_io_reader_init(lh_addr_of(reader), lh_os_serial_read, self);
    lh_io_writer_init(lh_addr_of(writer), lh_os_serial_write, self);
    lh_io_stream_init(lh_addr_of(stream), lh_addr_of(reader), lh_addr_of(writer));
    return stream;
}
