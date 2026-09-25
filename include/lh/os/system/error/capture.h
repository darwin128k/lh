/**
 * @file capture.h
 * @brief Formatting and capture of the native OS last error.
 *
 * ::lh_os_system_error_format is a pure, stateless formatter: given a native
 * code, it writes the OS message into a buffer the caller owns.
 * ::lh_os_system_error_capture is the convenience entry point most call
 * sites want: it reads the native last error (`GetLastError` / `errno`),
 * formats it into a private thread-local buffer this module owns, and
 * stores code + a view of that buffer as the current thread's last
 * *native* error via ::lh_os_system_set_last_error (`lh/os/system.h`) —
 * never ::lh_os_set_last_error (`lh/os.h`), which is a different
 * namespace entirely (see `lh/os/system/error/code.h`).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_ERROR_CAPTURE_H
#define LH_OS_SYSTEM_ERROR_CAPTURE_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/error/desc.h>
#include <lh/os/system/error/code.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/error/capture.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

#ifndef LH_OS_SYSTEM_ERROR_BUFFER_SIZE
/**
 * @def LH_OS_SYSTEM_ERROR_BUFFER_SIZE
 * @brief Capacity (in characters, including the NUL) of the private buffer
 *        ::lh_os_system_error_capture formats the native message into.
 *
 * Define this before including this header (or as a compiler `-D` flag)
 * to override it. Does not bound ::lh_os_system_error_format, which
 * writes into whatever buffer the caller passes.
 */
#    define LH_OS_SYSTEM_ERROR_BUFFER_SIZE 256U
#endif /* LH_OS_SYSTEM_ERROR_BUFFER_SIZE */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Format the OS message for @p code into @p dest.
 *
 * Windows: `FormatMessageA`/`FormatMessageW` (per ::LH_LIBRARY_OPTION_OS_WERROR).
 * POSIX: `strerror_r`. Trailing CR/LF/whitespace is trimmed. Does not touch
 * either last-error slot — @p dest is entirely the caller's buffer, sized
 * however they like.
 *
 * @param code      Native error code (as `GetLastError()`/`errno` would report).
 * @param dest      Destination buffer, at least @p dest_size characters.
 * @param dest_size Capacity of @p dest in characters, including the NUL.
 * @return @p dest.
 */
lh_os_error_desc_ptr
lh_os_system_error_format(lh_os_system_error_code_t code, lh_os_error_desc_ptr dest, lh_usize_t dest_size);

/**
 * @brief The calling thread's native last-error code, unformatted
 *        (`GetLastError()` on Windows, `errno` on POSIX).
 *
 * Only reads; does not touch ::lh_os_system_last_error.
 */
lh_os_system_error_code_t
lh_os_system_error_get_native_code(void);

/**
 * @brief Snapshot the native last error as the *native* last error on this
 *        thread (::lh_os_system_set_last_error).
 *
 * Reads the native code, then formats it via ::lh_os_system_error_format
 * into this module's own private, thread-local, ::LH_OS_SYSTEM_ERROR_BUFFER_SIZE
 * buffer. The stored description views that buffer; it stays valid until
 * the next call to this function on the same thread.
 */
void
lh_os_system_error_capture(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_ERROR_CAPTURE_H */
