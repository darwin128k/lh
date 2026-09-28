/**
 * @file shared.h
 * @brief Kernel shared-image primitives: open, close, symbol, path.
 *
 * The only place that talks to `LoadLibraryW` / `GetProcAddress` /
 * `FreeLibrary` (Windows) or `dlopen` / `dlsym` / `dlclose` (POSIX).
 *
 * Every handle these functions hand out holds exactly one OS reference to
 * the image (the OS counts them per process, for everyone), and
 * ::lh_os_system_shared_close gives exactly that one back. A caller never
 * releases a reference it did not take, so an image someone else also
 * loaded stays loaded for them.
 * Every function has one contract on every platform; the backend is picked
 * by CMake (`src/lh/os/system/win` or `src/lh/os/system/posix`), not by
 * `#if` at the call site.
 *
 * Takes native text, not ::lh_fs_path_t — rendering a path is the caller's
 * job (see ::lh_os_shared_open). Holds no state: a handle in, a result out.
 *
 * On failure the native reason is in ::lh_os_system_last_error.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_SHARED_H
#define LH_OS_SYSTEM_SHARED_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/system/shared/handle.h>
#include <lh/ptr.h>
#include <lh/str.h>
#include <lh/str/ptr.h>
#include <lh/str/view.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/shared.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Shared-image suffix for this OS (`".dll"` / `".so"`).
 *
 * @return View over a static literal; its size is known without a scan.
 */
lh_str_view_t
lh_os_system_shared_get_ext(void);

/**
 * @brief Open the shared image at @p path.
 *
 * @param path Native, NUL-terminated path text.
 * @return Open handle, or ::LH_OS_SYSTEM_SHARED_HANDLE_INVALID on failure.
 */
lh_os_system_shared_handle_t
lh_os_system_shared_open(lh_str_cptr path);

/**
 * @brief Give back the one reference @p handle holds.
 *
 * @param handle Open handle; the caller has already checked it.
 * @return ::lh_bool_true if the OS reported success.
 */
lh_bool_t
lh_os_system_shared_close(lh_os_system_shared_handle_t handle);

/**
 * @brief Address of exported symbol @p name, or ::lh_null.
 *
 * @param handle Open handle.
 * @param name   Exported symbol name.
 */
lh_ptr
lh_os_system_shared_get_sym(lh_os_system_shared_handle_t handle, lh_str_cptr name);

/**
 * @brief Handle of the program itself (the executable, not a library).
 *
 * Windows: `GetModuleHandleEx(NULL)`; POSIX: `dlopen(NULL)`. Takes a
 * reference like any other handle here; the executable itself can never
 * be unloaded, but the count stays balanced.
 *
 * @return Handle (close it), or ::LH_OS_SYSTEM_SHARED_HANDLE_INVALID on failure.
 */
lh_os_system_shared_handle_t
lh_os_system_shared_get_executable(void);

/**
 * @brief Handle of the already-loaded image that contains @p addr.
 *
 * Windows: `GetModuleHandleEx(FROM_ADDRESS)`; POSIX: `dlopen(RTLD_NOLOAD)`.
 * Both take a reference, so the image stays loaded while the handle is
 * held even if whoever loaded it lets go.
 *
 * @param addr An address inside the loaded image.
 * @return Handle (close it), or ::LH_OS_SYSTEM_SHARED_HANDLE_INVALID on failure.
 */
lh_os_system_shared_handle_t
lh_os_system_shared_get_by_addr(lh_ptr addr);

/**
 * @brief Native path of @p handle into @p out (replacing its contents).
 *
 * Buffer sizing is the backend's business: the caller gets the whole path
 * or a failure, never a truncated one.
 *
 * @param handle Open handle.
 * @param out    Initialized string that receives the path.
 * @return ::lh_bool_false on failure (::lh_os_last_error for out-of-memory
 *         or an over-long path, ::lh_os_system_last_error otherwise).
 */
lh_bool_t
lh_os_system_shared_get_path(lh_os_system_shared_handle_t handle, lh_str_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_SHARED_H */
