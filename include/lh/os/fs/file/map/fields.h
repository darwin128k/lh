/**
 * @file fields.h
 * @brief Member fields of ::lh_os_fs_file_map_t.
 *
 * `data` / `size` are the requested view. `base` / `base_size` are the OS
 * mapping (page-aligned); ::lh_os_fs_file_unmap uses those.
 */

#ifndef LH_OS_FS_FILE_MAP_FIELDS_H
#define LH_OS_FS_FILE_MAP_FIELDS_H

/**
 * @def lh_os_fs_file_map_fields(ptr_type, size_type)
 * @brief User view plus the aligned OS region behind it.
 */
#define lh_os_fs_file_map_fields(ptr_type, size_type)                                              \
    ptr_type data;                                                                                 \
    size_type size;                                                                                \
    ptr_type base;                                                                                 \
    size_type base_size

#endif /* LH_OS_FS_FILE_MAP_FIELDS_H */
