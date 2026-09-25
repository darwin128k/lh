/**
 * @file perm.h
 * @brief Unix permission bits on a filesystem object (::lh_fs_perm_t).
 *
 * Same numbers as POSIX `mode_t` permission bits (`07777`). Windows is
 * mapped onto this: `FILE_ATTRIBUTE_READONLY` clears write bits; a
 * directory gets execute bits. There is no second permission model.
 */

#ifndef LH_FS_PERM_H
#define LH_FS_PERM_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/util/bit.h>
#include <lh/util/math.h>

/**
 * @typedef lh_fs_perm_t
 * @brief Unix permission mask (`st_mode & 07777`).
 */
typedef lh_u16_t lh_fs_perm_t;

/**
 * @def lh_fs_perm_ixoth
 * @brief Others execute (`0001`).
 */
#define lh_fs_perm_ixoth 0001U

/**
 * @def lh_fs_perm_iwoth
 * @brief Others write (`0002`).
 */
#define lh_fs_perm_iwoth 0002U

/**
 * @def lh_fs_perm_iroth
 * @brief Others read (`0004`).
 */
#define lh_fs_perm_iroth 0004U

/**
 * @def lh_fs_perm_ixgrp
 * @brief Group execute (`0010`).
 */
#define lh_fs_perm_ixgrp 0010U

/**
 * @def lh_fs_perm_iwgrp
 * @brief Group write (`0020`).
 */
#define lh_fs_perm_iwgrp 0020U

/**
 * @def lh_fs_perm_irgrp
 * @brief Group read (`0040`).
 */
#define lh_fs_perm_irgrp 0040U

/**
 * @def lh_fs_perm_ixusr
 * @brief Owner execute (`0100`).
 */
#define lh_fs_perm_ixusr 0100U

/**
 * @def lh_fs_perm_iwusr
 * @brief Owner write (`0200`).
 */
#define lh_fs_perm_iwusr 0200U

/**
 * @def lh_fs_perm_irusr
 * @brief Owner read (`0400`).
 */
#define lh_fs_perm_irusr 0400U

/**
 * @def lh_fs_perm_isvtx
 * @brief Sticky (`01000`).
 */
#define lh_fs_perm_isvtx 01000U

/**
 * @def lh_fs_perm_isgid
 * @brief Set-group-ID (`02000`).
 */
#define lh_fs_perm_isgid 02000U

/**
 * @def lh_fs_perm_isuid
 * @brief Set-user-ID (`04000`).
 */
#define lh_fs_perm_isuid 04000U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when every bit in @p mask is set in @p perm.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_fs_perm_has(lh_fs_perm_t perm, lh_fs_perm_t mask)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_bit_and(perm, mask), mask));
}

/**
 * @brief True when the owner-read bit (::lh_fs_perm_irusr) is set.
 *
 * Owner bits only: there is no second permission model (see this file's
 * doc), and the Windows mapping always mirrors owner into group/other, so
 * checking owner is checking the only bits that ever actually differ.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_fs_perm_is_readable(lh_fs_perm_t perm)
{
    return lh_fs_perm_has(perm, lh_fs_perm_irusr);
}

/**
 * @brief True when the owner-write bit (::lh_fs_perm_iwusr) is set.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_fs_perm_is_writable(lh_fs_perm_t perm)
{
    return lh_fs_perm_has(perm, lh_fs_perm_iwusr);
}

/**
 * @brief True when the owner-execute bit (::lh_fs_perm_ixusr) is set.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_fs_perm_is_executable(lh_fs_perm_t perm)
{
    return lh_fs_perm_has(perm, lh_fs_perm_ixusr);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_FS_PERM_H */
