/**
 * @file ptr.h
 * @brief Wide-string pointer type aliases.
 *
 * Defines ::lh_wstr_ptr and ::lh_wstr_cptr as typed pointers to ::lh_wchar_t
 * using the generic pointer macros from @c <lh/util/ptr.h>.
 */

#ifndef LH_WSTR_PTR_H
#define LH_WSTR_PTR_H

#include <lh/util/ptr.h>
#include <lh/wchar.h>

/**
 * @def lh_wstr_ptr
 * @brief Mutable pointer to a wide character (::lh_wchar_t *).
 */
#define lh_wstr_ptr lh_ptr_of(lh_wchar_t)

/**
 * @def lh_wstr_cptr
 * @brief Constant pointer to a wide character (const ::lh_wchar_t *).
 */
#define lh_wstr_cptr lh_cptr_of(lh_wchar_t)

#endif /* LH_WSTR_PTR_H */