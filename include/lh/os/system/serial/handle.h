/**
 * @file handle.h
 * @brief Raw serial port handle and its "no port" sentinel.
 *
 * A Windows `HANDLE` from `CreateFileW` and a POSIX file descriptor, stored by bit
 * pattern in a pointer-width signed integer, as the socket handle is: both
 * sentinels (`INVALID_HANDLE_VALUE`, `-1`) are then the same `-1`.
 */

#ifndef LH_OS_SYSTEM_SERIAL_HANDLE_H
#define LH_OS_SYSTEM_SERIAL_HANDLE_H

#include <lh/cast/static.h>
#include <lh/size.h>

/**
 * @typedef lh_os_system_serial_handle_t
 * @brief Raw serial port handle, stored by bit pattern.
 */
typedef lh_ssize_t lh_os_system_serial_handle_t;

/**
 * @def LH_OS_SYSTEM_SERIAL_HANDLE_INVALID
 * @brief Sentinel for "no port".
 */
#define LH_OS_SYSTEM_SERIAL_HANDLE_INVALID (lh_cast_static(lh_os_system_serial_handle_t, -1))

#endif /* LH_OS_SYSTEM_SERIAL_HANDLE_H */
