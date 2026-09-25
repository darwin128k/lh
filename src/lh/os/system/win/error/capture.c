#include <lh/os/system/error/capture.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/util/str/ptr.h>
#include <lh/util/wstr/ptr.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

lh_os_system_error_code_t
lh_os_system_error_get_native_code(void)
{
    return lh_cast_static(lh_os_system_error_code_t, GetLastError());
}

lh_os_error_desc_ptr
lh_os_system_error_format(lh_os_system_error_code_t code, lh_os_error_desc_ptr dest, lh_usize_t dest_size)
{
    DWORD n;

    lh_assert_runtime_ref(dest);
#if LH_LIBRARY_OPTION_OS_WERROR
    n = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, lh_null,
                       lh_cast_static(DWORD, code), 0UL, dest, lh_cast_static(DWORD, dest_size),
                       lh_null);
#else
    n = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, lh_null,
                       lh_cast_static(DWORD, code), 0UL, dest, lh_cast_static(DWORD, dest_size),
                       lh_null);
#endif
    if (n == 0UL)
    {
#if LH_LIBRARY_OPTION_OS_WERROR
        return lh_wstr_ptr_clear(dest);
#else
        return lh_str_ptr_clear(dest);
#endif
    }
#if LH_LIBRARY_OPTION_OS_WERROR
    return lh_wstr_ptr_rtrim(dest);
#else
    return lh_str_ptr_rtrim(dest);
#endif
}
