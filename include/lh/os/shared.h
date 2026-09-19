/**
 * @file shared.h
 * @brief OS shared library open / close / symbol lookup.
 *
 * Requires ::LH_LIBRARY_OPTION_OS. Windows: `LoadLibraryA` / `GetProcAddress` /
 * `FreeLibrary`. POSIX: `dlopen` / `dlsym` / `dlclose`.
 *
 * ::lh_os_shared_of_addr maps an already-loaded image (the host plugin)
 * without `LoadLibrary` / a fresh `dlopen`. A loaded image with a child
 * table is ::lh_os_shared_module_t (`lh/os/shared/module.h`).
 *
 * On failure the reason is in ::lh_os_get_last_error (see `lh/os.h`).
 */

#ifndef LH_OS_SHARED_H
#define LH_OS_SHARED_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/ptr.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/shared.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @typedef lh_os_shared_handle_t
 * @brief Opaque OS module handle (`HMODULE` / `void *` from `dlopen`).
 */
typedef lh_ptr lh_os_shared_handle_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Shared-library suffix for this OS (`".dll"` / `".so"`).
 *
 * @return Pointer to a static string; never ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_os_shared_ext(void);

/**
 * @brief True if @p name ends with ::lh_os_shared_ext (case-insensitive).
 *
 * A predicate: ::lh_null or empty is false, no last-error.
 *
 * @param name File name or path.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_is(lh_str_cptr name);

/**
 * @brief True if @p path is a loadable shared library file.
 *
 * ::lh_os_shared_is, not a directory, not a shortcut, and a file.
 *
 * @param path Filesystem path.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_path_is(lh_str_cptr path);

/**
 * @brief Open the shared library at @p path.
 *
 * @param path Filesystem path (`LoadLibraryA` / `dlopen`). ::lh_null is an error.
 * @return Handle, or ::lh_null on failure. Then ::lh_os_get_last_error.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_shared_handle_t
lh_os_shared_open(lh_str_cptr path);

/**
 * @brief Handle of the already-loaded image that contains @p addr.
 *
 * Windows: `GetModuleHandleExA` (`FROM_ADDRESS` | `UNCHANGED_REFCOUNT`) —
 * Vista-safe, does not add a `FreeLibrary` ref. POSIX: `dladdr` + `dlopen`
 * `RTLD_NOLOAD` (that extra ref must be `dlclose`'d).
 *
 * @param addr An address inside the loaded module (e.g. an exported symbol).
 * @return Handle, or ::lh_null on failure. Then ::lh_os_get_last_error.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_shared_handle_t
lh_os_shared_of_addr(lh_ptr addr);

/**
 * @brief Close @p handle.
 *
 * @param handle Module from ::lh_os_shared_open. ::lh_null is an error.
 * @return ::lh_bool_true if the OS reported success.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_close(lh_os_shared_handle_t handle);

/**
 * @brief Address of exported symbol @p name, or ::lh_null.
 *
 * @param handle Module from ::lh_os_shared_open.
 * @param name   Exported symbol (`GetProcAddress` / `dlsym`).
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_os_shared_get_sym(lh_os_shared_handle_t handle, lh_str_cptr name);

/**
 * @brief True if @p name is exported from @p handle.
 *
 * @param handle Module from ::lh_os_shared_open.
 * @param name   Exported symbol name.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_has_sym(lh_os_shared_handle_t handle, lh_str_cptr name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SHARED_H */
