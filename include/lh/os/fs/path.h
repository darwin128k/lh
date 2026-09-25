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

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/path.h>
#include <lh/str.h>
#include <lh/str/ptr.h>

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
lh_str_cptr
lh_os_fs_path_to_cstr(const lh_fs_path_t *path, lh_str_t *scratch);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_PATH_H */
