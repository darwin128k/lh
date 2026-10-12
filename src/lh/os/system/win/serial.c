/**
 * @file serial.c
 * @brief Win32 backend for `lh/os/system/serial.h`: a COM port.
 */

#include <lh/void.h>
#include <lh/os/system/serial.h>

#include <lh/assert/runtime.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/os/str.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/str.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

/* Room for `\\.\` and a port name; nothing longer is a port. */
#define LH_OS_SYSTEM_WIN_SERIAL_NAME_MAX 64

static lh_os_system_win_handle_t
lh_os_system_serial_native(lh_os_system_serial_handle_t handle)
{
    return lh_cast_reinterpret(lh_os_system_win_handle_t, handle);
}

static lh_os_system_win_dword_t
lh_os_system_serial_size(lh_usize_t size)
{
    return size > (lh_usize_t)LH_OS_SYSTEM_WIN_MAXDWORD ? LH_OS_SYSTEM_WIN_MAXDWORD : (lh_os_system_win_dword_t)size;
}

/* Set up an opened port: the line, and timeouts that make a read return at once. */
static lh_bool_t
lh_os_system_serial_setup(lh_os_system_win_handle_t port, lh_u32_t baud, lh_os_serial_parity_t parity,
                          lh_u8_t stop_bits)
{
    lh_os_system_win_dcb_t dcb;
    lh_os_system_win_commtimeouts_t timeouts;

    lh_memory_set(lh_addr_of(dcb), sizeof(dcb), 0);
    dcb.DCBlength = (lh_os_system_win_dword_t)sizeof(dcb);
    lh_return_if(GetCommState(port, lh_addr_of(dcb)) == 0, lh_bool_false);
    dcb.BaudRate = (lh_os_system_win_dword_t)baud;
    dcb.ByteSize = 8U;
    dcb.Parity = (lh_byte_t)parity; /* NOPARITY, ODDPARITY, EVENPARITY are 0, 1, 2 */
    dcb.StopBits = (lh_byte_t)(stop_bits == 2U ? LH_OS_SYSTEM_WIN_TWOSTOPBITS : LH_OS_SYSTEM_WIN_ONESTOPBIT);
    /* Every bit field said outright rather than kept from whatever was there: no
       flow control of either kind -- RS-485 has neither -- and no abort on error,
       which would stop every read after one framing error until it is cleared.
       DTR and RTS on, which some adapters need to power their transceiver. */
    dcb.flags = LH_OS_SYSTEM_WIN_DCB_BINARY | LH_OS_SYSTEM_WIN_DCB_DTR_ENABLE | LH_OS_SYSTEM_WIN_DCB_RTS_ENABLE |
                (parity != lh_os_serial_parity_none ? LH_OS_SYSTEM_WIN_DCB_PARITY : 0UL);
    lh_return_if(SetCommState(port, lh_addr_of(dcb)) == 0, lh_bool_false);

    /* MAXDWORD, 0, 0 is the documented way to say "return what has arrived, now":
       a read never waits. Writes have no timeout and complete when the bytes are
       sent, which is the frame's transmission time. */
    timeouts.ReadIntervalTimeout = LH_OS_SYSTEM_WIN_MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0UL;
    timeouts.ReadTotalTimeoutConstant = 0UL;
    timeouts.WriteTotalTimeoutMultiplier = 0UL;
    timeouts.WriteTotalTimeoutConstant = 0UL;
    lh_return_if(SetCommTimeouts(port, lh_addr_of(timeouts)) == 0, lh_bool_false);
    (void)PurgeComm(port, LH_OS_SYSTEM_WIN_PURGE_RXCLEAR | LH_OS_SYSTEM_WIN_PURGE_TXCLEAR);
    return lh_bool_true;
}

lh_os_system_serial_handle_t
lh_os_system_serial_open(lh_str_cptr name, lh_u32_t baud, lh_os_serial_parity_t parity, lh_u8_t stop_bits)
{
    lh_char_t path[LH_OS_SYSTEM_WIN_SERIAL_NAME_MAX];
    lh_os_str_t os_path;
    lh_os_system_win_handle_t port;
    lh_usize_t at = 0U;
    lh_usize_t i = 0U;
    lh_bool_t ok;

    lh_assert_runtime_ref(name);
    lh_return_if(baud == 0U || (stop_bits != 1U && stop_bits != 2U), LH_OS_SYSTEM_SERIAL_HANDLE_INVALID);
    /* COM1 to COM9 open by their names; COM10 and up only as \\.\COM10, and the
       long form works for all of them -- so a bare name gets it. */
    if (!(name[0] == '\\' && name[1] == '\\'))
    {
        path[at++] = '\\';
        path[at++] = '\\';
        path[at++] = '.';
        path[at++] = '\\';
    }
    while (name[i] != '\0' && at + 1U < (lh_usize_t)LH_OS_SYSTEM_WIN_SERIAL_NAME_MAX)
    {
        path[at++] = name[i++];
    }
    lh_return_if(name[i] != '\0', LH_OS_SYSTEM_SERIAL_HANDLE_INVALID);
    path[at] = '\0';

    ok = lh_os_system_str_init_by_utf8(lh_addr_of(os_path), path);
    port = ok ? CreateFileW(lh_os_str_get_data(lh_addr_of(os_path)),
                            LH_OS_SYSTEM_WIN_GENERIC_READ | LH_OS_SYSTEM_WIN_GENERIC_WRITE, 0UL, lh_null,
                            LH_OS_SYSTEM_WIN_OPEN_EXISTING, 0UL, lh_null)
              : LH_OS_SYSTEM_WIN_INVALID_HANDLE;
    lh_os_str_deinit(lh_addr_of(os_path));
    if (port == LH_OS_SYSTEM_WIN_INVALID_HANDLE)
    {
        if (ok)
        {
            lh_os_system_error_capture();
        }
        return LH_OS_SYSTEM_SERIAL_HANDLE_INVALID;
    }
    if (!lh_os_system_serial_setup(port, baud, parity, stop_bits))
    {
        lh_os_system_error_capture();
        (void)CloseHandle(port);
        return LH_OS_SYSTEM_SERIAL_HANDLE_INVALID;
    }
    return lh_cast_reinterpret(lh_os_system_serial_handle_t, port);
}

lh_void
lh_os_system_serial_close(lh_os_system_serial_handle_t handle)
{
    if (handle != LH_OS_SYSTEM_SERIAL_HANDLE_INVALID)
    {
        (void)CloseHandle(lh_os_system_serial_native(handle));
    }
}

lh_ssize_t
lh_os_system_serial_read(lh_os_system_serial_handle_t handle, lh_ptr buf, lh_usize_t size)
{
    lh_os_system_win_dword_t got = 0UL;

    if (ReadFile(lh_os_system_serial_native(handle), buf, lh_os_system_serial_size(size), lh_addr_of(got), lh_null) ==
        0)
    {
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    /* Zero with the timeouts above is "nothing has arrived", never the end. */
    return got == 0UL ? LH_OS_RESULT_PENDING : lh_cast_static(lh_ssize_t, got);
}

lh_ssize_t
lh_os_system_serial_write(lh_os_system_serial_handle_t handle, const lh_ptr buf, lh_usize_t size)
{
    lh_os_system_win_dword_t put = 0UL;

    if (WriteFile(lh_os_system_serial_native(handle), buf, lh_os_system_serial_size(size), lh_addr_of(put), lh_null) ==
        0)
    {
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return lh_cast_static(lh_ssize_t, put);
}
