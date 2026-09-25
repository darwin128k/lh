/**
 * @file style.h
 * @brief Which textual convention an ::lh_fs_path_t is parsed from or
 *        rendered to (::lh_fs_path_style_t).
 *
 * The path itself is style-free — a root plus segment names. Style only
 * matters at the text boundary: which characters separate segments, whether
 * `C:` is a drive, which separator to write. Passing it explicitly is what
 * lets a POSIX build read a Windows path (and the other way round).
 *
 * The style of the platform being built for is an OS fact, not a path
 * fact: see ::lh_os_system_fs_path_style_native (`lh/os/system/fs/path.h`).
 */

#ifndef LH_FS_PATH_STYLE_H
#define LH_FS_PATH_STYLE_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_fs_path_style_t
 * @brief Discriminator for the path text convention.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_fs_path_style_*`
 * constants below.
 */
typedef lh_u8_t lh_fs_path_style_t;

/**
 * @def lh_fs_path_style_posix
 * @brief `/` separates; no drives. Rendered with `/`.
 */
#define lh_fs_path_style_posix 0U

/**
 * @def lh_fs_path_style_windows
 * @brief `/` and `\` separate; a leading `X:` is a drive. Rendered with `\`.
 */
#define lh_fs_path_style_windows 1U

#endif /* LH_FS_PATH_STYLE_H */
