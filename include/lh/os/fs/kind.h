/**
 * @file kind.h
 * @brief What a filesystem path or directory entry is.
 *
 * Shared by ::lh_os_fs_path_is and ::lh_os_fs_dir_read. File, directory,
 * symlink, and shortcut may overlap (a `.lnk` is also a file; a symlink to
 * a directory is also a directory). `other` is for listing when the OS did
 * not say (`DT_UNKNOWN`) — ::lh_os_fs_path_is with `other` is false.
 */

#ifndef LH_OS_FS_KIND_H
#define LH_OS_FS_KIND_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_os_fs_kind_t
 * @brief Discriminator for a path or directory entry.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_os_fs_kind_*` constants
 * below.
 */
typedef lh_u8_t lh_os_fs_kind_t;

/**
 * @def lh_os_fs_kind_file
 * @brief Regular file (`FILE_ATTRIBUTE_DIRECTORY` clear / `S_ISREG` / `DT_REG`).
 */
#define lh_os_fs_kind_file 0U

/**
 * @def lh_os_fs_kind_dir
 * @brief Directory (`FILE_ATTRIBUTE_DIRECTORY` / `S_ISDIR` / `DT_DIR`).
 */
#define lh_os_fs_kind_dir 1U

/**
 * @def lh_os_fs_kind_symlink
 * @brief Symbolic link (`IO_REPARSE_TAG_SYMLINK` / `S_ISLNK` / `DT_LNK`).
 *        Not a junction, not a `.lnk` shortcut.
 */
#define lh_os_fs_kind_symlink 2U

/**
 * @def lh_os_fs_kind_shortcut
 * @brief Windows Shell Link (`.lnk`) file. Path query only — directory
 *        listing still reports these as ::lh_os_fs_kind_file.
 */
#define lh_os_fs_kind_shortcut 3U

/**
 * @def lh_os_fs_kind_other
 * @brief Neither file, directory, nor symlink (or the OS did not say).
 */
#define lh_os_fs_kind_other 4U

#endif /* LH_OS_FS_KIND_H */
