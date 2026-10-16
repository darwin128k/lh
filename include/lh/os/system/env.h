/**
 * @file env.h
 * @brief Kernel: one variable of the process environment.
 *
 * The backend is picked by CMake, not by `#if`. Both backends are `getenv`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_ENV_H
#define LH_OS_SYSTEM_ENV_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/env.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Value of the environment variable @p name, or ::lh_null if unset.
 *
 * @p name is a non-null, NUL-terminated string.
 */
const lh_str_ptr
lh_os_system_env_get(const lh_str_ptr name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_ENV_H */
