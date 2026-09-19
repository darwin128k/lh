/**
 * @file seek.h
 * @brief `whence` for ::lh_os_fs_file_seek — same numbers as POSIX `SEEK_*`.
 */

#ifndef LH_OS_FS_FILE_SEEK_H
#define LH_OS_FS_FILE_SEEK_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_os_fs_file_seek_whence_t
 * @brief Origin for a seek (`SEEK_SET` / `SEEK_CUR` / `SEEK_END`).
 */
typedef lh_u8_t lh_os_fs_file_seek_whence_t;

/**
 * @def lh_os_fs_file_seek_set
 * @brief From the start of the file (`SEEK_SET` / `FILE_BEGIN`).
 */
#define lh_os_fs_file_seek_set 0U

/**
 * @def lh_os_fs_file_seek_cur
 * @brief From the current position (`SEEK_CUR` / `FILE_CURRENT`).
 */
#define lh_os_fs_file_seek_cur 1U

/**
 * @def lh_os_fs_file_seek_end
 * @brief From the end of the file (`SEEK_END` / `FILE_END`).
 */
#define lh_os_fs_file_seek_end 2U

#endif /* LH_OS_FS_FILE_SEEK_H */
