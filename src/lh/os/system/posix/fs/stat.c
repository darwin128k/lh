#include <lh/os/system/fs/stat.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/error/capture.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/math.h>

#include <sys/stat.h>

#define LH_OS_SYSTEM_FS_PERM_UNIX_MASK 07777U

/*
 * Pure decoder for POSIX st_mode's S_IFMT bit format. Built on a portable
 * lh_u32_t, not mode_t, and on our own mirrors of the S_IF* numeric values —
 * not <sys/stat.h>'s. These values are not POSIX-mandated numbers, but every
 * mainstream Unix (Linux, BSD, macOS, Solaris) has used them unchanged since
 * traditional Unix, so they're safe to hardcode.
 */
#define LH_OS_SYSTEM_FS_UNIX_S_IFMT 0170000U
#define LH_OS_SYSTEM_FS_UNIX_S_IFLNK 0120000U
#define LH_OS_SYSTEM_FS_UNIX_S_IFDIR 0040000U
#define LH_OS_SYSTEM_FS_UNIX_S_IFREG 0100000U

/**
 * @brief Raw POSIX `st_mode` bits (file-type bits + permission bits,
 *        undecoded). Not ::lh_fs_file_mode_t (that's our own read/write
 *        open-mode discriminator — unrelated).
 */
typedef lh_u32_t lh_os_system_fs_unix_mode_t;

LH_ATTRIBUTE_STATIC
lh_fs_kind_t
lh_os_system_fs_kind_from_unix_mode(lh_os_system_fs_unix_mode_t mode)
{
    lh_u32_t fmt;

    fmt = lh_bit_and(mode, LH_OS_SYSTEM_FS_UNIX_S_IFMT);
    if (lh_math_eq(fmt, LH_OS_SYSTEM_FS_UNIX_S_IFLNK))
    {
        return lh_fs_kind_symlink;
    }
    if (lh_math_eq(fmt, LH_OS_SYSTEM_FS_UNIX_S_IFDIR))
    {
        return lh_fs_kind_dir;
    }
    if (lh_math_eq(fmt, LH_OS_SYSTEM_FS_UNIX_S_IFREG))
    {
        return lh_fs_kind_file;
    }
    return lh_fs_kind_other;
}

lh_bool_t
lh_os_system_fs_stat(lh_str_cptr path, lh_fs_stat_t *out)
{
    struct stat info;
    lh_os_system_fs_unix_mode_t mode_bits;

    lh_assert_runtime_ref(path);
    lh_assert_runtime_ref(out);

    if (lh_math_ne(lstat(path, lh_addr_of(info)), 0))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    if (lh_math_is_negative(info.st_size))
    {
        lh_os_set_last_error(
            lh_os_error_make(lh_os_error_code_negative_size, lh_os_error_desc_lit("file size is negative")));
        return lh_bool_false;
    }
    mode_bits = lh_cast_static(lh_os_system_fs_unix_mode_t, info.st_mode);
    lh_fs_stat_set(out, lh_os_system_fs_kind_from_unix_mode(mode_bits),
                   lh_cast_static(lh_fs_perm_t, lh_bit_and(mode_bits, LH_OS_SYSTEM_FS_PERM_UNIX_MASK)),
                   lh_cast_static(lh_fs_size_t, info.st_size), lh_cast_static(lh_fs_time_t, info.st_atime),
                   lh_cast_static(lh_fs_time_t, info.st_mtime), lh_cast_static(lh_fs_time_t, info.st_ctime), 0U);
    return lh_bool_true;
}
