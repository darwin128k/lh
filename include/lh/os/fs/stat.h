/**
 * @file stat.h
 * @brief Snapshot of a filesystem object's state (::lh_os_fs_stat_t).
 *
 * Unix `struct stat` is the model: kind (from `S_IFMT`), permission bits
 * (::lh_os_fs_perm_t), size, and atime/mtime/ctime as Unix seconds.
 * Windows `GetFileAttributesEx` / `GetFileInformationByHandle` is mapped
 * onto that. Extra Windows flags live in ::lh_os_fs_attr_t, not in perm.
 *
 * This is a value, not an open handle. Fill it from a path
 * (::lh_os_fs_path_stat, `lstat`) or from an open file
 * (::lh_os_fs_file_stat, `fstat`).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_STAT_H
#define LH_OS_FS_STAT_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/fs/attr.h>
#include <lh/os/fs/kind.h>
#include <lh/os/fs/perm.h>
#include <lh/os/fs/stat/fields.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/stat.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_stat
 * @brief One filesystem object's Unix-shaped state.
 */
struct lh_os_fs_stat
{
    lh_os_fs_stat_fields(lh_os_fs_kind_t, lh_os_fs_perm_t, lh_u64_t, lh_s64_t, lh_os_fs_attr_t);
};

/**
 * @typedef lh_os_fs_stat_t
 * @brief Alias for `struct lh_os_fs_stat`.
 */
typedef struct lh_os_fs_stat lh_os_fs_stat_t;

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
lh_u64_t
lh_os_fs_stat_get_size(const lh_os_fs_stat_t *self);

LH_ATTRIBUTE_SYMBOL
lh_s64_t
lh_os_fs_stat_get_atime(const lh_os_fs_stat_t *self);

LH_ATTRIBUTE_SYMBOL
lh_s64_t
lh_os_fs_stat_get_mtime(const lh_os_fs_stat_t *self);

/**
 * @brief Inode-change time on Unix; creation time on Windows.
 */
LH_ATTRIBUTE_SYMBOL
lh_s64_t
lh_os_fs_stat_get_ctime(const lh_os_fs_stat_t *self);

LH_ATTRIBUTE_SYMBOL
lh_os_fs_attr_t
lh_os_fs_stat_get_attr(const lh_os_fs_stat_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_STAT_H */
