/**
 * @file local.h
 * @brief Shared pieces of the `lh_os_fs_file_*` implementation. Not public.
 */

#ifndef LH_OS_FS_FILE_LOCAL_H
#define LH_OS_FS_FILE_LOCAL_H

#include <lh/os/fs/file.h>
#include <lh/attribute/force_inline.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
typedef HANDLE lh_os_fs_native_handle_t;
#else
typedef int lh_os_fs_native_handle_t;
#endif

LH_ATTRIBUTE_FORCE_INLINE
lh_os_fs_native_handle_t
lh_os_fs_file_native_handle(const lh_os_fs_file_t *self)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return lh_cast_reinterpret(lh_os_fs_native_handle_t, self->handle);
#else
    return lh_cast_static(lh_os_fs_native_handle_t, self->handle);
#endif
}

#endif /* LH_OS_FS_FILE_LOCAL_H */
