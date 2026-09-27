/**
 * @file kind.h
 * @brief Backend-private: ::lh_fs_kind_t of a Win32 file from its
 *        `FILE_ATTRIBUTE_*` flags and reparse tag.
 *
 * Shared by `win/fs/dir.c` (directory entries) and `win/fs/stat.c` so both
 * classify a file the same way. Not installed, not part of the API.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_FS_KIND_H
#define LH_SRC_OS_SYSTEM_WIN_FS_KIND_H

#include <lh/attribute/static.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/fs/kind.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/util/bit.h>
#include <lh/util/math.h>

/* A reparse point is not necessarily a symlink (junctions, dedup, cloud
   placeholders are reparse points too): only its tag says so. The tag is in
   WIN32_FIND_DATA's dwReserved0. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_fs_is_symlink_tag(lh_os_system_win_dword_t attrs, lh_os_system_win_dword_t reparse_tag)
{
    return lh_cast_static(lh_bool_t,
                          !lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_REPARSE_POINT)) &&
                              lh_math_eq(reparse_tag, LH_OS_SYSTEM_WIN_IO_REPARSE_TAG_SYMLINK));
}

/* Symlink first (a symlink to a directory carries the directory flag too),
   then directory, else a regular file. */
LH_ATTRIBUTE_STATIC
lh_fs_kind_t
lh_os_system_fs_kind_from_attrs(lh_os_system_win_dword_t attrs, lh_bool_t is_symlink)
{
    if (is_symlink)
    {
        return lh_fs_kind_symlink;
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_DIRECTORY)))
    {
        return lh_fs_kind_dir;
    }
    return lh_fs_kind_file;
}

#endif /* LH_SRC_OS_SYSTEM_WIN_FS_KIND_H */
