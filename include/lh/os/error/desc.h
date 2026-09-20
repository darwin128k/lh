/**
 * @file desc.h
 * @brief Description view of an OS error (::lh_os_error_desc_t).
 *
 * Narrow ::lh_str_view_t or wide ::lh_wstr_view_t, selected by
 * ::LH_LIBRARY_OPTION_OS_WERROR. Call sites use ::lh_os_error_desc_lit,
 * ::lh_os_error_desc_make, or ::lh_os_error_desc_from_cstr into a caller
 * buffer. Call sites do not pick the view type.
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_ERROR_DESC_H
#define LH_OS_ERROR_DESC_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/error/desc.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/wstr/view.h>
typedef lh_wstr_view_t lh_os_error_desc_t;
typedef lh_wstr_ptr lh_os_error_desc_ptr;
#    define lh_os_error_desc_lit(s) lh_wstr_view_lit(L##s)
#    define lh_os_error_desc_make(data) lh_wstr_view_make(data)
#else
#    include <lh/str/view.h>
typedef lh_str_view_t lh_os_error_desc_t;
typedef lh_str_ptr lh_os_error_desc_ptr;
#    define lh_os_error_desc_lit(s) lh_str_view_lit(s)
#    define lh_os_error_desc_make(data) lh_str_view_make(data)
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Copy narrow C string @p text into @p dest and return a view over it.
 *
 * When ::LH_LIBRARY_OPTION_OS_WERROR is on, @p text is converted to wide.
 * @p dest_size is the buffer capacity in code units, including the terminator.
 * @p text may be ::lh_null (empty description). Does not touch the last-error slot.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_desc_t
lh_os_error_desc_from_cstr(lh_os_error_desc_ptr dest, lh_usize_t dest_size, lh_str_cptr text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_ERROR_DESC_H */
