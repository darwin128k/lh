/**
 * @file path.h
 * @brief Kernel: the path text convention of the platform being built for.
 *
 * Every wrapper that hands an ::lh_fs_path_t to the OS renders it with
 * ::lh_os_system_fs_path_style_native. The backend is picked by CMake, not
 * by `#if`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_FS_PATH_H
#define LH_OS_SYSTEM_FS_PATH_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/path/style.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/fs/path.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief ::lh_fs_path_style_windows on Windows, ::lh_fs_path_style_posix
 *        elsewhere.
 */
lh_fs_path_style_t
lh_os_system_fs_path_style_native(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_FS_PATH_H */
