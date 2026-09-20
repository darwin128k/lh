/**
 * @file attr.h
 * @brief Extra filesystem flags that Unix `st_mode` does not carry.
 *
 * Canonical state is ::lh_os_fs_stat_t (kind + Unix perms + times). These
 * bits are the platform leftover: Windows `FILE_ATTRIBUTE_*` mapped onto
 * names we own, Unix only fills ::lh_os_fs_attr_hidden (a leading `.` in
 * the last path name, not `.` / `..`).
 *
 * Bits are not Windows `FILE_ATTRIBUTE_*` values.
 */

#ifndef LH_OS_FS_ATTR_H
#define LH_OS_FS_ATTR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/util/bit.h>

/**
 * @typedef lh_os_fs_attr_t
 * @brief Extra attribute mask on a ::lh_os_fs_stat_t.
 */
typedef lh_u32_t lh_os_fs_attr_t;

/**
 * @def lh_os_fs_attr_hidden
 * @brief Not shown in a default listing (`FILE_ATTRIBUTE_HIDDEN` / leading `.`).
 */
#define lh_os_fs_attr_hidden 0x0001U

/**
 * @def lh_os_fs_attr_system
 * @brief Operating-system file (`FILE_ATTRIBUTE_SYSTEM`). Unix: unset.
 */
#define lh_os_fs_attr_system 0x0002U

/**
 * @def lh_os_fs_attr_archive
 * @brief Backup/archive (`FILE_ATTRIBUTE_ARCHIVE`). Unix: unset.
 */
#define lh_os_fs_attr_archive 0x0004U

/**
 * @def lh_os_fs_attr_compressed
 * @brief Compressed (`FILE_ATTRIBUTE_COMPRESSED`). Unix: unset.
 */
#define lh_os_fs_attr_compressed 0x0008U

/**
 * @def lh_os_fs_attr_encrypted
 * @brief Encrypted (`FILE_ATTRIBUTE_ENCRYPTED`). Unix: unset.
 */
#define lh_os_fs_attr_encrypted 0x0010U

/**
 * @def lh_os_fs_attr_temporary
 * @brief Temporary (`FILE_ATTRIBUTE_TEMPORARY`). Unix: unset.
 */
#define lh_os_fs_attr_temporary 0x0020U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when every bit in @p mask is set in @p attr.
 */
static inline lh_bool_t
lh_os_fs_attr_has(lh_os_fs_attr_t attr, lh_os_fs_attr_t mask)
{
    return (lh_bit_and(attr, mask) == mask) ? lh_bool_true : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_ATTR_H */
