/**
 * @file file.h
 * @brief A filesystem file (::lh_os_fs_file_t): path plus handle.
 *
 * A name and an OS handle. One ::lh_os_fs_file_open for all access modes
 * (::lh_os_fs_file_mode_t). Not a directory listing, not a mapping, not a
 * stat snapshot, not an ::lh_io_stream_t — ::lh_os_fs_file_read talks to
 * the OS; ::lh_os_fs_file_get_reader plugs that into a stream half.
 * Close drops the handle and keeps the path; ::lh_os_fs_file_deinit
 * releases both.
 *
 * On failure the reason is in ::lh_os_get_last_error (see `lh/os.h`).
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_FILE_H
#define LH_OS_FS_FILE_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/io/reader.h>
#include <lh/os/fs/file/fields.h>
#include <lh/os/fs/file/handle.h>
#include <lh/os/fs/file/mode.h>
#include <lh/os/fs/path.h>
#include <lh/ptr.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/file.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_file
 * @brief Path plus handle. Fields via ::lh_os_fs_file_fields.
 */
struct lh_os_fs_file
{
    lh_os_fs_file_fields(lh_os_fs_path_t, lh_os_fs_file_handle_t);
};

/**
 * @typedef lh_os_fs_file_t
 * @brief Alias for `struct lh_os_fs_file`.
 */
typedef struct lh_os_fs_file lh_os_fs_file_t;

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
 * @brief Open @p path on @p self with the given access mode.
 *
 * Closes any previous handle first. Empty @p path is an error.
 *
 * @param self File object to open.
 * @param path Filesystem path (`CreateFileA` / `open`).
 * @param mode ::lh_os_fs_file_mode_read, ::lh_os_fs_file_mode_write, or
 *             ::lh_os_fs_file_mode_readwrite.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_open(lh_os_fs_file_t *self, const lh_os_fs_path_t *path, lh_os_fs_file_mode_t mode);

/**
 * @brief Drop the handle (if any). The stored path is kept.
 *
 * Safe to call on an already-closed file. Releases the OS handle when open.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_close(lh_os_fs_file_t *self);

/**
 * @brief Copy @p path into the stored path. The handle is not touched.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_set_path(lh_os_fs_file_t *self, const lh_os_fs_path_t *path);

/**
 * @brief Stored path of @p self, after validating the pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_fs_path_t *
lh_os_fs_file_get_path(lh_os_fs_file_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_file_get_path.
 */
LH_ATTRIBUTE_SYMBOL
const lh_os_fs_path_t *
lh_os_fs_file_get_path_as_const(const lh_os_fs_file_t *self);

/**
 * @brief Stored handle of @p self, after validating the pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_fs_file_handle_t *
lh_os_fs_file_get_handle(lh_os_fs_file_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_file_get_handle.
 */
LH_ATTRIBUTE_SYMBOL
const lh_os_fs_file_handle_t *
lh_os_fs_file_get_handle_as_const(const lh_os_fs_file_t *self);

/**
 * @brief True when @p self currently holds an open handle.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_is_valid(const lh_os_fs_file_t *self);

/**
 * @brief Read up to @p size bytes from @p self into @p buf.
 *
 * OS `ReadFile` / `read`. Not a directory listing. @p size `0` succeeds
 * without touching @p buf.
 *
 * @return Bytes read (`0` at end of file), or negative on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_fs_file_read(lh_os_fs_file_t *self, lh_ptr buf, lh_usize_t size);

/**
 * @brief An ::lh_io_reader_t that reads from @p self.
 *
 * @p self must outlive the returned reader. This is the stream plug:
 * ::lh_io_reader_read does not know a file is behind it.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_reader_t
lh_os_fs_file_get_reader(lh_os_fs_file_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_FILE_H */
