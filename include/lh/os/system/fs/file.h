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

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/system/fs/file/handle.h>
#include <lh/fs/file/mode.h>
#include <lh/ptr.h>
#include <lh/size.h>
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
lh_os_system_fs_file_open(lh_str_cptr path, lh_fs_file_mode_t mode);

/**
 * @brief Release @p handle. The close result is not reported.
 *
 * @param handle Open handle; must not be ::LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID.
 */
void
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
 * @return Bytes actually read (`0` at end of file), or a negative value on
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
 * @return Bytes actually written, or a negative value on failure.
 */
lh_ssize_t
lh_os_system_fs_file_write(lh_os_system_fs_file_handle_t handle, const lh_ptr buf, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_FS_FILE_H */
