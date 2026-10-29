/**
 * @file file.h
 * @brief Kernel file primitives: open, read, write, close on a raw
 *        ::lh_os_system_fs_file_handle_t.
 *
 * The only place that talks to `CreateFileA` / `ReadFile` / `WriteFile` /
 * `CloseHandle` (Windows) or `open` / `read` / `write` / `close` (POSIX).
 * Every function has one contract on every platform; the backend is picked
 * by CMake (`src/lh/os/system/win` or `src/lh/os/system/posix`), not by
 * `#if` at the call site.
 *
 * Takes native text, not ::lh_fs_path_t — rendering a path is the caller's
 * job (see ::lh_os_fs_file_open). Holds no state: a handle in, a result out.
 *
 * On failure the native reason is in ::lh_os_system_last_error.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_FS_FILE_H
#define LH_OS_SYSTEM_FS_FILE_H

#include <lh/config.h>
#include <lh/os/result.h>
#include <lh/os/system/fs/file/handle.h>
#include <lh/fs/file/mode.h>
#include <lh/fs/size.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/fs/file.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Open @p path with the access and creation flags of @p mode.
 *
 * @param path Native, NUL-terminated path text.
 * @param mode ::lh_fs_file_mode_read,
 *             ::lh_fs_file_mode_write, or
 *             ::lh_fs_file_mode_readwrite.
 * @return Open handle, or ::LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID on failure.
 */
lh_os_system_fs_file_handle_t
lh_os_system_fs_file_open(const lh_str_ptr path, lh_fs_file_mode_t mode);

/**
 * @brief Open an existing regular file below the directory handle @p root for reading.
 *
 * @p path is a NUL-terminated relative path with `/` separators. Empty
 * components, `.` and `..`, backslashes and colons are rejected. Symbolic
 * links are never followed, including in intermediate components. The root
 * must already refer to a trusted directory; it is borrowed and never closed.
 * The lookup uses directory handles, without a separate path-based check.
 * Mount points and hard links are not excluded by this operation.
 *
 * @param root Open directory, kept alive by the caller for this call.
 * @param path Relative file name; not modified or retained.
 * @param size Receives the nonnegative size of the opened file on success;
 *             unchanged on failure. The file may change after this snapshot.
 * @return Owned read handle (close with ::lh_os_system_fs_file_close), or
 *         ::LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID on failure. Does not wait for
 *         a FIFO writer. The returned handle is not inherited across exec.
 *
 * POSIX backend only for now; other backends fail closed with
 * ::lh_os_error_code_not_supported. Native errors are captured in
 * ::lh_os_system_last_error; invalid paths in ::lh_os_last_error.
 */
lh_os_system_fs_file_handle_t
lh_os_system_fs_file_open_read_at(lh_os_system_fs_file_handle_t root, const lh_str_ptr path,
                                  lh_fs_size_t *size);

/**
 * @brief Release @p handle. The close result is not reported.
 *
 * @param handle Open handle; must not be ::LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID.
 */
lh_void
lh_os_system_fs_file_close(lh_os_system_fs_file_handle_t handle);

/**
 * @brief Read up to @p size bytes from @p handle into @p buf.
 *
 * One native call. May read fewer bytes than asked (also when @p size
 * exceeds what the native call accepts at once).
 *
 * @param handle Open handle with read access.
 * @param buf    Destination buffer.
 * @param size   Maximum bytes to read into @p buf.
 * @return Bytes actually read (`0` at end of file), or ::LH_OS_RESULT_INVALID on
 *         failure.
 */
lh_ssize_t
lh_os_system_fs_file_read(lh_os_system_fs_file_handle_t handle, lh_ptr buf, lh_usize_t size);

/**
 * @brief Write up to @p size bytes from @p buf to @p handle.
 *
 * One native call. May write fewer bytes than asked (also when @p size
 * exceeds what the native call accepts at once).
 *
 * @param handle Open handle with write access.
 * @param buf    Source buffer.
 * @param size   Number of bytes from @p buf to write.
 * @return Bytes actually written, or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_system_fs_file_write(lh_os_system_fs_file_handle_t handle, const lh_ptr buf, lh_usize_t size);

/**
 * @brief Delete the file at @p path.
 *
 * @param path Native, NUL-terminated file path text.
 * @return ::lh_bool_true when the OS deleted it; ::lh_bool_false (error captured)
 *         otherwise — including when there is no such file.
 */
lh_bool_t
lh_os_system_fs_file_remove(const lh_str_ptr path);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_FS_FILE_H */
