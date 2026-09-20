/**
 * @file fields.h
 * @brief Member fields of ::lh_os_fs_file_t.
 */

#ifndef LH_OS_FS_FILE_FIELDS_H
#define LH_OS_FS_FILE_FIELDS_H

/**
 * @def lh_os_fs_file_fields(path_type, handle_type)
 * @brief Path plus the OS handle. Shared by ::lh_os_fs_file_t and
 *        ::lh_os_fs_dir_t; the types stay distinct.
 *
 * @param path_type   Type of the stored path (::lh_os_fs_path_t).
 * @param handle_type Type of the handle (::lh_os_fs_file_handle_t).
 */
#define lh_os_fs_file_fields(path_type, handle_type)                                               \
    path_type path;                                                                                \
    handle_type handle

#endif /* LH_OS_FS_FILE_FIELDS_H */
