#include <lh/os/system/error/capture.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/char/map.h>
#include <lh/null.h>
#include <lh/numeric/limits.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/util/math.h>
#include <lh/util/str/ptr.h>
#include <lh/util/wstr/ptr.h>

lh_os_system_error_code_t
lh_os_system_error_get_native_code(void)
{
    return lh_cast_static(lh_os_system_error_code_t, GetLastError());
}

#if LH_LIBRARY_OPTION_OS_WERROR

lh_os_error_desc_ptr
lh_os_system_error_format(lh_os_system_error_code_t code, lh_os_error_desc_ptr dest, lh_usize_t dest_size)
{
    lh_assert_runtime_ref(dest);
    const lh_os_system_win_dword_t n =
        FormatMessageW(LH_OS_SYSTEM_WIN_FORMAT_MESSAGE_FROM_SYSTEM |
                           LH_OS_SYSTEM_WIN_FORMAT_MESSAGE_IGNORE_INSERTS,
                       lh_null, lh_cast_static(lh_os_system_win_dword_t, code), 0UL, dest,
                       lh_cast_static(lh_os_system_win_dword_t, dest_size), lh_null);
    if (lh_math_is_zero(n))
    {
        return lh_wstr_ptr_clear(dest);
    }
    return lh_wstr_ptr_rtrim(dest);
}

#else

/* The message is taken in UTF-16 and handed out as UTF-8, like every other
   text lh gets from Windows: FormatMessageA would give the ANSI code page. */
lh_os_error_desc_ptr
lh_os_system_error_format(lh_os_system_error_code_t code, lh_os_error_desc_ptr dest,
                          lh_usize_t dest_size)
{
    lh_wchar_t wide[LH_OS_SYSTEM_ERROR_BUFFER_SIZE];

    lh_assert_runtime_ref(dest);
    if (lh_math_is_zero(dest_size))
    {
        return dest;
    }
    const lh_os_system_win_dword_t n =
        FormatMessageW(LH_OS_SYSTEM_WIN_FORMAT_MESSAGE_FROM_SYSTEM |
                           LH_OS_SYSTEM_WIN_FORMAT_MESSAGE_IGNORE_INSERTS,
                       lh_null, lh_cast_static(lh_os_system_win_dword_t, code), 0UL, wide,
                       LH_OS_SYSTEM_ERROR_BUFFER_SIZE, lh_null);
    if (lh_math_is_zero(n))
    {
        return lh_str_ptr_clear(dest);
    }
    /* Room for the NUL is kept back; a message that does not fit is dropped
       rather than cut inside a UTF-8 sequence. */
    const lh_int_t room =
        lh_cast_static(lh_int_t, lh_math_min(lh_math_sub_one(dest_size),
                                             lh_cast_static(lh_usize_t, LH_INT_T_MAX)));
    const lh_int_t bytes =
        WideCharToMultiByte(LH_OS_SYSTEM_WIN_CP_UTF8, 0UL, wide, lh_cast_static(lh_int_t, n), dest,
                            room, lh_null, lh_null);
    if (lh_math_is_zero(bytes))
    {
        return lh_str_ptr_clear(dest);
    }
    dest[bytes] = lh_char_map_nul;
    return lh_str_ptr_rtrim(dest);
}

#endif
