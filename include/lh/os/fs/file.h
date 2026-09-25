/**
 * @file file.h
 * @brief A filesystem file (::lh_os_fs_file_t): path plus handle.
 *
 * A name and an OS handle. Not a directory listing, not a mapping, not a
 * stat snapshot. Close drops the handle and keeps the path;
 * ::lh_os_fs_file_deinit releases both.
 *
 * On failure the reason is in ::lh_os_last_error (our own checks, e.g. an
 * empty path or a bad mode) or ::lh_os_system_last_error (the native call
 * failed) — see `lh/os.h` / `lh/os/system.h`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_FILE_H
#define LH_OS_FS_FILE_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/path.h>
#include <lh/io/reader.h>
#include <lh/io/stream.h>
#include <lh/io/writer.h>
#include <lh/os/fs/file/fields.h>
#include <lh/os/system/fs/file/handle.h>
#include <lh/os/system/fs/file/mode.h>
#include <lh/ptr.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/file.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_file
 * @brief Path plus handle. Fields via ::lh_os_fs_file_fields.
 */
typedef struct lh_os_fs_file
{
    lh_os_fs_file_fields(lh_fs_path_t, lh_os_system_fs_file_handle_t, lh_os_system_fs_file_mode_t);
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
lh_os_fs_file_set_path(lh_os_fs_file_t *self, const lh_fs_path_t *path);

/**
 * @brief Stored path of @p self, after validating the pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_fs_path_t *
lh_os_fs_file_get_path(lh_os_fs_file_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_file_get_path.
 */
LH_ATTRIBUTE_SYMBOL
const lh_fs_path_t *
lh_os_fs_file_get_path_as_const(const lh_os_fs_file_t *self);

/**
 * @brief Stored handle of @p self, after validating the pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_system_fs_file_handle_t *
lh_os_fs_file_get_handle(lh_os_fs_file_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_file_get_handle.
 */
LH_ATTRIBUTE_SYMBOL
const lh_os_system_fs_file_handle_t *
lh_os_fs_file_get_handle_as_const(const lh_os_fs_file_t *self);

/**
 * @brief True when @p self currently holds an open handle.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_is_valid(const lh_os_fs_file_t *self);

/**
 * @brief Mode last passed to a successful ::lh_os_fs_file_open, or
 *        ::lh_os_system_fs_file_mode_none when closed.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_system_fs_file_mode_t
lh_os_fs_file_get_mode(const lh_os_fs_file_t *self);

/**
 * @brief Overwrite the stored mode. Does not touch the path or handle.
 *
 * Internal bookkeeping for ::lh_os_fs_file_open/::lh_os_fs_file_close —
 * does not itself open or close anything.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_set_mode(lh_os_fs_file_t *self, lh_os_system_fs_file_mode_t mode);

/**
 * @brief Open @p path on @p self with the given access mode.
 *
 * Closes any previous handle first. Empty @p path is an error.
 *
 * @param self File object to open.
 * @param path Filesystem path; rendered and passed to ::lh_os_system_fs_file_open.
 * @param mode ::lh_os_system_fs_file_mode_read, ::lh_os_system_fs_file_mode_write, or
 *             ::lh_os_system_fs_file_mode_readwrite.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_open(lh_os_fs_file_t *self, const lh_fs_path_t *path, lh_os_system_fs_file_mode_t mode);

/**
 * @brief ::lh_io_reader_read_fn backed by @p context's file.
 *
 * @param context An ::lh_os_fs_file_t* (passed as the reader's context).
 * @param buf     Destination buffer.
 * @param size    Maximum bytes to read into @p buf.
 * @return Bytes actually read (`0` at end of file), or a negative value on
 *         failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_fs_file_read(lh_ptr context, lh_ptr buf, lh_usize_t size);

/**
 * @brief ::lh_io_writer_write_fn backed by @p context's file.
 *
 * @param context An ::lh_os_fs_file_t* (passed as the writer's context).
 * @param buf     Source buffer.
 * @param size    Number of bytes from @p buf to write.
 * @return Bytes actually written, or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_fs_file_write(lh_ptr context, const lh_ptr buf, lh_usize_t size);

/* ── lh_io adapters ──────────────────────────────────────────────────────── */

/**
 * @brief Build an ::lh_io_reader_t that reads from @p self.
 *
 * @param self File to read from; must outlive the returned reader.
 * @return An ::lh_io_reader_t wired to ::lh_os_fs_file_read.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_reader_t
lh_os_fs_file_get_reader(lh_os_fs_file_t *self);

/**
 * @brief Build an ::lh_io_writer_t that writes to @p self.
 *
 * @param self File to write to; must outlive the returned writer.
 * @return An ::lh_io_writer_t wired to ::lh_os_fs_file_write.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_writer_t
lh_os_fs_file_get_writer(lh_os_fs_file_t *self);

/**
 * @brief Build an ::lh_io_stream_t (reader + writer) over @p self.
 *
 * @param self File to wrap; must outlive the returned stream.
 * @return An ::lh_io_stream_t backed by @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_stream_t
lh_os_fs_file_get_stream(lh_os_fs_file_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_FILE_H */
