/**
 * @file kind.h
 * @brief What a native OS error code means, asked the same way on every OS.
 *
 * ::lh_os_system_error_code_t stays the OS's own number (see
 * `lh/os/system/error/code.h`): `ERROR_PATH_NOT_FOUND` and `ENOENT` are
 * different values and are never translated into one shared code. Instead,
 * a caller that must react to a class of failure asks a predicate here, and
 * the backend (picked by CMake, not `#if`) knows which native codes belong
 * to it.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_ERROR_KIND_H
#define LH_OS_SYSTEM_ERROR_KIND_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/system/error/code.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/error/kind.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when @p code says nothing exists at the path.
 *
 * Windows: `ERROR_FILE_NOT_FOUND`, `ERROR_PATH_NOT_FOUND`.
 * POSIX: `ENOENT`.
 */
lh_bool_t
lh_os_system_error_code_is_not_found(lh_os_system_error_code_t code);

/**
 * @brief True when @p code says the path (or a component of it) is not a
 *        directory where one was required.
 *
 * Windows: `ERROR_DIRECTORY`. POSIX: `ENOTDIR`.
 */
lh_bool_t
lh_os_system_error_code_is_not_dir(lh_os_system_error_code_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_ERROR_KIND_H */
