/**
 * @file access.h
 * @brief Access kind for ::lh_os_fs_is / ::lh_os_fs_file_is.
 *
 * Not Unix perm bits and not ::lh_os_fs_kind_t. Answers whether *this*
 * process can read, write, or execute a path (PHP `is_readable` /
 * `is_writable` / `is_executable`).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_ACCESS_H
#define LH_OS_FS_ACCESS_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/fs/path.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/access.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @typedef lh_os_fs_access_t
 * @brief Which capability ::lh_os_fs_is asks about.
 */
typedef lh_u8_t lh_os_fs_access_t;

/**
 * @def lh_os_fs_access_readable
 * @brief This process can read the path.
 */
#define lh_os_fs_access_readable 0U

/**
 * @def lh_os_fs_access_writable
 * @brief This process can write the path.
 */
#define lh_os_fs_access_writable 1U

/**
 * @def lh_os_fs_access_executable
 * @brief This process can execute the path.
 */
#define lh_os_fs_access_executable 2U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when this process has @p access on @p path.
 *
 * Builds a temporary ::lh_os_fs_file_t for read/write probes. Empty or
 * missing path is false (reason in ::lh_os_get_last_error).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_is(const lh_os_fs_path_t *path, lh_os_fs_access_t access);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_ACCESS_H */
