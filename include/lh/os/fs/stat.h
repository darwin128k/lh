/**
 * @file stat.h
 * @brief Snapshot of a filesystem object's state (::lh_os_fs_stat_t).
 *
 * Unix `struct stat` is the model: kind (from `S_IFMT`), permission bits
 * (::lh_os_fs_perm_t), size, and atime/mtime/ctime as Unix seconds.
 * Windows `GetFileAttributesEx` is mapped onto that. Extra Windows flags
 * live in ::lh_os_fs_attr_t, not in perm.
 *
 * This is a value, not an open handle. Fill it from a path
 * (::lh_os_fs_stat, `lstat` / `GetFileAttributesEx`) — no open, no
 * handle, just the name.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_STAT_H
#define LH_OS_FS_STAT_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/path.h>
#include <lh/os/fs/attr.h>
#include <lh/os/fs/kind.h>
#include <lh/os/fs/perm.h>
#include <lh/os/fs/size.h>
#include <lh/os/fs/stat/fields.h>
#include <lh/os/fs/time.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/stat.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_stat
 * @brief One filesystem object's Unix-shaped state. Fields via
 *        ::lh_os_fs_stat_fields.
 */
typedef struct lh_os_fs_stat
{
    lh_os_fs_stat_fields(lh_os_fs_kind_t, lh_os_fs_perm_t, lh_os_fs_size_t, lh_os_fs_time_t,
                        lh_os_fs_attr_t);
} lh_os_fs_stat_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Zero @p self: kind `other`, empty perm/attr, times and size `0`.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_stat_init(lh_os_fs_stat_t *self);

/**
 * @brief Copy @p other into @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_stat_assign(lh_os_fs_stat_t *self, const lh_os_fs_stat_t *other);

LH_ATTRIBUTE_SYMBOL
lh_os_fs_kind_t
lh_os_fs_stat_get_kind(const lh_os_fs_stat_t *self);

LH_ATTRIBUTE_SYMBOL
lh_os_fs_perm_t
lh_os_fs_stat_get_perm(const lh_os_fs_stat_t *self);

LH_ATTRIBUTE_SYMBOL
lh_os_fs_size_t
lh_os_fs_stat_get_size(const lh_os_fs_stat_t *self);

LH_ATTRIBUTE_SYMBOL
lh_os_fs_time_t
lh_os_fs_stat_get_atime(const lh_os_fs_stat_t *self);

LH_ATTRIBUTE_SYMBOL
lh_os_fs_time_t
lh_os_fs_stat_get_mtime(const lh_os_fs_stat_t *self);

/**
 * @brief Inode-change time on Unix; creation time on Windows.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_fs_time_t
lh_os_fs_stat_get_ctime(const lh_os_fs_stat_t *self);

LH_ATTRIBUTE_SYMBOL
lh_os_fs_attr_t
lh_os_fs_stat_get_attr(const lh_os_fs_stat_t *self);

/**
 * @brief Unix-shaped snapshot of @p path into @p out (`lstat`). No handle,
 *        no open — just the name.
 *
 * Kind is one value: symlink, else directory, else file, else other.
 * Extra Windows flags are in ::lh_os_fs_stat_get_attr. A name that starts
 * with `.` (not `.` / `..`) sets ::lh_os_fs_attr_hidden.
 *
 * On failure the reason is in ::lh_os_last_error (our own checks, e.g. an
 * empty path) or ::lh_os_system_last_error (the native call failed) — see
 * `lh/os.h` / `lh/os/system.h`.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_stat(const lh_fs_path_t *path, lh_os_fs_stat_t *out);

/**
 * @brief Last-write time of @p path as Unix seconds.
 *
 * Delegates to ::lh_os_fs_stat.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_mtime(const lh_fs_path_t *path, lh_os_fs_time_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_STAT_H */
