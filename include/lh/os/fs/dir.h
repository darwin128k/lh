/**
 * @file dir.h
 * @brief A filesystem directory (::lh_os_fs_dir_t): listing cursor.
 *
 * Same ingredients as ::lh_os_fs_file_t (::lh_os_fs_file_fields) and a
 * different type. Open holds an OS iterator (`FindFirstFile` / `opendir`);
 * ::lh_os_fs_dir_read returns the next entry name (`.` and `..` skipped).
 *
 * On failure the reason is in ::lh_os_get_last_error (see `lh/os.h`).
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_DIR_H
#define LH_OS_FS_DIR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/fs/dir/entry/kind.h>
#include <lh/os/fs/file/fields.h>
#include <lh/os/fs/file/handle.h>
#include <lh/os/fs/file/mode.h>
#include <lh/os/fs/path.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/dir.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @def LH_OS_FS_DIR_NAME_MAX
 * @brief Longest entry name, excluding NUL (`259` matches Windows `MAX_PATH - 1`).
 */
#define LH_OS_FS_DIR_NAME_MAX 259U

/**
 * @struct lh_os_fs_dir
 * @brief Path plus listing handle. Fields via ::lh_os_fs_file_fields.
 */
struct lh_os_fs_dir
{
    lh_os_fs_file_fields(lh_os_fs_path_t, lh_os_fs_file_handle_t, lh_os_fs_file_mode_t);
};

/**
 * @typedef lh_os_fs_dir_t
 * @brief Alias for `struct lh_os_fs_dir`.
 */
typedef struct lh_os_fs_dir lh_os_fs_dir_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty path and invalid handle. Does not touch the OS.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_dir_init(lh_os_fs_dir_t *self);

/**
 * @brief Close the handle and release the stored path.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_dir_deinit(lh_os_fs_dir_t *self);

/**
 * @brief Open @p path as a directory listing on @p self.
 *
 * Closes any previous iterator first. Empty @p path is an error.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_dir_open(lh_os_fs_dir_t *self, const lh_os_fs_path_t *path);

/**
 * @brief Drop the listing handle (if any). The stored path is kept.
 *
 * Safe to call on an already-closed directory.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_dir_close(lh_os_fs_dir_t *self);

/**
 * @brief Copy @p path into the stored path. The handle is not touched.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_dir_set_path(lh_os_fs_dir_t *self, const lh_os_fs_path_t *path);

LH_ATTRIBUTE_SYMBOL
lh_os_fs_path_t *
lh_os_fs_dir_get_path(lh_os_fs_dir_t *self);

LH_ATTRIBUTE_SYMBOL
const lh_os_fs_path_t *
lh_os_fs_dir_get_path_as_const(const lh_os_fs_dir_t *self);

LH_ATTRIBUTE_SYMBOL
lh_os_fs_file_handle_t *
lh_os_fs_dir_get_handle(lh_os_fs_dir_t *self);

LH_ATTRIBUTE_SYMBOL
const lh_os_fs_file_handle_t *
lh_os_fs_dir_get_handle_as_const(const lh_os_fs_dir_t *self);

/**
 * @brief True when @p self currently holds an open listing.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_dir_is_valid(const lh_os_fs_dir_t *self);

/**
 * @brief Next entry name into @p name.
 *
 * Skips `.` and `..`. @p name is cleared when there are no more entries.
 * @p kind may be ::lh_null.
 *
 * @return Name length excluding NUL, `0` when there are no more entries,
 *         or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_fs_dir_read(lh_os_fs_dir_t *self, lh_os_fs_path_t *name, lh_os_fs_dir_entry_kind_t *kind);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_DIR_H */
