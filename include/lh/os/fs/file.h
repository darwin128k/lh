/**
 * @file file.h
 * @brief A blocking OS file — the second real ::lh_io_stream_t backend
 *        (a file, per that file's doc).
 *
 * One struct, one open function for both access modes (see
 * ::lh_os_fs_file_mode_t): read and write are not different kinds of
 * object, just the flags argument to the same `CreateFile` / `open`.
 *
 * Deliberately narrow otherwise: no seek, no mapping. Directory listing is
 * ::lh_os_fs_dir_t (`lh/os/fs/dir.h`).
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
#include <lh/io/stream.h>
#include <lh/io/writer.h>
#include <lh/numeric/fixed/types.h>
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
 * @typedef lh_os_fs_file_t
 * @brief A single OS file handle.
 */
struct lh_os_fs_file
{
    lh_os_fs_file_handle_t handle;
};
typedef struct lh_os_fs_file lh_os_fs_file_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── construct / lifetime ────────────────────────────────────────────────── */

/**
 * @brief Set @p self to the empty (not-yet-open) state.
 *
 * Pure value initialization — does not touch the OS. Call this (or
 * ::lh_os_fs_file_open directly) before any other operation.
 *
 * @param self File object to initialize.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_init(lh_os_fs_file_t *self);

/**
 * @brief Open @p path on @p self with the given access mode.
 *
 * @param self File object to open; must be in the empty state
 *             (::lh_os_fs_file_init or freshly ::lh_os_fs_file_close'd).
 * @param path Filesystem path (`CreateFileA` / `open`). Empty is an error.
 * @param mode ::lh_os_fs_file_mode_read or ::lh_os_fs_file_mode_write.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_open(lh_os_fs_file_t *self, const lh_os_fs_path_t *path, lh_os_fs_file_mode_t mode);

/**
 * @brief Close @p self's handle (if open) and return it to the empty state.
 *
 * Safe to call on an already-empty file.
 *
 * @param self File object to close.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_close(lh_os_fs_file_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the raw handle stored in @p self.
 * @param self File to read from.
 * @return Current ::lh_os_fs_file_handle_t
 *         (::LH_OS_FS_FILE_HANDLE_INVALID if not open).
 */
LH_ATTRIBUTE_SYMBOL
lh_os_fs_file_handle_t
lh_os_fs_file_get_handle(const lh_os_fs_file_t *self);

/**
 * @brief Test whether @p self currently holds an open handle.
 * @param self File to test.
 * @return ::lh_bool_true if open, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_is_valid(const lh_os_fs_file_t *self);

/**
 * @brief Byte size of the file currently open on @p self.
 *
 * Windows: `GetFileSizeEx`. POSIX: `fstat`.
 *
 * @param self Open file.
 * @param out  Receives the size in bytes.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_get_size(const lh_os_fs_file_t *self, lh_u64_t *out);

/* ── operations ──────────────────────────────────────────────────────────── */

/**
 * @brief ::lh_io_reader_read_fn backed by @p context's file.
 *
 * @param context An ::lh_os_fs_file_t* (passed as the reader's context).
 * @param buf     Destination buffer.
 * @param size    Maximum bytes to read into @p buf.
 * @return Bytes actually read (`0` at end of file), or a negative value
 *         on failure.
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

/**
 * @brief Read exactly @p size bytes from @p self into @p buf.
 *
 * Loops ::lh_os_fs_file_read until @p size bytes arrive. A short file (EOF
 * before @p size) is a failure. @p size `0` succeeds without touching @p buf.
 *
 * @param self Open file (read mode).
 * @param buf  Destination buffer.
 * @param size Number of bytes to read.
 * @return ::lh_bool_true if every byte was read, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_read_all(lh_os_fs_file_t *self, lh_ptr buf, lh_usize_t size);

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
 * This is the point of ::lh_io_stream_t: everything above this call sees a
 * generic duplex stream and does not need to know a file is behind it.
 *
 * @param self Open file to wrap; must outlive the returned stream.
 * @return An ::lh_io_stream_t backed by @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_stream_t
lh_os_fs_file_get_stream(lh_os_fs_file_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_FILE_H */
