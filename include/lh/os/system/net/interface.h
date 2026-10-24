/**
 * @file interface.h
 * @brief Kernel: IPv4 addresses of the interfaces (`getifaddrs` / `GetIpAddrTable`).
 *
 * The backend is picked by CMake, not by `#if`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_NET_INTERFACE_H
#define LH_OS_SYSTEM_NET_INTERFACE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/net/interface.h>
#include <lh/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/net/interface.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Call @p fn (non-null) for every IPv4 address of the interfaces.
 * @return ::lh_bool_false with the error captured when the list cannot be read.
 */
lh_bool_t
lh_os_system_net_interface_each_ip4(lh_os_net_interface_ip4_fn *fn, lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_NET_INTERFACE_H */
