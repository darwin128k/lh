/**
 * @file stat.h
 * @brief Kernel `lstat` / `GetFileAttributesEx`: fill an ::lh_fs_stat_t
 *        from native path text.
 *
 * The only place that reads native file metadata and decodes it (Unix
 * `st_mode`, Windows `FILE_ATTRIBUTE_*` and `FILETIME`) into the portable
 * ::lh_fs_stat_t. The backend is picked by CMake, not by `#if`.
 *
 * Takes native text, not ::lh_fs_path_t, and knows nothing about names:
 * name-derived flags (a leading `.` → ::lh_fs_attr_hidden) are the
 * caller's job (see ::lh_os_fs_stat).
 *
 * On failure the reason is in ::lh_os_system_last_error (the native call
 * failed) or ::lh_os_last_error (a native value we cannot represent, e.g.
 * a negative size).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_FS_STAT_H
#define LH_OS_SYSTEM_FS_STAT_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/stat.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/fs/stat.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Snapshot of @p path into @p out, without following a final
 *        symlink.
 *
 * @param path Native, NUL-terminated path text.
 * @param out  Receives the snapshot on success; untouched on failure.
 * @return ::lh_bool_true on success, ::lh_bool_false on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_system_fs_stat(lh_str_cptr path, lh_fs_stat_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_FS_STAT_H */
