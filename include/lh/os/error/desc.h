#ifndef LH_OS_ERROR_DESC_H
#define LH_OS_ERROR_DESC_H

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/wstr/ptr.h>
#    include <lh/wstr/view.h>
#    include <lh/wstr/view/initializer.h>

typedef lh_wstr_view_t lh_os_error_desc_t;

/**
 * @def lh_os_error_desc_ptr
 * @brief Mutable pointer to one description character (write target for
 *        ::lh_os_system_error_format). ::lh_wstr_ptr or ::lh_str_ptr,
 *        matching ::lh_os_error_desc_t.
 */
#    define lh_os_error_desc_ptr lh_wstr_ptr

#    define lh_os_error_desc_lit(s) lh_wstr_view_lit(s)
#else
#    include <lh/str/ptr.h>
#    include <lh/str/view.h>
#    include <lh/str/view/initializer.h>

typedef lh_str_view_t lh_os_error_desc_t;

/**
 * @def lh_os_error_desc_ptr
 * @brief Mutable pointer to one description character (write target for
 *        ::lh_os_system_error_format). ::lh_wstr_ptr or ::lh_str_ptr,
 *        matching ::lh_os_error_desc_t.
 */
#    define lh_os_error_desc_ptr lh_str_ptr

#    define lh_os_error_desc_lit(s) lh_str_view_lit(s)
#endif

#endif /* LH_OS_ERROR_DESC_H */
