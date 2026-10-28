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
struct lh_fs_stat
{
    /** Members: ::lh_fs_stat_fields. */
    lh_fs_stat_fields(lh_fs_kind_t, lh_fs_perm_t, lh_fs_size_t, lh_fs_time_t, lh_fs_attr_t);
};
/** @copybrief lh_fs_stat */
typedef struct lh_fs_stat lh_fs_stat_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Copy @p other into @p self.
 */
lh_void
lh_fs_stat_assign(lh_fs_stat_t *self, const lh_fs_stat_t *other);

/**
 * @brief What the entry is: file, directory, link or other.
 * @param self Stat to read.
 * @return The ::lh_fs_kind_t.
 */
lh_fs_kind_t
lh_fs_stat_get_kind(const lh_fs_stat_t *self);

/**
 * @brief Permission bits of the entry.
 * @param self Stat to read.
 * @return The ::lh_fs_perm_t mask.
 */
lh_fs_perm_t
lh_fs_stat_get_perm(const lh_fs_stat_t *self);

/**
 * @brief Size of the entry in bytes.
 * @param self Stat to read.
 * @return The size.
 */
lh_fs_size_t
lh_fs_stat_get_size(const lh_fs_stat_t *self);

/**
 * @brief Last access time.
 * @param self Stat to read.
 * @return Unix seconds (::lh_fs_time_t).
 */
lh_fs_time_t
lh_fs_stat_get_atime(const lh_fs_stat_t *self);

/**
 * @brief Last modification time.
 * @param self Stat to read.
 * @return Unix seconds (::lh_fs_time_t).
 */
lh_fs_time_t
lh_fs_stat_get_mtime(const lh_fs_stat_t *self);

/**
 * @brief Inode-change time on Unix; creation time on Windows.
 */
lh_fs_time_t
lh_fs_stat_get_ctime(const lh_fs_stat_t *self);

/**
 * @brief Extra attributes Unix `st_mode` does not carry (hidden, ...).
 * @param self Stat to read.
 * @return The ::lh_fs_attr_t mask.
 */
lh_fs_attr_t
lh_fs_stat_get_attr(const lh_fs_stat_t *self);

/**
 * @brief Overwrite the extra flags of @p self. Other fields are untouched.
 */
lh_void
lh_fs_stat_set_attr(lh_fs_stat_t *self, lh_fs_attr_t attr);

/** @brief Store what the entry is (file, directory, link, ...). Level-1 setter: no validation. */
lh_void
lh_fs_stat_set_kind(lh_fs_stat_t *self, lh_fs_kind_t kind);

/** @brief Store the permission bits. Level-1 setter: no validation. */
lh_void
lh_fs_stat_set_perm(lh_fs_stat_t *self, lh_fs_perm_t perm);

/** @brief Store the size in bytes. Level-1 setter: no validation. */
lh_void
lh_fs_stat_set_size(lh_fs_stat_t *self, lh_fs_size_t size);

/** @brief Store the last access time. Level-1 setter: no validation. */
lh_void
lh_fs_stat_set_atime(lh_fs_stat_t *self, lh_fs_time_t atime);

/** @brief Store the last modification time. Level-1 setter: no validation. */
lh_void
lh_fs_stat_set_mtime(lh_fs_stat_t *self, lh_fs_time_t mtime);

/** @brief Store the status change time (creation time on Windows). Level-1 setter: no validation. */
lh_void
lh_fs_stat_set_ctime(lh_fs_stat_t *self, lh_fs_time_t ctime);

/**
 * @brief Overwrite every field of @p self.
 */
lh_void
lh_fs_stat_set(lh_fs_stat_t *self, lh_fs_kind_t kind, lh_fs_perm_t perm, lh_fs_size_t size,
               lh_fs_time_t atime, lh_fs_time_t mtime, lh_fs_time_t ctime, lh_fs_attr_t attr);

/**
 * @brief Zero @p self: kind `other`, empty perm/attr, times and size `0`.
 */
lh_void
lh_fs_stat_init(lh_fs_stat_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_FS_STAT_H */
