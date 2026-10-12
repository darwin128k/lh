/**
 * @file serial.h
 * @brief Kernel serial port primitives on a raw ::lh_os_system_serial_handle_t.
 *
 * The only place that talks to `CreateFileW` / `SetCommState` (Windows) or
 * `open` / `termios` (POSIX) for a serial line. One contract on every platform;
 * the backend is picked by CMake.
 *
 * A port is opened **non-blocking for reading**: a read takes what has arrived and
 * answers ::LH_OS_RESULT_PENDING when nothing has, because a serial line has no
 * end -- a quiet line is not a closed one. Eight data bits always: that is what
 * Modbus RTU and nearly everything else on a line speaks.
 *
 * On failure the native reason is in ::lh_os_system_last_error.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_SERIAL_H
#define LH_OS_SYSTEM_SERIAL_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/result.h>
#include <lh/os/serial/parity.h>
#include <lh/os/system/serial/handle.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/serial.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Open the port @p name at @p baud bits per second, eight data bits.
 *
 * @param name      `COM3` (or `\\.\COM12`) on Windows, `/dev/ttyUSB0` on POSIX, UTF-8.
 * @param baud      Bits per second; a rate the platform has no setting for is
 *                  refused, not rounded.
 * @param parity    The parity bit.
 * @param stop_bits 1 or 2.
 *
 * @return Open handle, or ::LH_OS_SYSTEM_SERIAL_HANDLE_INVALID.
 */
lh_os_system_serial_handle_t
lh_os_system_serial_open(lh_str_cptr name, lh_u32_t baud, lh_os_serial_parity_t parity, lh_u8_t stop_bits);

/**
 * @brief Close @p handle. No-op when invalid.
 */
void
lh_os_system_serial_close(lh_os_system_serial_handle_t handle);

/**
 * @brief Read up to @p size bytes that have arrived, without waiting.
 *
 * @return Bytes read, ::LH_OS_RESULT_PENDING when none have arrived, or
 *         ::LH_OS_RESULT_INVALID on failure (the adapter was unplugged).
 */
lh_ssize_t
lh_os_system_serial_read(lh_os_system_serial_handle_t handle, lh_ptr buf, lh_usize_t size);

/**
 * @brief Write @p size bytes.
 *
 * The bytes are handed to the driver, which sends them at the line's rate. On
 * POSIX that is a kernel buffer and returns at once; Windows' serial driver
 * completes a write when it is on the wire, so there it takes the frame's
 * transmission time -- 11 bits a character, about 30 ms for a 260-byte frame at
 * 115200 baud. A master on a half-duplex line waits that long for its answer
 * either way.
 *
 * @return Bytes written, ::LH_OS_RESULT_PENDING when the driver could take none
 *         now, or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_system_serial_write(lh_os_system_serial_handle_t handle, const lh_ptr buf, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_SERIAL_H */
