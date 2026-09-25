/**
 * @file stat.h
 * @brief Path queries that fill an ::lh_fs_stat_t (::lh_os_fs_stat) or read
 *        one of its fields (mtime, owner permissions).
 *
 * The snapshot itself is a pure value in `lh/fs/stat.h`; this header is
 * the OS side that fills it from a path (`lstat` /
 * `GetFileAttributesEx`) — no open, no handle, just the name.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_STAT_H
#define LH_OS_FS_STAT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/path.h>
#include <lh/fs/stat.h>
#include <lh/fs/time.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/stat.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Unix-shaped snapshot of @p path into @p out (`lstat`). No handle,
 *        no open — just the name.
 *
 * Kind is one value: symlink, else directory, else file, else other.
 * Extra Windows flags are in ::lh_fs_stat_get_attr. A name that starts
 * with `.` (not `.` / `..`) sets ::lh_fs_attr_hidden.
 *
 * On failure the reason is in ::lh_os_last_error (our own checks, e.g. an
 * empty path) or ::lh_os_system_last_error (the native call failed) — see
 * `lh/os.h` / `lh/os/system.h`.
 */
lh_bool_t
lh_os_fs_stat(const lh_fs_path_t *path, lh_fs_stat_t *out);

/**
 * @brief Last-write time of @p path as Unix seconds.
 *
 * Delegates to ::lh_os_fs_stat.
 */
lh_bool_t
lh_os_fs_mtime(const lh_fs_path_t *path, lh_fs_time_t *out);

/**
 * @brief Owner-read permission of @p path (::lh_fs_perm_is_readable).
 *
 * Delegates to ::lh_os_fs_stat; @p out is only written on success.
 *
 * @return ::lh_bool_true if @p path could be stat'd, ::lh_bool_false if the
 *         stat itself failed (see ::lh_os_fs_stat for where the reason is).
 */
lh_bool_t
lh_os_fs_is_readable(const lh_fs_path_t *path, lh_bool_t *out);

/**
 * @brief Owner-write permission of @p path (::lh_fs_perm_is_writable).
 *
 * Delegates to ::lh_os_fs_stat; @p out is only written on success.
 *
 * @return ::lh_bool_true if @p path could be stat'd, ::lh_bool_false if the
 *         stat itself failed (see ::lh_os_fs_stat for where the reason is).
 */
lh_bool_t
lh_os_fs_is_writable(const lh_fs_path_t *path, lh_bool_t *out);

/**
 * @brief Owner-execute permission of @p path (::lh_fs_perm_is_executable).
 *
 * Delegates to ::lh_os_fs_stat; @p out is only written on success.
 *
 * @return ::lh_bool_true if @p path could be stat'd, ::lh_bool_false if the
 *         stat itself failed (see ::lh_os_fs_stat for where the reason is).
 */
lh_bool_t
lh_os_fs_is_executable(const lh_fs_path_t *path, lh_bool_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_STAT_H */
