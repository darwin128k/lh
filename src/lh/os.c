#include <lh/os.h>
#include <lh/attribute/thread_local.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/size.h>
#include <lh/str/view.h>
#include <lh/util/str/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <errno.h>
#    include <string.h>
#endif

#define LH_OS_LAST_ERROR_DESC_MAX 512U

static LH_ATTRIBUTE_THREAD_LOCAL lh_error_code_t g_lh_os_last_error_code;
static LH_ATTRIBUTE_THREAD_LOCAL char g_lh_os_last_error_desc[LH_OS_LAST_ERROR_DESC_MAX];

static void
lh_os_last_error_copy_desc(lh_error_desc_t text)
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
        end = lh_str_ptr_copy(g_lh_os_last_error_desc, cap, (lh_str_ptr)data,
                              lh_str_view_get_size(&text));
        end[0] = '\0';
    }
    lh_str_ptr_rtrim(g_lh_os_last_error_desc);
}

void
lh_os_set_last_error(lh_error_code_t code, lh_error_desc_t desc)
{
    g_lh_os_last_error_code = code;
    lh_os_last_error_copy_desc(desc);
}

void
lh_os_capture_last_error(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    const DWORD code = GetLastError();

    g_lh_os_last_error_code = (lh_error_code_t)code;
    if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, lh_null, code,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), g_lh_os_last_error_desc,
                       (DWORD)LH_OS_LAST_ERROR_DESC_MAX, lh_null) == 0)
    {
        g_lh_os_last_error_desc[0] = '\0';
    }
    else
    {
        lh_os_last_error_copy_desc(lh_str_view_make(g_lh_os_last_error_desc));
    }
#else
    g_lh_os_last_error_code = (lh_error_code_t)errno;
    lh_os_last_error_copy_desc(lh_str_view_make(strerror(errno)));
#endif
}

lh_error_code_t
lh_os_get_last_error_code(void)
{
    return g_lh_os_last_error_code;
}

lh_error_desc_t
lh_os_get_last_error_desc(void)
{
    return lh_str_view_make(g_lh_os_last_error_desc);
}

lh_error_t
lh_os_get_last_error(void)
{
    return lh_error_make(lh_os_get_last_error_code(), lh_os_get_last_error_desc());
}
