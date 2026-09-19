/**
 * @file path.h
 * @brief A filesystem path (::lh_os_fs_path_t): levels plus a separator.
 *
 * Not a string at the API: levels plus a separator. Internally one
 * ::lh_str_t buffer and offsets of each name (OpenJDK `UnixPath`). `set`
 * parses into that buffer; ::lh_os_fs_path_join appends a level; OS APIs
 * use ::lh_os_fs_path_get_text. Same encoding as `LoadLibraryA` / POSIX
 * bytes. No wide paths.
 *
 * On failure the reason is in ::lh_os_get_last_error (see `lh/os.h`).
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_PATH_H
#define LH_OS_FS_PATH_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/index.h>
#include <lh/os/fs/kind.h>
#include <lh/os/fs/path/fields.h>
#include <lh/os/fs/stat.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/str.h>
#include <lh/str/ptr.h>
#include <lh/str/view.h>
#include <lh/vector.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/path.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_path
 * @brief One filesystem path. Fields via ::lh_os_fs_path_fields.
 */
struct lh_os_fs_path
{
    lh_os_fs_path_fields(lh_char_t, lh_str_t, lh_vector_t);
};

/**
 * @typedef lh_os_fs_path_t
 * @brief Alias for `struct lh_os_fs_path`.
 */
typedef struct lh_os_fs_path lh_os_fs_path_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Preferred directory separator for this OS (`'\\'` on Windows,
 *        `'/'` elsewhere).
 *
 * Join glues with this character. Parse accepts `'/'` as well on Windows.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_os_fs_path_sep(void);

/**
 * @brief Empty path: no levels, empty buffer, OS separator stored.
 *
 * Call once on uninitialized storage. Reset an already-initialized path
 * with ::lh_os_fs_path_clear; free it with ::lh_os_fs_path_deinit.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_init(lh_os_fs_path_t *self);

/**
 * @brief Drop every level and empty the buffer. Keeps allocations.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_clear(lh_os_fs_path_t *self);

/**
 * @brief Release the buffer and the offset table.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_deinit(lh_os_fs_path_t *self);

/**
 * @brief Copy @p other into @p self (buffer and offsets).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_assign(lh_os_fs_path_t *self, const lh_os_fs_path_t *other);

/**
 * @brief Parse @p text into the buffer and name offsets.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_set(lh_os_fs_path_t *self, lh_str_view_t text);

/**
 * @brief Stored separator of @p self, after validating the pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t *
lh_os_fs_path_get_sep(lh_os_fs_path_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_path_get_sep.
 */
LH_ATTRIBUTE_SYMBOL
const lh_char_t *
lh_os_fs_path_get_sep_as_const(const lh_os_fs_path_t *self);

/**
 * @brief Offset table of @p self, after validating the pointer.
 *
 * Elements are `{offset, size}` into ::lh_os_fs_path_get_text. Other
 * functions go through this or ::lh_os_fs_path_get_parts_as_const instead
 * of `self->parts`.
 */
LH_ATTRIBUTE_SYMBOL
lh_vector_t *
lh_os_fs_path_get_parts(lh_os_fs_path_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_path_get_parts.
 */
LH_ATTRIBUTE_SYMBOL
const lh_vector_t *
lh_os_fs_path_get_parts_as_const(const lh_os_fs_path_t *self);

/**
 * @brief Level at @p index, as a view into the path buffer.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_view_t
lh_os_fs_path_get_part(const lh_os_fs_path_t *self, lh_uindex_t index);

/**
 * @brief Path buffer of @p self, after validating the pointer.
 *
 * Single access to `text`. OS wrappers use ::lh_str_get_data of this.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_t *
lh_os_fs_path_get_text(lh_os_fs_path_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_path_get_text.
 */
LH_ATTRIBUTE_SYMBOL
const lh_str_t *
lh_os_fs_path_get_text_as_const(const lh_os_fs_path_t *self);

/**
 * @brief View over the path buffer of @p self (does not include the NUL).
 */
LH_ATTRIBUTE_SYMBOL
lh_str_view_t
lh_os_fs_path_as_view(const lh_os_fs_path_t *self);

/**
 * @brief True when @p self has no levels.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_empty(const lh_os_fs_path_t *self);

/**
 * @brief Absolute path of the running executable into @p out.
 *
 * Windows: `GetModuleFileNameA` of the process. POSIX: `readlink` of
 * `/proc/self/exe`. macOS: `_NSGetExecutablePath`. Then parsed into levels.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_exe(lh_os_fs_path_t *out);

/**
 * @brief Directory containing the running executable, without a trailing
 *        separator except for a drive/root (`C:\\`, `/`).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_exe_dir(lh_os_fs_path_t *out);

/**
 * @brief Join @p dir and @p name into @p out by appending @p name's levels.
 *
 * Empty @p name is an error.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_join(lh_os_fs_path_t *out, const lh_os_fs_path_t *dir, const lh_os_fs_path_t *name);

/**
 * @brief Directory containing @p path. A single relative level becomes `.`.
 *        A drive or `/` is left as the root.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_dir(const lh_os_fs_path_t *path, lh_os_fs_path_t *out);

/**
 * @brief True when @p path matches @p kind.
 *
 * Kinds may overlap: a Shell Link is also a file; a symlink to a directory
 * is also a directory. ::lh_os_fs_kind_other is listing-only and is false
 * here. Missing path is false (reason in ::lh_os_get_last_error).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is(const lh_os_fs_path_t *path, lh_os_fs_kind_t kind);

/**
 * @brief True when @p path is a directory.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_dir(const lh_os_fs_path_t *path);

/**
 * @brief True when @p path is a regular file. A `.lnk` shortcut is a file.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_file(const lh_os_fs_path_t *path);

/**
 * @brief True when @p path itself is a symbolic link.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_symlink(const lh_os_fs_path_t *path);

/**
 * @brief True when @p path is a Windows Shell Link (`.lnk` shortcut).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_shortcut(const lh_os_fs_path_t *path);

/**
 * @brief Last-write time of @p path as Unix seconds.
 *
 * Delegates to ::lh_os_fs_path_stat (`lstat` / `GetFileAttributesEx`).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_mtime(const lh_os_fs_path_t *path, lh_s64_t *out);

/**
 * @brief Unix-shaped snapshot of @p path into @p out (`lstat`).
 *
 * Kind is one value: symlink, else directory, else file, else other.
 * Overlapping queries (symlink-to-dir is also a dir; `.lnk` is also a
 * file) stay on ::lh_os_fs_path_is. Extra Windows flags are in
 * ::lh_os_fs_stat_get_attr. A name that starts with `.` (not `.` / `..`)
 * sets ::lh_os_fs_attr_hidden.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_stat(const lh_os_fs_path_t *path, lh_os_fs_stat_t *out);

/**
 * @brief Read the whole file at @p path into @p buf (PHP `file_get_contents`).
 *
 * All-or-nothing: if the file is larger than @p buf_size, nothing is copied
 * and the call fails. Does not append a NUL — @p out_size is the byte count.
 * An empty file succeeds with @p out_size `0`.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_read(const lh_os_fs_path_t *path, lh_ptr buf, lh_usize_t buf_size,
                   lh_usize_t *out_size);

/**
 * @brief Delete the file at @p path. Directories are not removed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_remove(const lh_os_fs_path_t *path);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_PATH_H */
