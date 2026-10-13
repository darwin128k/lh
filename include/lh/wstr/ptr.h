/**
 * @file ptr.h
 * @brief Wide-string pointer type aliases.
 *
 * Public headers write read-only text as `const lh_wstr_ptr`. ::lh_wstr_cptr is
 * for `.c` implementations.
 */

#ifndef LH_WSTR_PTR_H
#define LH_WSTR_PTR_H

#include <lh/util/ptr.h>
#include <lh/wchar.h>

/**
 * @def lh_wstr_ptr
 * @brief Mutable pointer to a wide character (::lh_wchar_t *).
 *
 * For read-only text in headers use `const lh_wstr_ptr`.
 */
#define lh_wstr_ptr lh_ptr_of(lh_wchar_t)

/**
 * @def lh_wstr_cptr
 * @brief Constant pointer to a wide character (const ::lh_wchar_t *).
 *
 * For implementation files only; headers use `const lh_wstr_ptr` instead.
 */
#define lh_wstr_cptr lh_cptr_of(lh_wchar_t)

#endif /* LH_WSTR_PTR_H */
