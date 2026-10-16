/**
 * @file desc.h
 * @brief Description pointer type for ::lh_werror_t.
 *
 * The description is a plain ::lh_wstr_ptr; callers that need a length
 * build an ::lh_wstr_view_t from it on demand.
 */

#ifndef LH_WERROR_DESC_H
#define LH_WERROR_DESC_H

#include <lh/wstr/ptr.h>

/**
 * @def lh_werror_desc_ptr
 * @brief Pointer to one ::lh_werror_t description character (::lh_wstr_ptr).
 *
 * For read-only text in headers use `const lh_werror_desc_ptr`.
 */
#define lh_werror_desc_ptr lh_wstr_ptr

#endif /* LH_WERROR_DESC_H */
