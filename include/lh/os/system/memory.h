/**
 * @file memory.h
 * @brief Kernel: page protection (`mprotect` / `VirtualProtect`).
 *
 * The backend is picked by CMake, not by `#if`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_MEMORY_H
#define LH_OS_SYSTEM_MEMORY_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/memory/access.h>
#include <lh/ptr.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/memory.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Set @p access on the pages covering [@p address, @p address + @p size).
 *
 * @p address is non-null and @p size is non-zero.
 * @return ::lh_bool_true on success; ::lh_bool_false with the error captured.
 */
lh_bool_t
lh_os_system_memory_protect(lh_ptr address, lh_usize_t size, lh_os_memory_access_t access);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_MEMORY_H */
