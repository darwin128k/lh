/**
 * @file scan.h
 * @brief Find every module image in a folder of module folders.
 *
 * The folder holds one folder per module, the image inside named like that
 * folder (`modules/qproto/qproto.so`); a file right in the folder
 * (`modules/qproto.so`) is taken as it is. Each entry goes through
 * ::lh_os_loader_image_try_resolve, and every one that resolves to an existing
 * file is handed to the callback, in the order the system lists the folder.
 * The scan opens nothing itself: loading, skipping and logging are the
 * callback's.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_LOADER_SCAN_H
#define LH_OS_LOADER_SCAN_H

#include <lh/config.h>
#include <lh/os/loader/scan/cb.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/loader/scan.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Call @p cb for every module image in @p dir.
 *
 * A @p dir that does not exist or is not a folder holds no images: nothing is
 * called and the scan succeeds.
 *
 * @param dir  The folder to look in.
 * @param cb   Called once per image; checked by ::lh_assert_runtime_ref.
 * @param user Handed to @p cb as it is.
 * @return ::lh_bool_false when @p dir is empty (::lh_os_error_code_path_empty)
 *         or the folder could not be opened or read (the reason is in
 *         ::lh_os_system_last_error); images found before a read error have
 *         been handed to @p cb.
 */
lh_bool_t
lh_os_loader_scan(const lh_fs_path_t *dir, lh_os_loader_scan_cb cb, lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOADER_SCAN_H */
