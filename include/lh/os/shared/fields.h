/**
 * @file fields.h
 * @brief Member fields of ::lh_os_shared_t.
 */

#ifndef LH_OS_SHARED_FIELDS_H
#define LH_OS_SHARED_FIELDS_H

/**
 * @def lh_os_shared_fields(path_type, handle_type)
 * @brief Path of the image and the OS handle, which holds one reference.
 *
 * @param path_type   Type of `path` (::lh_fs_path_t).
 * @param handle_type Type of `handle` (::lh_os_system_shared_handle_t).
 */
#define lh_os_shared_fields(path_type, handle_type)                                                \
    path_type path;                                                                                \
    handle_type handle

#endif /* LH_OS_SHARED_FIELDS_H */
