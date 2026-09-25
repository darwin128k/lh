/**
 * @file os.h
 * @brief OS layer: the last-error slot for OUR errors (::lh_os_error_t).
 *
 * One thread-local ::lh_os_error_t, the OS-layer counterpart to
 * ::lh_runtime_allocator's thread-local instance. The slot is a value
 * holder only — it does not own a text buffer; a description is either
 * a literal (::lh_os_error_desc_lit) or a view into whatever buffer the
 * caller keeps alive.
 *
 * This is OUR slot — codes lh itself raises before ever asking the OS
 * anything (::lh_os_error_code_t). The OS's own last error
 * (`GetLastError()` / `errno`) has its own, separate slot:
 * ::lh_os_system_last_error (`lh/os/system.h`), never this one — see
 * `lh/os/system/error/code.h` for why the two must not share a slot.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_H
#define LH_OS_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/error/code.h>
#include <lh/os/error.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return address of the internal thread-local last-error slot.
 *
 * @return Pointer to internal ::lh_os_error_t object (never null).
 */
lh_os_error_t *
lh_os_last_error(void);

/**
 * @brief Store @p error as the last OS-layer error on this thread.
 *
 * Description is not copied. Literals stay valid; text captured from the
 * OS must go through `lh/os/system.h`, which keeps its own slot and buffer.
 */
void
lh_os_set_last_error(lh_os_error_t error);

/**
 * @brief Last error on this thread, as ::lh_os_error_t.
 */
lh_os_error_t
lh_os_get_last_error(void);

/**
 * @brief Code of the last error on this thread.
 */
lh_error_code_t
lh_os_get_last_error_code(void);

/**
 * @brief Description of the last error on this thread.
 */
lh_os_error_desc_t
lh_os_get_last_error_desc(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_H */
