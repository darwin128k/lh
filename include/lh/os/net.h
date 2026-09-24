/**
 * @file net.h
 * @brief Process-wide setup/teardown for the OS networking backend.
 *
 * Windows needs `WSAStartup`/`WSACleanup` bracketing every socket call in the
 * process; POSIX needs nothing. Call ::lh_os_net_init once before opening any
 * ::lh_os_net_socket_t, and ::lh_os_net_deinit once when done — on POSIX both
 * are (cheap) no-ops, so the call is unconditionally safe to make everywhere.
 */

#ifndef LH_OS_NET_H
#define LH_OS_NET_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/net.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Bring up the OS networking backend for this process.
 *
 * Must succeed before any ::lh_os_net_socket_open call. Safe to call more
 * than once; safe to call on every platform.
 *
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS backend
 *         failed to initialize.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_net_init(void);

/**
 * @brief Tear down the OS networking backend for this process.
 *
 * Call once after every ::lh_os_net_socket_t has been closed. Safe to call
 * on every platform.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_net_deinit(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_NET_H */
