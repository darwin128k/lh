/**
 * @file dir.h
 * @brief Kernel directory iteration on a raw ::lh_os_system_fs_dir_handle_t.
 *
 * The only place that talks to `FindFirstFileA` / `FindNextFileA` /
 * `FindClose` (Windows) or `opendir` / `readdir` / `closedir` (POSIX).
 * The backend is picked by CMake, not by `#if`.
 *
 * Reports every native entry as is, `.` and `..` included — filtering and
 * name limits are the caller's policy (see ::lh_os_fs_dir_read). Takes
 * native text, not ::lh_fs_path_t.
 *
 * On failure the reason is in ::lh_os_system_last_error (the native call
 * failed) or ::lh_os_last_error (out of memory for the iterator state).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_FS_DIR_H
#define LH_OS_SYSTEM_FS_DIR_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/kind.h>
#include <lh/os/system/fs/dir/handle.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/fs/dir.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Start listing the directory at @p path.
 *
 * @param path Native, NUL-terminated directory path text.
 * @return Open handle, or ::LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_system_fs_dir_handle_t
lh_os_system_fs_dir_open(lh_str_cptr path);

/**
 * @brief Stop listing and release @p handle.
 *
 * @param handle Open handle; must not be ::LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_system_fs_dir_close(lh_os_system_fs_dir_handle_t handle);

/**
 * @brief Advance @p handle to its next entry.
 *
 * @param handle Open handle.
 * @param name   Receives the entry name (NUL-terminated); valid until the
 *               next read or close on @p handle.
 * @param kind   Receives the entry kind (file / dir / symlink / other).
 * @return Length of @p name (`> 0`) for an entry, `0` when the listing is
 *         exhausted, or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_system_fs_dir_read(lh_os_system_fs_dir_handle_t handle, lh_str_cptr *name, lh_fs_kind_t *kind);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_FS_DIR_H */
