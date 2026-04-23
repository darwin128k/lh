/**
 * @file desc.h
 * @brief Type alias for error description pointers (::lh_error_desc_t).
 */

#ifndef LH_ERROR_DESC_H
#define LH_ERROR_DESC_H

#include <lh/str/ptr.h>

/**
 * @def lh_error_desc_t
 * @brief Type of ::lh_error_t::desc — read-only text; alias of ::lh_str_cptr.
 *
 * Central place to change the description pointer type (e.g. to `const char *`).
 * Adjust here without touching every signature.
 */
#define lh_error_desc_t lh_str_cptr

#endif // LH_ERROR_DESC_H
