/*
 * Force the XSI signature of strerror_r (int-returning, always writes into
 * our buffer) before any system header can pull in <string.h> and lock in
 * glibc's default feature-test macros. Without this, a project build that
 * enables GNU extensions (CMAKE_C_EXTENSIONS, on by default) sees glibc's
 * incompatible GNU variant instead (char*-returning; may ignore our buffer
 * for a known error and hand back a static string).
 */
#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#    define _POSIX_C_SOURCE 200112L
#endif

#include <lh/os/system/error.h>
#include <lh/assert.h>
#include <lh/attribute/thread_local.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/desc.h>
#include <lh/util/str/ptr.h>
#include <lh/util/wstr/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <errno.h>
#    include <string.h>
#    if LH_LIBRARY_OPTION_OS_WERROR
#        error "lh/os/system/error.c: LH_LIBRARY_OPTION_OS_WERROR is Windows-only for now"
#    endif
#endif

#if LH_LIBRARY_OPTION_OS_WERROR
LH_ATTRIBUTE_THREAD_LOCAL
lh_wchar_t m_os_system_error_buffer[LH_OS_SYSTEM_ERROR_BUFFER_SIZE];
#else
LH_ATTRIBUTE_THREAD_LOCAL
lh_char_t m_os_system_error_buffer[LH_OS_SYSTEM_ERROR_BUFFER_SIZE];
#endif

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS

lh_os_error_desc_ptr
lh_os_system_error_format(lh_error_code_t code, lh_os_error_desc_ptr dest, lh_usize_t dest_size)
{
    DWORD n;

    lh_assert_runtime_ref(dest);
#    if LH_LIBRARY_OPTION_OS_WERROR
    n = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, lh_null,
                       lh_cast_static(DWORD, code), 0UL, dest, lh_cast_static(DWORD, dest_size),
                       lh_null);
#    else
    n = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, lh_null,
                       lh_cast_static(DWORD, code), 0UL, dest, lh_cast_static(DWORD, dest_size),
                       lh_null);
#    endif
    if (n == 0UL)
    {
#    if LH_LIBRARY_OPTION_OS_WERROR
        return lh_wstr_ptr_clear(dest);
#    else
        return lh_str_ptr_clear(dest);
#    endif
    }
#    if LH_LIBRARY_OPTION_OS_WERROR
    return lh_wstr_ptr_rtrim(dest);
#    else
    return lh_str_ptr_rtrim(dest);
#    endif
}

#else

lh_os_error_desc_ptr
lh_os_system_error_format(lh_error_code_t code, lh_os_error_desc_ptr dest, lh_usize_t dest_size)
{
    lh_assert_runtime_ref(dest);
    if (strerror_r(lh_cast_static(int, code), dest, dest_size) != 0)
    {
        return lh_str_ptr_clear(dest);
    }
    return lh_str_ptr_rtrim(dest);
}

#endif

void
lh_os_system_error_capture(void)
{
    lh_error_code_t code;
    lh_os_error_desc_t desc;

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    code = lh_cast_static(lh_error_code_t, GetLastError());
#else
    code = lh_cast_static(lh_error_code_t, errno);
#endif
    (void)lh_os_system_error_format(code, m_os_system_error_buffer, LH_OS_SYSTEM_ERROR_BUFFER_SIZE);
#if LH_LIBRARY_OPTION_OS_WERROR
    desc = lh_wstr_view_make(m_os_system_error_buffer);
#else
    desc = lh_str_view_make(m_os_system_error_buffer);
#endif
    lh_os_set_last_error(lh_os_error_make(code, desc));
}
