/**
 * @file name.h
 * @brief The short name of the operating system LH was built for.
 *
 * One word, lower case, the same on every build for that system: `"win32"`
 * on Windows (32- and 64-bit alike), `"linux"` on Linux, `"mac"` on macOS,
 * `"posix"` on any other POSIX system. Meant for text that differs per system
 * — a section of a list file (`[win32]` / `[linux]`), a folder of native
 * images — so a file can say which system a line is for.
 *
 * The backend is picked by CMake (`src/lh/os/system/win` or
 * `src/lh/os/system/posix`), like every other function under `lh/os/system/`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_NAME_H
#define LH_OS_SYSTEM_NAME_H

#include <lh/config.h>
#include <lh/core/str/view.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/name.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write the system's short name (`"win32"`, `"linux"`, ...) into @p out.
 *
 * @param out Receives a view over a static literal; checked by
 *            ::lh_assert_runtime_ref.
 */
lh_void
lh_os_system_get_name(lh_str_view_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_NAME_H */
