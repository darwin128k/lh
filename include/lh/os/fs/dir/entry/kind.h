/**
 * @file kind.h
 * @brief Directory-entry names for ::lh_fs_kind_t.
 *
 * Listing reports file / dir / symlink / other. A `.lnk` shortcut is still
 * a file in the listing — stat the joined path for
 * ::lh_fs_kind_shortcut if that distinction matters.
 */

#ifndef LH_OS_FS_DIR_ENTRY_KIND_H
#define LH_OS_FS_DIR_ENTRY_KIND_H

#include <lh/fs/kind.h>

/**
 * @typedef lh_os_fs_dir_entry_kind_t
 * @brief Same type as ::lh_fs_kind_t, for ::lh_os_fs_dir_read.
 */
typedef lh_fs_kind_t lh_os_fs_dir_entry_kind_t;

/**
 * @def lh_os_fs_dir_entry_kind_file
 * @brief ::lh_fs_kind_file
 */
#define lh_os_fs_dir_entry_kind_file lh_fs_kind_file

/**
 * @def lh_os_fs_dir_entry_kind_dir
 * @brief ::lh_fs_kind_dir
 */
#define lh_os_fs_dir_entry_kind_dir lh_fs_kind_dir

/**
 * @def lh_os_fs_dir_entry_kind_symlink
 * @brief ::lh_fs_kind_symlink
 */
#define lh_os_fs_dir_entry_kind_symlink lh_fs_kind_symlink

/**
 * @def lh_os_fs_dir_entry_kind_shortcut
 * @brief ::lh_fs_kind_shortcut (not emitted by ::lh_os_fs_dir_read)
 */
#define lh_os_fs_dir_entry_kind_shortcut lh_fs_kind_shortcut

/**
 * @def lh_os_fs_dir_entry_kind_other
 * @brief ::lh_fs_kind_other
 */
#define lh_os_fs_dir_entry_kind_other lh_fs_kind_other

#endif /* LH_OS_FS_DIR_ENTRY_KIND_H */
