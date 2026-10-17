/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_fs_stat_t.
 */

#ifndef LH_FS_STAT_INITIALIZER_H
#define LH_FS_STAT_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_fs_stat_initializer(kind, perm, size, atime, mtime, ctime, attr)
 * @brief Produces a brace-enclosed initializer for ::lh_fs_stat_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_fs_stat_fields.
 *
 * @param kind  Object kind (::lh_fs_kind_t).
 * @param perm  Permission bits (::lh_fs_perm_t).
 * @param size  Size in bytes (::lh_fs_size_t).
 * @param atime Last access, Unix seconds (::lh_fs_time_t).
 * @param mtime Last modification, Unix seconds.
 * @param ctime Last status change, Unix seconds.
 * @param attr  Extra Windows flags (::lh_fs_attr_t).
 *
 * Example usage:
 * @code{.c}
 * lh_fs_stat_t stat = lh_fs_stat_initializer(kind,   // kind
 *                                            perm,   // perm
 *                                            4096,   // size
 *                                            0,      // atime
 *                                            0,      // mtime
 *                                            0,      // ctime
 *                                            attr);  // attr
 * @endcode
 *
 * @see lh_initializer
 * @see lh_fs_stat_t
 * @see lh_fs_stat_init
 */
#define lh_fs_stat_initializer(kind, perm, size, atime, mtime, ctime, attr)                        \
    lh_initializer(kind, perm, size, atime, mtime, ctime, attr)

#endif /* LH_FS_STAT_INITIALIZER_H */
