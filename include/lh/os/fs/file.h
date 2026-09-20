/**
 * @file file.h
 * @brief A filesystem file (::lh_os_fs_file_t): path plus handle.
 *
 * A name and an OS handle. Not a directory listing, not a mapping, not a
 * stat snapshot. Close drops the handle and keeps the path;
 * ::lh_os_fs_file_deinit releases both.
 *
 * On failure the reason is in ::lh_os_get_last_error (see `lh/os.h`).
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_FILE_H
#define LH_OS_FS_FILE_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/fs/file/fields.h>
#include <lh/os/fs/file/handle.h>
#include <lh/os/fs/file/mode.h>
#include <lh/os/fs/path.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/file.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_file
 * @brief Path plus handle. Fields via ::lh_os_fs_file_fields.
 */
typedef struct lh_os_fs_file
{
    lh_os_fs_file_fields(lh_os_fs_path_t, lh_os_fs_file_handle_t, lh_os_fs_file_mode_t);
} lh_os_fs_file_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty path and invalid handle. Does not touch the OS.
 *
 * Call once on uninitialized storage. Reset an already-initialized file
 * with ::lh_os_fs_file_close (handle only) or ::lh_os_fs_file_deinit.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_init(lh_os_fs_file_t *self);

/**
 * @brief Close the handle and release the stored path.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_deinit(lh_os_fs_file_t *self);

/**
 * @brief Drop the handle (if any). The stored path is kept.
 *
 * Safe to call on an already-closed file.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_close(lh_os_fs_file_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_FILE_H */
