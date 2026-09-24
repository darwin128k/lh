/**
 * @file kind.h
 * @brief What (if anything) a path is rooted at (::lh_fs_path_root_kind_t).
 *
 * A path's root is not a segment — it is not a name you'd ever see in a
 * directory listing, and unlike a segment it never needs quoting/escaping
 * rules of its own. Keeping it out of the segment list means every
 * operation on segments (::lh_str_list_t) never has to special-case
 * "is this segment secretly the root".
 */

#ifndef LH_FS_PATH_ROOT_KIND_H
#define LH_FS_PATH_ROOT_KIND_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_fs_path_root_kind_t
 * @brief Discriminator for a path's root.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_fs_path_root_kind_*`
 * constants below.
 */
typedef lh_u8_t lh_fs_path_root_kind_t;

/**
 * @def lh_fs_path_root_kind_none
 * @brief Relative path: no root at all.
 */
#define lh_fs_path_root_kind_none 0U

/**
 * @def lh_fs_path_root_kind_posix
 * @brief POSIX-absolute: rooted at `/`. No drive letter.
 */
#define lh_fs_path_root_kind_posix 1U

/**
 * @def lh_fs_path_root_kind_drive
 * @brief Windows drive-absolute: rooted at a drive letter (`C:`).
 */
#define lh_fs_path_root_kind_drive 2U

#endif /* LH_FS_PATH_ROOT_KIND_H */
