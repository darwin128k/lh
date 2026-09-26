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

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
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

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether @p ch separates path segments in @p style.
 *
 * `/` separates in every style; `\` only in ::lh_fs_path_style_windows.
 *
 * Example usage:
 * @code{.c}
 * lh_fs_path_style_is_sep(lh_fs_path_style_windows, '\\'); // true
 * lh_fs_path_style_is_sep(lh_fs_path_style_posix, '\\');   // false
 * @endcode
 *
 * @param style Path text convention.
 * @param ch    Character to test.
 * @return ::lh_bool_true when @p ch is a separator in @p style.
 */
lh_bool_t
lh_fs_path_style_is_sep(lh_fs_path_style_t style, lh_char_t ch);

/**
 * @brief The separator @p style writes between segments: `\` for
 *        ::lh_fs_path_style_windows, `/` otherwise.
 *
 * @param style Path text convention.
 * @return Separator character rendered for @p style.
 */
lh_char_t
lh_fs_path_style_get_sep(lh_fs_path_style_t style);

LH_COMPILER_EXTERN_C_END

#endif /* LH_FS_PATH_STYLE_H */
