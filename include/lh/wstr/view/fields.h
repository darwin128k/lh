/**
 * @file fields.h
 * @brief Macro for declaring wide-string-view endpoint fields (`first` / `second`).
 */

#ifndef LH_WSTR_VIEW_FIELDS_H
#define LH_WSTR_VIEW_FIELDS_H

#include <lh/memory/bounds/fields.h>

/**
 * @def lh_wstr_view_fields(elem_type)
 * @brief Same members as ::lh_memory_bounds_fields for a wide string view.
 *
 * @param elem_type Pointee type (typically `const lh_wchar_t`).
 */
#define lh_wstr_view_fields(elem_type) lh_memory_bounds_fields(elem_type)

#endif /* LH_WSTR_VIEW_FIELDS_H */
