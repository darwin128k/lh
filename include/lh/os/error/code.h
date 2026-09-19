/**
 * @file code.h
 * @brief OS-layer codes on top of ::lh_error_code_t.
 *
 * Native failures go through ::lh_os_capture_last_error (`GetLastError` /
 * `errno`). These codes are the lh-side values stored by
 * ::lh_os_set_last_error when the check is ours, not the OS's.
 */

#ifndef LH_OS_ERROR_CODE_H
#define LH_OS_ERROR_CODE_H

#include <lh/error/code.h>

/**
 * @def lh_os_error_code_ok
 * @brief OS alias for the non-error sentinel (::lh_error_code_ok).
 */
#define lh_os_error_code_ok lh_error_code_ok

/**
 * @def lh_os_error_code_path_empty
 * @brief The path has no levels.
 */
#define lh_os_error_code_path_empty 0x01

/**
 * @def lh_os_error_code_too_small
 * @brief A path or name does not fit in ::LH_OS_FS_PATH_MAX (or the OS buffer).
 */
#define lh_os_error_code_too_small 0x02

/**
 * @def lh_os_error_code_invalid_kind
 * @brief ::lh_os_fs_kind_t is not a kind ::lh_os_fs_path_is accepts.
 */
#define lh_os_error_code_invalid_kind 0x03

/**
 * @def lh_os_error_code_null_pointer
 * @brief A required handle, address, or name was null.
 */
#define lh_os_error_code_null_pointer 0x04

/**
 * @def lh_os_error_code_not_in_image
 * @brief The address is not in a loaded shared image.
 */
#define lh_os_error_code_not_in_image 0x05

/**
 * @def lh_os_error_code_no_image_path
 * @brief The OS has no filesystem path for this image handle.
 */
#define lh_os_error_code_no_image_path 0x06

/**
 * @def lh_os_error_code_name_too_long
 * @brief A directory entry name exceeds ::LH_OS_FS_DIR_NAME_MAX.
 */
#define lh_os_error_code_name_too_long 0x07

/**
 * @def lh_os_error_code_not_open
 * @brief The directory iterator or file handle is not open.
 */
#define lh_os_error_code_not_open 0x08

/**
 * @def lh_os_error_code_out_of_memory
 * @brief Allocation for an OS object failed.
 */
#define lh_os_error_code_out_of_memory 0x09

/**
 * @def lh_os_error_code_negative_size
 * @brief The OS reported a negative file size.
 */
#define lh_os_error_code_negative_size 0x0A

/**
 * @def lh_os_error_code_short_read
 * @brief The file ended before the requested number of bytes was read.
 */
#define lh_os_error_code_short_read 0x0B

/**
 * @def lh_os_error_code_not_loaded
 * @brief The shared image is not loaded.
 */
#define lh_os_error_code_not_loaded 0x0C

/**
 * @def lh_os_error_code_already_loaded
 * @brief The shared image is already loaded.
 */
#define lh_os_error_code_already_loaded 0x0D

/**
 * @def lh_os_error_code_failed
 * @brief A native API failed with a string and no numeric last-error (`dlerror`).
 */
#define lh_os_error_code_failed 0x0E

/**
 * @def lh_os_error_code_invalid_offset
 * @brief Seek/map offset is past the file, or does not fit the OS type.
 */
#define lh_os_error_code_invalid_offset 0x0F

/**
 * @typedef lh_os_error_code_t
 * @brief Same representation as ::lh_error_code_t for OS-layer last-error codes.
 */
typedef lh_error_code_t lh_os_error_code_t;

#endif /* LH_OS_ERROR_CODE_H */
