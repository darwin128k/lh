/**
 * @file env.h
 * @brief Process environment: ::lh_os_env_get.
 *
 * Reads one variable of the running process's environment. Windows and POSIX
 * both go through `getenv` in the backend (`lh/os/system/env.h`), so the value
 * is a pointer into the process environment, not a copy.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_ENV_H
#define LH_OS_ENV_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/env.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Value of the environment variable @p name of this process.
 *
 * @param name NUL-terminated variable name.
 * @return NUL-terminated value, or ::lh_null if the variable is not set (or
 *         @p name is ::lh_null). Read-only; valid until the environment is
 *         next modified. Do not free it.
 */
lh_str_cptr
lh_os_env_get(lh_str_cptr name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_ENV_H */