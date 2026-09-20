#include <lh/os/error/desc.h>
#include <lh/config.h>
#include <lh/null.h>

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/attribute/thread_local.h>
#    include <lh/cast/static.h>
#    include <lh/compiler/os.h>
#    include <lh/size.h>

#    if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#        define WIN32_LEAN_AND_MEAN
#        include <windows.h>
#    else
#        include <stdlib.h>
#    endif

#    define LH_OS_ERROR_DESC_CSTR_MAX 512U

static LH_ATTRIBUTE_THREAD_LOCAL lh_wchar_t g_lh_os_error_desc_cstr[LH_OS_ERROR_DESC_CSTR_MAX];
#endif

lh_os_error_desc_t
lh_os_error_desc_from_cstr(lh_str_cptr text)
{
#if LH_LIBRARY_OPTION_OS_WERROR
    if (lh_null_eq(text) || text[0] == '\0')
    {
        g_lh_os_error_desc_cstr[0] = L'\0';
        return lh_os_error_desc_make(g_lh_os_error_desc_cstr);
    }
#    if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (MultiByteToWideChar(CP_ACP, 0, text, -1, g_lh_os_error_desc_cstr,
                            lh_cast_static(int, LH_OS_ERROR_DESC_CSTR_MAX)) <= 0)
    {
        g_lh_os_error_desc_cstr[0] = L'\0';
    }
#    else
    {
        const size_t n = mbstowcs(g_lh_os_error_desc_cstr, text,
                                  lh_cast_static(size_t, LH_OS_ERROR_DESC_CSTR_MAX - 1U));

        if (n == lh_cast_static(size_t, -1))
        {
            g_lh_os_error_desc_cstr[0] = L'\0';
        }
        else
        {
            g_lh_os_error_desc_cstr[n] = L'\0';
        }
    }
#    endif
    return lh_os_error_desc_make(g_lh_os_error_desc_cstr);
#else
    return lh_os_error_desc_make(text);
#endif
}
