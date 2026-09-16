/**
 * @file view.h
 * @brief Non-owning wide string view (::lh_wstr_view_t).
 *
 * Layout-compatible with ::lh_memory_view_t. Endpoints are `const lh_wchar_t`.
 * ::lh_wstr_view_get_size is in ::lh_wchar_t units (not bytes).
 */

#ifndef LH_WSTR_VIEW_H
#define LH_WSTR_VIEW_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/memory/view.h>
#include <lh/size.h>
#include <lh/void.h>
#include <lh/wchar.h>
#include <lh/wstr/ptr.h>
#include <lh/wstr/view/initializer.h>

/**
 * @brief Non-owning read-only half-open wide string view.
 */
typedef lh_memory_view_t lh_wstr_view_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Initialize @p self as <tt>[data, data + size)</tt> in wchar units.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_wstr_view_init_by_size(lh_wstr_view_t *self, lh_wstr_cptr data, lh_usize_t size);

/**
 * @brief Initialize @p self from a NUL-terminated wide string @p data.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_wstr_view_init(lh_wstr_view_t *self, lh_wstr_cptr data);

/**
 * @brief Initialize @p self with the empty view initializer.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_wstr_view_init_empty(lh_wstr_view_t *self);

/**
 * @brief Initialize @p self as a copy of @p other.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_wstr_view_init_by_other(lh_wstr_view_t *self, const lh_wstr_view_t *other);

/**
 * @brief View over a NUL-terminated wide C string.
 *
 * ::lh_null or an empty string become ::lh_wstr_view_empty_initializer.
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_view_t
lh_wstr_view_make(lh_wstr_cptr data);

LH_ATTRIBUTE_SYMBOL
lh_wstr_cptr
lh_wstr_view_get_begin(const lh_wstr_view_t *self);

LH_ATTRIBUTE_SYMBOL
lh_wstr_cptr
lh_wstr_view_get_end(const lh_wstr_view_t *self);

LH_ATTRIBUTE_SYMBOL
lh_wstr_cptr
lh_wstr_view_get_data(const lh_wstr_view_t *self);

/**
 * @brief Number of ::lh_wchar_t elements in the view (not counting a stored NUL).
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_wstr_view_get_size(const lh_wstr_view_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_view_is_empty(const lh_wstr_view_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WSTR_VIEW_H */
