/**
 * @file stat.h
 * @brief Snapshot of a filesystem object's state (::lh_fs_stat_t).
 *
 * Unix `struct stat` is the model: kind (from `S_IFMT`), permission bits
 * (::lh_fs_perm_t), size, and atime/mtime/ctime as Unix seconds.
 * Windows `GetFileAttributesEx` is mapped onto that. Extra Windows flags
 * live in ::lh_fs_attr_t, not in perm.
 *
 * Pure value type — no OS. Filled from a path by ::lh_os_fs_stat
 * (`lh/os/fs/stat.h`); built and usable without ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_FS_STAT_H
#define LH_FS_STAT_H

#include <lh/compiler/extern/c.h>
#include <lh/fs/attr.h>
#include <lh/fs/kind.h>
#include <lh/fs/perm.h>
#include <lh/fs/size.h>
#include <lh/fs/stat/fields.h>
#include <lh/fs/time.h>

/**
 * @struct lh_fs_stat
 * @brief One filesystem object's Unix-shaped state. Fields via
 *        ::lh_fs_stat_fields.
 */
typedef struct lh_fs_stat
{
    lh_fs_stat_fields(lh_fs_kind_t, lh_fs_perm_t, lh_fs_size_t, lh_fs_time_t, lh_fs_attr_t);
} lh_fs_stat_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Zero @p self: kind `other`, empty perm/attr, times and size `0`.
 */
void
lh_fs_stat_init(lh_fs_stat_t *self);

/**
 * @brief Copy @p other into @p self.
 */
void
lh_fs_stat_assign(lh_fs_stat_t *self, const lh_fs_stat_t *other);

lh_fs_kind_t
lh_fs_stat_get_kind(const lh_fs_stat_t *self);

lh_fs_perm_t
lh_fs_stat_get_perm(const lh_fs_stat_t *self);

lh_fs_size_t
lh_fs_stat_get_size(const lh_fs_stat_t *self);

lh_fs_time_t
lh_fs_stat_get_atime(const lh_fs_stat_t *self);

lh_fs_time_t
lh_fs_stat_get_mtime(const lh_fs_stat_t *self);

/**
 * @brief Inode-change time on Unix; creation time on Windows.
 */
lh_fs_time_t
lh_fs_stat_get_ctime(const lh_fs_stat_t *self);

lh_fs_attr_t
lh_fs_stat_get_attr(const lh_fs_stat_t *self);

/**
 * @brief Overwrite every field of @p self.
 */
void
lh_fs_stat_set(lh_fs_stat_t *self, lh_fs_kind_t kind, lh_fs_perm_t perm, lh_fs_size_t size,
               lh_fs_time_t atime, lh_fs_time_t mtime, lh_fs_time_t ctime, lh_fs_attr_t attr);

/**
 * @brief Overwrite the extra flags of @p self. Other fields are untouched.
 */
void
lh_fs_stat_set_attr(lh_fs_stat_t *self, lh_fs_attr_t attr);

LH_COMPILER_EXTERN_C_END

#endif /* LH_FS_STAT_H */
