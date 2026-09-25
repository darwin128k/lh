/**
 * @file size.h
 * @brief Filesystem object size representation (::lh_fs_size_t).
 *
 * Bytes, matching `off_t`/`stat.st_size`'s usual meaning on POSIX and
 * `nFileSizeHigh`/`nFileSizeLow` combined on Windows.
 *
 * Purely a documentation typedef (alias for ::lh_u64_t, not a distinct
 * type) — it exists so `lh_fs_stat_t`'s size field and its accessor
 * read as "a filesystem object's size", not "some u64".
 */

#ifndef LH_FS_SIZE_H
#define LH_FS_SIZE_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_fs_size_t
 * @brief Size in bytes. Alias for ::lh_u64_t.
 */
typedef lh_u64_t lh_fs_size_t;

#endif /* LH_FS_SIZE_H */
