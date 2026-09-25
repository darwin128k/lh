/**
 * @file net.h
 * @brief Kernel process-wide networking setup (Winsock startup on Windows;
 *        nothing on POSIX).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_NET_H
#define LH_OS_SYSTEM_NET_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/net.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Prepare the native socket stack for use in this process.
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_system_net_init(void);

/**
 * @brief Undo one successful ::lh_os_system_net_init.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_system_net_deinit(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_NET_H */
