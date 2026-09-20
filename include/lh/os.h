/**
 * @file os.h
 * @brief OS layer: last-error slot (::lh_os_get_last_error).
 *
 * One thread-local ::lh_os_error_t. Our failures store a view (usually a
 * literal via ::lh_os_error_desc_lit). OS text is copied by
 * ::lh_os_system_error_capture / ::lh_os_system_error_set_cstr, then the
 * same slot views that buffer.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_H
#define LH_OS_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/error/code.h>
#include <lh/os/error.h>
#include <lh/os/system/error.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Store @p error as the last OS-layer error.
 *
 * Description is not copied. Literals stay valid. OS strings must go
 * through ::lh_os_system_error_capture or ::lh_os_system_error_set_cstr.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_set_last_error(lh_os_error_t error);

/**
 * @brief Last error as ::lh_os_error_t.
 *
 * After a system capture the description views the system buffer until
 * the next capture / ::lh_os_system_error_set_cstr.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_t
lh_os_get_last_error(void);

/**
 * @brief Code of the last error.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_os_get_last_error_code(void);

/**
 * @brief Description of the last error.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_desc_t
lh_os_get_last_error_desc(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_H */
