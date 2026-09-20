/**
 * @file error.h
 * @brief OS error value type (::lh_os_error_t) and its API.
 *
 * ::lh_werror_t when ::LH_LIBRARY_OPTION_OS_WERROR is on, otherwise
 * ::lh_error_t. Description is ::lh_os_error_desc_t (`lh/os/error/desc.h`).
 * Native set / get / make are macros onto ::lh_error_* or ::lh_werror_*.
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_ERROR_H
#define LH_OS_ERROR_H

#include <lh/config.h>
#include <lh/os/error/desc.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/error.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/werror.h>
typedef lh_werror_t lh_os_error_t;
#    define lh_os_error_set lh_werror_set
#    define lh_os_error_set_code lh_werror_set_code
#    define lh_os_error_set_desc lh_werror_set_desc
#    define lh_os_error_get_code lh_werror_get_code
#    define lh_os_error_get_desc lh_werror_get_desc
#    define lh_os_error_assign lh_werror_assign
#    define lh_os_error_clear lh_werror_clear
#    define lh_os_error_init lh_werror_init
#    define lh_os_error_init_by_empty lh_werror_init_by_empty
#    define lh_os_error_make lh_werror_make
#    define lh_os_error_make_by_code lh_werror_make_by_code
#else
#    include <lh/error.h>
typedef lh_error_t lh_os_error_t;
#    define lh_os_error_set lh_error_set
#    define lh_os_error_set_code lh_error_set_code
#    define lh_os_error_set_desc lh_error_set_desc
#    define lh_os_error_get_code lh_error_get_code
#    define lh_os_error_get_desc lh_error_get_desc
#    define lh_os_error_assign lh_error_assign
#    define lh_os_error_clear lh_error_clear
#    define lh_os_error_init lh_error_init
#    define lh_os_error_init_by_empty lh_error_init_by_empty
#    define lh_os_error_make lh_error_make
#    define lh_os_error_make_by_code lh_error_make_by_code
#endif

/**
 * @def lh_os_error_set_cstr(self, code, dest, dest_size, text)
 * @brief Replace @p self with @p code and a view of @p text copied into @p dest.
 */
#define lh_os_error_set_cstr(self, code, dest, dest_size, text)                                    \
    lh_os_error_set((self), (code), lh_os_error_desc_from_cstr((dest), (dest_size), (text)))

/**
 * @def lh_os_error_make_by_cstr(code, dest, dest_size, text)
 * @brief Construct an ::lh_os_error_t from @p code and @p text copied into @p dest.
 */
#define lh_os_error_make_by_cstr(code, dest, dest_size, text)                                      \
    lh_os_error_make((code), lh_os_error_desc_from_cstr((dest), (dest_size), (text)))

#endif /* LH_OS_ERROR_H */
