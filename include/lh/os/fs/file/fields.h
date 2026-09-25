/**
 * @file fields.h
 * @brief Member fields of ::lh_os_fs_file_t.
 */

#ifndef LH_OS_FS_FILE_FIELDS_H
#define LH_OS_FS_FILE_FIELDS_H

/**
 * @def lh_os_fs_file_fields(path_type, handle_type, mode_type)
 * @brief Path, OS handle, and the mode last used with ::lh_os_fs_file_open.
 *
 * @param path_type   Type of the stored path (::lh_fs_path_t).
 * @param handle_type Type of the handle (::lh_os_system_fs_file_handle_t).
 * @param mode_type   Type of the open mode (::lh_fs_file_mode_t).
 */
#define lh_os_fs_file_fields(path_type, handle_type, mode_type)                                    \
    path_type path;                                                                                \
    handle_type handle;                                                                            \
    mode_type mode

#endif /* LH_OS_FS_FILE_FIELDS_H */
