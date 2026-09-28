/**
 * @file shared.h
 * @brief Kernel shared-image primitives: open, close, symbol, path.
 *
 * The only place that talks to `LoadLibraryA` / `GetProcAddress` /
 * `FreeLibrary` (Windows) or `dlopen` / `dlsym` / `dlclose` (POSIX).
 * Every function has one contract on every platform; the backend is picked
 * by CMake (`src/lh/os/system/win` or `src/lh/os/system/posix`), not by
 * `#if` at the call site.
 *
 * Takes native text, not ::lh_fs_path_t — rendering a path is the caller's
 * job (see ::lh_os_module_open). Holds no state: a handle in, a result out.
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
lh_os_system_shared_ext(void);

/**
 * @brief Open the shared image at @p path.
 *
 * @param path Native, NUL-terminated path text.
 * @return Open handle, or ::LH_OS_SYSTEM_SHARED_HANDLE_INVALID on failure.
 */
lh_os_system_shared_handle_t
lh_os_system_shared_open(lh_str_cptr path);

/**
 * @brief Release @p handle. The caller owns the reference being released.
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
lh_os_system_shared_sym(lh_os_system_shared_handle_t handle, lh_str_cptr name);

/**
 * @brief Handle of the program itself (the executable, not a library).
 *
 * Windows: `GetModuleHandleEx(NULL)`, no `FreeLibrary` reference. POSIX:
 * `dlopen(NULL)`, a reference that must be closed.
 *
 * @param owned Receives ::lh_bool_true when the caller must
 *              ::lh_os_system_shared_close the result.
 * @return Handle, or ::LH_OS_SYSTEM_SHARED_HANDLE_INVALID on failure.
 */
lh_os_system_shared_handle_t
lh_os_system_shared_main(lh_bool_t *owned);

/**
 * @brief Handle of the already-loaded image that contains @p addr.
 *
 * Windows does not take a `FreeLibrary` reference
 * (`GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT`). POSIX `dlopen`
 * (`RTLD_NOLOAD`) does, and that reference must be closed.
 *
 * @param addr  An address inside the loaded image.
 * @param owned Receives ::lh_bool_true when the caller must
 *              ::lh_os_system_shared_close the result.
 * @return Handle, or ::LH_OS_SYSTEM_SHARED_HANDLE_INVALID on failure.
 */
lh_os_system_shared_handle_t
lh_os_system_shared_of_addr(lh_ptr addr, lh_bool_t *owned);

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
lh_os_system_shared_path(lh_os_system_shared_handle_t handle, lh_str_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_SHARED_H */
