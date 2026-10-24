/**
 * @file interface.h
 * @brief The machine's own addresses: ::lh_os_net_interface_each_ip4.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_NET_INTERFACE_H
#define LH_OS_NET_INTERFACE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/net/ip.h>
#include <lh/ptr.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/net/interface.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_net_interface_ip4_fn
 * @brief Called once per IPv4 address with the caller's @p user pointer.
 */
typedef lh_void(lh_os_net_interface_ip4_fn)(lh_ptr user, const lh_net_ip4_t *ip);

/**
 * @brief Call @p fn for every IPv4 address on this machine's interfaces.
 *
 * Loopback and aliases included — filtering is the caller's job.
 *
 * @return ::lh_bool_true when the list was read (even if empty);
 *         ::lh_bool_false with the OS error set, or for a null @p fn.
 */
lh_bool_t
lh_os_net_interface_each_ip4(lh_os_net_interface_ip4_fn *fn, lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_NET_INTERFACE_H */
