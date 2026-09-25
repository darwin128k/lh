/**
 * @file handle.h
 * @brief Raw OS socket handle type and its "no socket" sentinel.
 *
 * A POSIX file descriptor (`int`, invalid = `-1`) and a Windows `SOCKET`
 * (`UINT_PTR`, invalid = `INVALID_SOCKET` = `~0`) are different types, but
 * both are just a bit pattern that fits in a pointer-width integer, and both
 * sentinels are that integer's all-ones pattern. Storing the handle as
 * ::lh_ssize_t (signed, pointer-width) makes one sentinel
 * (::LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID, `-1`) correct on every platform,
 * without this header (or anything that includes it) ever naming
 * `<winsock2.h>` or `<sys/socket.h>`.
 */

#ifndef LH_OS_SYSTEM_NET_SOCKET_HANDLE_H
#define LH_OS_SYSTEM_NET_SOCKET_HANDLE_H

#include <lh/cast/static.h>
#include <lh/size.h>

/**
 * @typedef lh_os_system_net_socket_handle_t
 * @brief Raw OS socket handle, stored by bit pattern.
 */
typedef lh_ssize_t lh_os_system_net_socket_handle_t;

/**
 * @def LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID
 * @brief Sentinel for "no socket" (matches POSIX `-1` and Windows
 *        `INVALID_SOCKET` once both are read back as this type).
 */
#define LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID (lh_cast_static(lh_os_system_net_socket_handle_t, -1))

#endif /* LH_OS_SYSTEM_NET_SOCKET_HANDLE_H */
