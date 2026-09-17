#include <lh/os.h>
#include <lh/attribute/thread_local.h>
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/size.h>

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/util/wstr/ptr.h>
#    include <lh/wstr/view.h>
#else
#    include <lh/str/view.h>
#    include <lh/util/str/ptr.h>
#endif

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <errno.h>
#    include <stdlib.h>
#    include <string.h>
#endif

#define LH_OS_LAST_ERROR_DESC_MAX 512U

static LH_ATTRIBUTE_THREAD_LOCAL lh_error_code_t g_lh_os_last_error_code;

#if LH_LIBRARY_OPTION_OS_WERROR
static LH_ATTRIBUTE_THREAD_LOCAL lh_wchar_t g_lh_os_last_error_desc[LH_OS_LAST_ERROR_DESC_MAX];

static void
lh_os_last_error_copy_desc(lh_os_error_desc_t text)
{
    const lh_usize_t cap = lh_wstr_ptr_get_size(g_lh_os_last_error_desc);
    lh_wstr_cptr data;
    lh_wstr_ptr end;

    if (lh_wstr_view_is_empty(&text))
    {
        g_lh_os_last_error_desc[0] = L'\0';
        return;
    }
    data = lh_wstr_view_get_data(&text);
    if (data != g_lh_os_last_error_desc)
    {
        end = lh_wstr_ptr_copy(g_lh_os_last_error_desc, cap, lh_cast_const(lh_wstr_ptr, data),
                               lh_wstr_view_get_size(&text));
        end[0] = L'\0';
    }
    lh_wstr_ptr_rtrim(g_lh_os_last_error_desc);
}

static void
lh_os_last_error_copy_cstr(lh_str_cptr text)
{
    if (lh_null_eq(text) || text[0] == '\0')
    {
        g_lh_os_last_error_desc[0] = L'\0';
        return;
    }
#    if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (MultiByteToWideChar(CP_ACP, 0, text, -1, g_lh_os_last_error_desc,
                            lh_cast_static(int, LH_OS_LAST_ERROR_DESC_MAX)) <= 0)
    {
        g_lh_os_last_error_desc[0] = L'\0';
        return;
    }
#    else
    {
        const size_t n = mbstowcs(g_lh_os_last_error_desc, text,
                                  lh_cast_static(size_t, LH_OS_LAST_ERROR_DESC_MAX - 1U));

        if (n == lh_cast_static(size_t, -1))
        {
            g_lh_os_last_error_desc[0] = L'\0';
            return;
        }
        g_lh_os_last_error_desc[n] = L'\0';
    }
#    endif
    lh_wstr_ptr_rtrim(g_lh_os_last_error_desc);
}
#else
static LH_ATTRIBUTE_THREAD_LOCAL char g_lh_os_last_error_desc[LH_OS_LAST_ERROR_DESC_MAX];

static void
lh_os_last_error_copy_desc(lh_os_error_desc_t text)
{
    const lh_usize_t cap = lh_str_ptr_get_size(g_lh_os_last_error_desc);
    lh_str_cptr data;
    lh_str_ptr end;

    if (lh_str_view_is_empty(&text))
    {
        g_lh_os_last_error_desc[0] = '\0';
        return;
    }
    data = lh_str_view_get_data(&text);
    if (data != g_lh_os_last_error_desc)
    {
        end = lh_str_ptr_copy(g_lh_os_last_error_desc, cap, lh_cast_const(lh_str_ptr, data),
                              lh_str_view_get_size(&text));
        end[0] = '\0';
    }
    lh_str_ptr_rtrim(g_lh_os_last_error_desc);
}

static void
lh_os_last_error_copy_cstr(lh_str_cptr text)
{
    lh_os_last_error_copy_desc(lh_str_view_make(text));
}
#endif

void
lh_os_set_last_error(lh_error_code_t code, lh_os_error_desc_t desc)
{
    g_lh_os_last_error_code = code;
    lh_os_last_error_copy_desc(desc);
}

void
lh_os_set_last_error_cstr(lh_error_code_t code, lh_str_cptr text)
{
    g_lh_os_last_error_code = code;
    lh_os_last_error_copy_cstr(text);
}

void
lh_os_capture_last_error(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    const DWORD code = GetLastError();

    g_lh_os_last_error_code = lh_cast_static(lh_error_code_t, code);
#    if LH_LIBRARY_OPTION_OS_WERROR
    if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, lh_null, code,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), g_lh_os_last_error_desc,
                       lh_cast_static(DWORD, LH_OS_LAST_ERROR_DESC_MAX), lh_null) == 0)
    {
        g_lh_os_last_error_desc[0] = L'\0';
    }
    else
    {
        lh_os_last_error_copy_desc(lh_wstr_view_make(g_lh_os_last_error_desc));
    }
#    else
    if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, lh_null, code,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), g_lh_os_last_error_desc,
                       lh_cast_static(DWORD, LH_OS_LAST_ERROR_DESC_MAX), lh_null) == 0)
    {
        g_lh_os_last_error_desc[0] = '\0';
    }
    else
    {
        lh_os_last_error_copy_desc(lh_str_view_make(g_lh_os_last_error_desc));
    }
#    endif
#else
    g_lh_os_last_error_code = lh_cast_static(lh_error_code_t, errno);
    lh_os_last_error_copy_cstr(strerror(errno));
#endif
}

lh_error_code_t
lh_os_get_last_error_code(void)
{
    return g_lh_os_last_error_code;
}

lh_os_error_desc_t
lh_os_get_last_error_desc(void)
{
#if LH_LIBRARY_OPTION_OS_WERROR
    return lh_wstr_view_make(g_lh_os_last_error_desc);
#else
    return lh_str_view_make(g_lh_os_last_error_desc);
#endif
}

lh_os_error_t
lh_os_get_last_error(void)
{
#if LH_LIBRARY_OPTION_OS_WERROR
    return lh_werror_make(lh_os_get_last_error_code(), lh_os_get_last_error_desc());
#else
    return lh_error_make(lh_os_get_last_error_code(), lh_os_get_last_error_desc());
#endif
}
