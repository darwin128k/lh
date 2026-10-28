/**
 * @file path.h
 * @brief Render an ::lh_fs_path_t as native text for a kernel call.
 *
 * The single place where the fs wrappers (file, dir, stat) turn a path into
 * what `lh/os/system` takes: reject an empty path, render in
 * ::lh_os_system_fs_path_style_native.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_PATH_H
#define LH_OS_FS_PATH_H

#include <lh/config.h>
#include <lh/fs/path.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/path.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Render @p path in the native style into @p scratch.
 *
 * @p scratch is initialized by this call either way (must not already be
 * initialized); the caller always ::lh_str_deinit's it.
 *
 * @return The native text, or ::lh_null for an empty @p path (reason in
 *         ::lh_os_last_error).
 */
const lh_str_ptr
lh_os_fs_path_to_cstr(const lh_fs_path_t *path, lh_str_t *scratch);

/**
 * @brief Resolve @p path to an absolute filesystem path, following symbolic links.
 *
 * @p path and @p out must be initialized and may be the same object.
 * @p out is replaced only on success and remains owned by the caller.
 * Missing, empty, inaccessible and unresolvable paths return ::lh_bool_false
 * with @p out unchanged. Error slots are as for
 * ::lh_os_system_fs_path_canonical; no lexical fallback is performed.
 */
lh_bool_t
lh_os_fs_path_canonical(const lh_fs_path_t *path, lh_fs_path_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_PATH_H */
