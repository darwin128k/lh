/**
 * @file prot.h
 * @brief Protection bits for ::lh_os_fs_file_map (`PROT_READ` / `PROT_WRITE`).
 *
 * Unix numbers. Windows `PAGE_READONLY` / `PAGE_READWRITE` are mapped onto
 * these. Write implies the pages are also readable.
 */

#ifndef LH_OS_FS_FILE_MAP_PROT_H
#define LH_OS_FS_FILE_MAP_PROT_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/util/bit.h>

/**
 * @typedef lh_os_fs_file_map_prot_t
 * @brief mmap protection mask.
 */
typedef lh_u32_t lh_os_fs_file_map_prot_t;

/**
 * @def lh_os_fs_file_map_prot_read
 * @brief Pages may be read (`PROT_READ`).
 */
#define lh_os_fs_file_map_prot_read 0x1U

/**
 * @def lh_os_fs_file_map_prot_write
 * @brief Pages may be written (`PROT_WRITE`). Also readable.
 */
#define lh_os_fs_file_map_prot_write 0x2U

LH_COMPILER_EXTERN_C_BEGIN

LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_os_fs_file_map_prot_has(lh_os_fs_file_map_prot_t prot, lh_os_fs_file_map_prot_t mask)
{
    return (lh_bit_and(prot, mask) == mask) ? lh_bool_true : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_FILE_MAP_PROT_H */
