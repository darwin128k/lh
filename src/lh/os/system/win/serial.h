/**
 * @file serial.h
 * @brief Backend-private: Win32 COM handle casts and line setup.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_SERIAL_H
#define LH_SRC_OS_SYSTEM_WIN_SERIAL_H

#include <lh/bool.h>
#include <lh/os/serial/parity.h>
#include <lh/os/system/serial/handle.h>
#include <lh/os/system/win/kernel32.h>

/** @p handle as the Win32 `HANDLE` it is. */
lh_os_system_win_handle_t
lh_os_system_serial_native(lh_os_system_serial_handle_t handle);

/** @p size as a `DWORD`, clamped to ::LH_OS_SYSTEM_WIN_MAXDWORD. */
lh_os_system_win_dword_t
lh_os_system_serial_size(lh_usize_t size);

/**
 * @brief Line + non-blocking read timeouts on an opened @p port.
 * @return False when `GetCommState` / `SetCommState` / `SetCommTimeouts` failed.
 */
lh_bool_t
lh_os_system_serial_setup(lh_os_system_win_handle_t port, lh_u32_t baud,
                          lh_os_serial_parity_t parity, lh_u8_t stop_bits);

#endif /* LH_SRC_OS_SYSTEM_WIN_SERIAL_H */
