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

#include <lh/config.h>
#include <lh/fs/path/style.h>
#include <lh/str.h>

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

/**
 * @brief Resolve an existing path to absolute native text, following symbolic links.
 *
 * @param path NUL-terminated native path (UTF-8 on Windows); relative paths
 *             are resolved against the current working directory.
 * @param out Initialized string; replaced on success, unchanged on failure.
 *            May own the text passed as @p path. The caller retains ownership
 *            and must eventually ::lh_str_deinit it.
 * @return ::lh_bool_true on success; ::lh_bool_false for an empty, missing,
 *         inaccessible or unresolvable path. No lexical fallback is performed.
 *         Native failures are captured in ::lh_os_system_last_error;
 *         validation and backend limits in ::lh_os_last_error.
 *
 * Windows returns DOS/UNC text, without the extended-length prefix. Systems
 * without GetFinalPathNameByHandleW report ::lh_os_error_code_not_supported.
 * The result is a snapshot, not a guarantee against later filesystem changes.
 */
lh_bool_t
lh_os_system_fs_path_canonical(const lh_str_ptr path, lh_str_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_FS_PATH_H */
