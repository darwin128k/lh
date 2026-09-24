/**
 * @file system.h
 * @brief OS layer: the last-error slot for the OS's OWN errors (::lh_os_system_error_t).
 *
 * One thread-local ::lh_os_system_error_t — the native counterpart to
 * ::lh_os_last_error (`lh/os.h`). Kept as a separate slot on purpose:
 * a native code (`GetLastError()` / `errno`) and one of ours
 * (::lh_os_error_code_t) are different namespaces that must never be
 * compared or stored in the same field — see
 * `lh/os/system/error/code.h`.
 *
 * The slot is a value holder only — it does not own a text buffer; a
 * description is either a literal (::lh_os_error_desc_lit) or a view into
 * whatever buffer the caller keeps alive (native capture keeps its own,
 * once `lh/os/system/error/capture.h` exists).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_H
#define LH_OS_SYSTEM_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/system/error.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return address of the internal thread-local native last-error slot.
 *
 * @return Pointer to internal ::lh_os_system_error_t object (never null).
 */
LH_ATTRIBUTE_SYMBOL
lh_os_system_error_t *
lh_os_system_last_error(void);

/**
 * @brief Store @p error as the last native OS error on this thread.
 *
 * Description is not copied. Literals stay valid; text captured from the
 * OS must keep its own buffer alive for as long as it is stored here.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_system_set_last_error(lh_os_system_error_t error);

/**
 * @brief Last native error on this thread, as ::lh_os_system_error_t.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_system_error_t
lh_os_system_get_last_error(void);

/**
 * @brief Code of the last native error on this thread.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_system_error_code_t
lh_os_system_get_last_error_code(void);

/**
 * @brief Description of the last native error on this thread.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_desc_t
lh_os_system_get_last_error_desc(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_H */
