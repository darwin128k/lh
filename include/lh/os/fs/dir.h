/**
 * @file dir.h
 * @brief One directory's names: open, read, close.
 *
 * Not a recursive walk and not a glob. Each ::lh_os_fs_dir_read returns the
 * next entry's name (`.` and `..` are skipped) as a view into the
 * iterator's own storage — no copy, no allocation per entry. To keep a name
 * or make a full path, copy it out (e.g. ::lh_fs_path_set, then
 * ::lh_fs_path_join) before the next read.
 *
 * On failure the reason is in ::lh_os_last_error (our own checks) or
 * ::lh_os_system_last_error (the native call failed) — see `lh/os.h` /
 * `lh/os/system.h`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_DIR_H
#define LH_OS_FS_DIR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/path.h>
#include <lh/os/fs/dir/entry/kind.h>
#include <lh/os/system/fs/dir/handle.h>
#include <lh/size.h>
#include <lh/str/view.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/dir.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_dir
 * @typedef lh_os_fs_dir_t
 * @brief Open directory iterator.
 */
struct lh_os_fs_dir
{
    lh_os_system_fs_dir_handle_t handle;
};
typedef struct lh_os_fs_dir lh_os_fs_dir_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Set @p self to the empty (not-yet-open) state.
 *
 * Pure value initialization — does not touch the OS.
 *
 * @param self Directory object to initialize.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_dir_init(lh_os_fs_dir_t *self);

/**
 * @brief Open @p path as a directory on @p self.
 *
 * @param self Directory object; must be empty (::lh_os_fs_dir_init or freshly
 *             ::lh_os_fs_dir_close'd).
 * @param path Directory path. Empty is an error.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_dir_open(lh_os_fs_dir_t *self, const lh_fs_path_t *path);

/**
 * @brief Close @p self (if open) and return it to the empty state.
 *
 * Safe to call on an already-empty directory.
 *
 * @param self Directory object to close.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_dir_close(lh_os_fs_dir_t *self);

/**
 * @brief Raw handle stored in @p self.
 * @param self Directory to read from.
 * @return Current ::lh_os_system_fs_dir_handle_t
 *         (::LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID if not open).
 */
LH_ATTRIBUTE_SYMBOL
lh_os_system_fs_dir_handle_t
lh_os_fs_dir_get_handle(const lh_os_fs_dir_t *self);

/**
 * @brief True when @p self currently holds an open iterator.
 * @param self Directory to test.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_dir_is_valid(const lh_os_fs_dir_t *self);

/**
 * @brief Next entry name into @p name.
 *
 * Skips `.` and `..`.
 *
 * @param self Open directory.
 * @param name Receives a view of the entry name (one path component, no
 *             separators). Valid until the next ::lh_os_fs_dir_read or
 *             ::lh_os_fs_dir_close on @p self. Empty when there are no more
 *             entries.
 * @param kind Receives ::lh_os_fs_dir_entry_kind_file / `_dir` / `_symlink`
 *             / `_other`. May be ::lh_null.
 * @return Name length excluding NUL, `0` when there are no more entries,
 *         or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_fs_dir_read(lh_os_fs_dir_t *self, lh_str_view_t *name, lh_os_fs_dir_entry_kind_t *kind);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_DIR_H */
