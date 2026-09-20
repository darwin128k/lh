#include <lh/os.h>
#include <lh/assert.h>
#include <lh/attribute/thread_local.h>
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/config.h>
#include <lh/null.h>
#include <lh/os/error/desc.h>
#include <lh/size.h>
#include <lh/util/str/ptr.h>

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/util/wstr/ptr.h>
#    define lh_os_system_error_desc_clear lh_wstr_ptr_clear
#    define lh_os_system_error_desc_rtrim lh_wstr_ptr_rtrim
#endif

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    define lh_os_system_error_format_message(fn, dest, dest_size, code)                           \
        (fn)(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, lh_null,                  \
             lh_cast_static(DWORD, (code)), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (dest),     \
             lh_cast_static(DWORD, (dest_size)), lh_null)
#else
#    include <errno.h>
#    include <string.h>
#endif

#define LH_OS_SYSTEM_ERROR_DESC_MAX 512U

#if LH_LIBRARY_OPTION_OS_WERROR
static LH_ATTRIBUTE_THREAD_LOCAL lh_wchar_t g_lh_os_system_error_desc[LH_OS_SYSTEM_ERROR_DESC_MAX];
#else
static LH_ATTRIBUTE_THREAD_LOCAL char g_lh_os_system_error_desc[LH_OS_SYSTEM_ERROR_DESC_MAX];
#endif

static void
lh_os_system_error_publish(lh_error_code_t code)
{
    lh_os_set_last_error(
        lh_os_error_make(code, lh_os_error_desc_make(g_lh_os_system_error_desc)));
}

void
lh_os_system_error_set_cstr(lh_error_code_t code, lh_str_cptr text)
{
    lh_os_set_last_error(lh_os_error_make(
        code, lh_os_error_desc_from_cstr(g_lh_os_system_error_desc, LH_OS_SYSTEM_ERROR_DESC_MAX,
                                         text)));
}

lh_str_ptr
lh_os_system_error_format_cstr(lh_error_code_t code, lh_str_ptr dest, lh_usize_t dest_size)
{
    lh_assert_runtime_ref(dest);
    lh_assert_runtime_if(dest_size == 0,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_range));

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_error_format_message(FormatMessageA, dest, dest_size, code) == 0)
    {
        lh_str_ptr_clear(dest);
        return dest;
    }
#else
#    if defined(__GLIBC__) && defined(_GNU_SOURCE)
    {
        const char *text;

        text = strerror_r(lh_cast_static(int, code), dest, lh_cast_static(size_t, dest_size));
        if (lh_null_eq(text))
        {
            lh_str_ptr_clear(dest);
            return dest;
        }
        if (text != dest)
        {
            lh_str_ptr end;

            end = lh_str_ptr_copy(dest, dest_size - 1U, lh_cast_const(lh_str_ptr, text),
                                  lh_str_ptr_len(lh_cast_const(lh_str_ptr, text)));
            lh_str_ptr_clear(end);
        }
    }
#    else
    if (strerror_r(lh_cast_static(int, code), dest, lh_cast_static(size_t, dest_size)) != 0)
    {
        lh_str_ptr_clear(dest);
        return dest;
    }
#    endif
#endif
    lh_str_ptr_rtrim(dest);
    return dest;
}

lh_os_error_desc_ptr
lh_os_system_error_format(lh_error_code_t code, lh_os_error_desc_ptr dest, lh_usize_t dest_size)
{
#if LH_LIBRARY_OPTION_OS_WERROR
    lh_assert_runtime_ref(dest);
    lh_assert_runtime_if(dest_size == 0,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_range));

#    if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_error_format_message(FormatMessageW, dest, dest_size, code) == 0)
    {
        lh_os_system_error_desc_clear(dest);
        return dest;
    }
    lh_os_system_error_desc_rtrim(dest);
    return dest;
#    else
    {
        char text[LH_OS_SYSTEM_ERROR_DESC_MAX];

        lh_os_system_error_format_cstr(code, text, lh_str_ptr_get_capacity(text));
        lh_os_error_desc_from_cstr(dest, dest_size, text);
        return dest;
    }
#    endif
#else
    return lh_os_system_error_format_cstr(code, dest, dest_size);
#endif
}

void
lh_os_system_error_capture(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    const lh_error_code_t code = lh_cast_static(lh_error_code_t, GetLastError());
#else
    const lh_error_code_t code = lh_cast_static(lh_error_code_t, errno);
#endif

    lh_os_system_error_format(code, g_lh_os_system_error_desc, LH_OS_SYSTEM_ERROR_DESC_MAX);
    lh_os_system_error_publish(code);
}
