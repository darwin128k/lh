/**
 * @file mode.h
 * @brief Which access ::lh_os_fs_file_open requests from the OS.
 *
 * Read and write are not different kinds of object — both come from the
 * same `CreateFile` / `open` call, distinguished only by access and
 * creation flags. ::lh_os_fs_file_t reflects that: one struct, one open
 * function, and this constant says which flags the OS should use.
 */

#ifndef LH_OS_FS_FILE_MODE_H
#define LH_OS_FS_FILE_MODE_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_os_fs_file_mode_t
 * @brief Discriminator for which access to open.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_os_fs_file_mode_*`
 * constants below.
 */
typedef lh_u8_t lh_os_fs_file_mode_t;

/**
 * @def lh_os_fs_file_mode_read
 * @brief Existing file, read-only (`GENERIC_READ` / `O_RDONLY`).
 */
#define lh_os_fs_file_mode_read 0U

/**
 * @def lh_os_fs_file_mode_write
 * @brief Create or replace, write-only (`GENERIC_WRITE` + `CREATE_ALWAYS` /
 *        `O_WRONLY | O_CREAT | O_TRUNC`).
 */
#define lh_os_fs_file_mode_write 1U

/**
 * @def lh_os_fs_file_mode_readwrite
 * @brief Existing or created, read-write (`GENERIC_READ | GENERIC_WRITE` /
 *        `O_RDWR | O_CREAT`). Needed to map pages writable.
 */
#define lh_os_fs_file_mode_readwrite 2U

#endif /* LH_OS_FS_FILE_MODE_H */
