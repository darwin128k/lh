/**
 * @file path.h
 * @brief Narrow filesystem paths: exe location, join, mtime, read, remove.
 *
 * Path text is a NUL-terminated ::lh_str_cptr, same encoding as
 * ::lh_os_shared_open (`LoadLibraryA` / POSIX bytes). One directory's
 * names are ::lh_os_fs_dir_t (`lh/os/fs/dir.h`) — not a recursive walk,
 * not a glob. No wide paths.
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
#include <lh/numeric/fixed/types.h>
#include <lh/os/fs/kind.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/path.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Preferred directory separator for this OS (`'\\'` on Windows,
 *        `'/'` elsewhere).
 *
 * Join accepts either slash on Windows. This is the character
 * ::lh_os_fs_path_join inserts when the left side has none.
 *
 * @return Directory separator character.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_os_fs_path_sep(void);

/**
 * @brief Absolute path of the running executable.
 *
 * Windows: `GetModuleFileNameA` of the process. POSIX: `readlink` of
 * `/proc/self/exe`. macOS: `_NSGetExecutablePath`.
 *
 * All-or-nothing: if @p out_size is too small, @p out is left with an
 * empty string and the call fails.
 *
 * @param out      Destination buffer (receives a NUL-terminated path).
 * @param out_size Capacity of @p out in characters, including NUL.
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_exe(lh_str_ptr out, lh_usize_t out_size);

/**
 * @brief Directory containing the running executable, without a trailing
 *        separator except for a drive/root (`C:\\`, `/`).
 *
 * Same OS query as ::lh_os_fs_path_exe, then the last path component is
 * dropped.
 *
 * @param out      Destination buffer (receives a NUL-terminated path).
 * @param out_size Capacity of @p out in characters, including NUL.
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_exe_dir(lh_str_ptr out, lh_usize_t out_size);

/**
 * @brief Join @p dir and @p name into @p out.
 *
 * Inserts ::lh_os_fs_path_sep between them only when @p dir is non-empty
 * and does not already end with a separator, and @p name does not begin
 * with one. All-or-nothing on overflow.
 *
 * @param out      Destination buffer.
 * @param out_size Capacity of @p out in characters, including NUL.
 * @param dir      Left side (directory). Empty means @p name alone.
 * @param name     Right side (file name). Empty is an error.
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_join(lh_str_ptr out, lh_usize_t out_size, lh_str_cptr dir, lh_str_cptr name);

/**
 * @brief Directory containing @p path, without a trailing separator except
 *        for a drive/root (`C:\\`, `/`).
 *
 * Same last-component drop as ::lh_os_fs_path_exe_dir, for any path (plugin
 * DLL, data file). A path with no separator becomes `"."`.
 *
 * @param path     Filesystem path. ::lh_null or empty is an error.
 * @param out      Destination buffer (receives a NUL-terminated path).
 * @param out_size Capacity of @p out in characters, including NUL.
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_dir(lh_str_cptr path, lh_str_ptr out, lh_usize_t out_size);

/**
 * @brief True when @p path matches @p kind.
 *
 * Kinds may overlap: a Shell Link is also a file; a symlink to a directory
 * is also a directory. ::lh_os_fs_kind_other is listing-only and is false
 * here. Missing path is false (reason in ::lh_os_get_last_error).
 *
 * @param path Filesystem path. ::lh_null or empty is false.
 * @param kind ::lh_os_fs_kind_file / `_dir` / `_symlink` / `_shortcut`.
 * @return ::lh_bool_true if @p path has that kind.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is(lh_str_cptr path, lh_os_fs_kind_t kind);

/**
 * @brief True when @p path is a directory.
 *
 * Same as ::lh_os_fs_path_is(@p path, ::lh_os_fs_kind_dir).
 *
 * @param path Filesystem path. ::lh_null or empty is false.
 * @return ::lh_bool_true if @p path is a directory.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_dir(lh_str_cptr path);

/**
 * @brief True when @p path is a regular file.
 *
 * Same as ::lh_os_fs_path_is(@p path, ::lh_os_fs_kind_file).
 * A `.lnk` shortcut is a file.
 *
 * @param path Filesystem path. ::lh_null or empty is false.
 * @return ::lh_bool_true if @p path is a regular file.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_file(lh_str_cptr path);

/**
 * @brief True when @p path itself is a symbolic link.
 *
 * Same as ::lh_os_fs_path_is(@p path, ::lh_os_fs_kind_symlink).
 *
 * @param path Filesystem path. ::lh_null or empty is false.
 * @return ::lh_bool_true if @p path is a symbolic link.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_symlink(lh_str_cptr path);

/**
 * @brief True when @p path is a Windows Shell Link (`.lnk` shortcut).
 *
 * Same as ::lh_os_fs_path_is(@p path, ::lh_os_fs_kind_shortcut).
 *
 * @param path Filesystem path. ::lh_null or empty is false.
 * @return ::lh_bool_true if @p path is a Shell Link file.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_shortcut(lh_str_cptr path);

/**
 * @brief Last-write time of @p path as Unix seconds.
 *
 * Windows: `GetFileAttributesExA`. POSIX: `stat`.
 *
 * @param path Filesystem path. ::lh_null or empty is an error.
 * @param out  Receives `mtime` (seconds since 1970-01-01 UTC).
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_mtime(lh_str_cptr path, lh_s64_t *out);

/**
 * @brief Read the whole file at @p path into @p buf (PHP `file_get_contents`).
 *
 * All-or-nothing: if the file is larger than @p buf_size, nothing is copied
 * and the call fails. Does not append a NUL — @p out_size is the byte count.
 * An empty file succeeds with @p out_size `0`.
 *
 * @param path     Filesystem path. ::lh_null or empty is an error.
 * @param buf      Destination buffer. Ignored when the file is empty.
 * @param buf_size Capacity of @p buf in bytes.
 * @param out_size Receives the number of bytes written.
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_read(lh_str_cptr path, lh_ptr buf, lh_usize_t buf_size, lh_usize_t *out_size);

/**
 * @brief Delete the file at @p path.
 *
 * Windows: `DeleteFileA`. POSIX: `unlink`. Directories are not removed.
 *
 * @param path Filesystem path. ::lh_null or empty is an error.
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_remove(lh_str_cptr path);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_PATH_H */
