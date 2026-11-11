/**
 * @file fn.h
 * @brief ::lh_os_loader_scan_fn — what ::lh_os_loader_scan calls for each image.
 */

#ifndef LH_OS_LOADER_SCAN_FN_H
#define LH_OS_LOADER_SCAN_FN_H

#include <lh/core/fs/path.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Take one module image found by a scan.
 *
 * @param user  The value given to ::lh_os_loader_scan.
 * @param image The image path (it exists and is a file); valid for the call only.
 */
typedef lh_void(lh_os_loader_scan_fn)(lh_ptr user, const lh_fs_path_t *image);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOADER_SCAN_FN_H */
