/**
 * @file shared.h
 * @brief One open shared image (::lh_os_shared_t): a `.dll` / `.so` or the
 *        program itself, by path and handle.
 *
 * The counterpart of ::lh_os_fs_file_t for shared images. Its one job is
 * the image: open it, look up its symbols, close it. It knows nothing of
 * modules, lifecycles or loaders — ::lh_os_module_t builds that on top.
 * A library that is not a plugin (load it, call a function) needs only this.
 *
 * An open image holds exactly one OS reference, taken by ::lh_os_shared_open,
 * ::lh_os_shared_bind or ::lh_os_shared_bind_executable and given back by
 * ::lh_os_shared_close. Whoever else loaded the same image keeps their own
 * references: closing this one never unloads it from under them.
 *
 * On failure the reason is in ::lh_os_last_error (our own checks, e.g. an
 * empty path or an already-open image) or ::lh_os_system_last_error (the
 * native call failed).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SHARED_H
#define LH_OS_SHARED_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/path.h>
#include <lh/os/shared/fields.h>
#include <lh/os/system/shared/handle.h>
#include <lh/ptr.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/shared.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_shared
 * @brief Path plus handle. Fields via ::lh_os_shared_fields.
 */
typedef struct lh_os_shared
{
    lh_os_shared_fields(lh_fs_path_t, lh_os_system_shared_handle_t);
} lh_os_shared_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Closed image, empty path. Does not touch the OS.
 */
void
lh_os_shared_init(lh_os_shared_t *self);

/**
 * @brief ::lh_os_shared_close, then release the stored path.
 */
void
lh_os_shared_deinit(lh_os_shared_t *self);

/**
 * @brief True when @p self holds an image.
 */
lh_bool_t
lh_os_shared_is_open(const lh_os_shared_t *self);

/**
 * @brief Raw handle, or ::LH_OS_SYSTEM_SHARED_HANDLE_INVALID when closed.
 */
lh_os_system_shared_handle_t
lh_os_shared_get_handle(const lh_os_shared_t *self);

/**
 * @brief Path of the image. Written only by open / bind, so it always names
 *        the image behind the handle.
 */
const lh_fs_path_t *
lh_os_shared_get_path_as_const(const lh_os_shared_t *self);

/**
 * @brief Load the image at @p path (or take another reference to it, if it
 *        is already loaded).
 *
 * An open @p self fails with ::lh_os_error_code_already_open; an empty
 * @p path is an error.
 */
lh_bool_t
lh_os_shared_open(lh_os_shared_t *self, const lh_fs_path_t *path);

/**
 * @brief Take the already-loaded image that contains @p addr. Its path comes
 *        from the OS.
 */
lh_bool_t
lh_os_shared_bind(lh_os_shared_t *self, lh_ptr addr);

/**
 * @brief Take the program itself (the executable). Its path comes from the OS.
 */
lh_bool_t
lh_os_shared_bind_executable(lh_os_shared_t *self);

/**
 * @brief Give the reference back and forget the handle. The path is kept.
 *
 * Safe on a closed image.
 *
 * @return ::lh_bool_false if the OS close failed; the handle is dropped
 *         either way.
 */
lh_bool_t
lh_os_shared_close(lh_os_shared_t *self);

/**
 * @brief Exported symbol @p name, or ::lh_null if closed / missing.
 *
 * Closed: ::lh_os_error_code_not_open. Missing: the native slot.
 */
lh_ptr
lh_os_shared_get_sym(const lh_os_shared_t *self, lh_str_cptr name);

/**
 * @brief True if open and @p name is exported.
 */
lh_bool_t
lh_os_shared_has_sym(const lh_os_shared_t *self, lh_str_cptr name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SHARED_H */
