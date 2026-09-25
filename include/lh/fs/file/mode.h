/**
 * @file mode.h
 * @brief Which access ::lh_os_system_fs_file_open requests from the OS.
 *
 * Read and write are not different kinds of object — both come from the
 * same `CreateFile` / `open` call, distinguished only by access and
 * creation flags. ::lh_os_system_fs_file_open reflects that: one open
 * function, and this constant says which flags the OS should use.
 */

#ifndef LH_FS_FILE_MODE_H
#define LH_FS_FILE_MODE_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/util/math.h>

/**
 * @typedef lh_fs_file_mode_t
 * @brief Discriminator for which access to open.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_fs_file_mode_*`
 * constants below.
 */
typedef lh_u8_t lh_fs_file_mode_t;

/**
 * @def lh_fs_file_mode_read
 * @brief Existing file, read-only (`GENERIC_READ` / `O_RDONLY`).
 */
#define lh_fs_file_mode_read 0U

/**
 * @def lh_fs_file_mode_write
 * @brief Create or replace, write-only (`GENERIC_WRITE` + `CREATE_ALWAYS` /
 *        `O_WRONLY | O_CREAT | O_TRUNC`).
 */
#define lh_fs_file_mode_write 1U

/**
 * @def lh_fs_file_mode_readwrite
 * @brief Existing or created, read-write (`GENERIC_READ | GENERIC_WRITE` /
 *        `O_RDWR | O_CREAT`). Needed to map pages writable.
 */
#define lh_fs_file_mode_readwrite 2U

/**
 * @def lh_fs_file_mode_none
 * @brief No open mode (handle is invalid).
 */
#define lh_fs_file_mode_none 0xFFU

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when @p mode grants read access (`read` or `readwrite`).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_fs_file_mode_is_readable(lh_fs_file_mode_t mode)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(mode, lh_fs_file_mode_read) ||
                                     lh_math_eq(mode, lh_fs_file_mode_readwrite));
}

/**
 * @brief True when @p mode grants write access (`write` or `readwrite`).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_fs_file_mode_is_writable(lh_fs_file_mode_t mode)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(mode, lh_fs_file_mode_write) ||
                                     lh_math_eq(mode, lh_fs_file_mode_readwrite));
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_FS_FILE_MODE_H */
