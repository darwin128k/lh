/**
 * @file desc.h
 * @brief Description pointer type for ::lh_error_t.
 *
 * The description is a plain ::lh_str_ptr; callers that need a length
 * build an ::lh_str_view_t from it on demand.
 */

#ifndef LH_ERROR_DESC_H
#define LH_ERROR_DESC_H

#include <lh/str/ptr.h>

/**
 * @def lh_error_desc_ptr
 * @brief Pointer to one ::lh_error_t description character (::lh_str_ptr).
 *
 * For read-only text in headers use `const lh_error_desc_ptr`.
 */
#define lh_error_desc_ptr lh_str_ptr

#endif /* LH_ERROR_DESC_H */
