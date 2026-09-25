/**
 * @file fields.h
 * @brief Member fields of ::lh_fs_stat_t.
 */

#ifndef LH_FS_STAT_FIELDS_H
#define LH_FS_STAT_FIELDS_H

/**
 * @def lh_fs_stat_fields(kind_type, perm_type, size_type, time_type, attr_type)
 * @brief Unix-shaped snapshot plus extra ::lh_fs_attr_t bits.
 *
 * Times are Unix seconds.
 */
#define lh_fs_stat_fields(kind_type, perm_type, size_type, time_type, attr_type)                \
    kind_type kind;                                                                                \
    perm_type perm;                                                                                \
    size_type size;                                                                                \
    time_type atime;                                                                               \
    time_type mtime;                                                                                \
    time_type ctime;                                                                               \
    attr_type attr

#endif /* LH_FS_STAT_FIELDS_H */
