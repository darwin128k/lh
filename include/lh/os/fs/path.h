/**
 * @file path.h
 * @brief A filesystem path (::lh_os_fs_path_t): root plus segments.
 *
 * Root (::lh_os_fs_path_root_kind_t + drive letter) and segments
 * (::lh_str_list_t of real names) are separate fields — a segment is never
 * asked "are you secretly the root". A name, not a disk probe: exists /
 * is-file / stat are not here.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_PATH_H
#define LH_OS_FS_PATH_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/fs/path/fields.h>
#include <lh/os/fs/path/root/kind.h>
#include <lh/size.h>
#include <lh/str/list.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/path.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_path
 * @brief Root plus segments. Fields via ::lh_os_fs_path_fields.
 */
typedef struct lh_os_fs_path
{
    lh_os_fs_path_fields(lh_os_fs_path_root_kind_t, lh_char_t, lh_str_list_t);
} lh_os_fs_path_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty, relative path (no root, no segments). Does not touch the OS.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_init(lh_os_fs_path_t *self);

/**
 * @brief Release everything owned by @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_deinit(lh_os_fs_path_t *self);

/**
 * @brief What (if anything) @p self is rooted at.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_fs_path_root_kind_t
lh_os_fs_path_get_root_kind(const lh_os_fs_path_t *self);

/**
 * @brief Drive letter of @p self; meaningful only when the root kind is
 *        ::lh_os_fs_path_root_kind_drive.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_os_fs_path_get_root_drive(const lh_os_fs_path_t *self);

/**
 * @brief Segments of @p self, after validating the pointer.
 *
 * Single access to `segments`.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_list_t *
lh_os_fs_path_get_segments(lh_os_fs_path_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_path_get_segments.
 */
LH_ATTRIBUTE_SYMBOL
const lh_str_list_t *
lh_os_fs_path_get_segments_as_const(const lh_os_fs_path_t *self);

/**
 * @brief Number of segments in @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_os_fs_path_get_segment_count(const lh_os_fs_path_t *self);

/**
 * @brief True when @p self has neither a root nor any segments.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_empty(const lh_os_fs_path_t *self);

/**
 * @brief True when @p self is rooted (POSIX `/` or a drive), as opposed to
 *        relative.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_absolute(const lh_os_fs_path_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_PATH_H */
