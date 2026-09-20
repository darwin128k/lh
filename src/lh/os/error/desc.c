#include <lh/os/error/desc.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/config.h>
#include <lh/null.h>
#include <lh/size.h>
#include <lh/util/str/ptr.h>

#if LH_LIBRARY_OPTION_OS_WERROR
#    include <lh/util/wstr/ptr.h>
#    define lh_os_error_desc_ptr_clear lh_wstr_ptr_clear
#    define lh_os_error_desc_ptr_erase lh_wstr_ptr_erase
#    define lh_os_error_desc_ptr_rtrim lh_wstr_ptr_rtrim
#else
#    define lh_os_error_desc_ptr_clear lh_str_ptr_clear
#    define lh_os_error_desc_ptr_rtrim lh_str_ptr_rtrim
#endif

#if LH_LIBRARY_OPTION_OS_WERROR
#    if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#        define WIN32_LEAN_AND_MEAN
#        include <windows.h>
#    else
#        include <stdlib.h>
#    endif
#endif

lh_os_error_desc_t
lh_os_error_desc_from_cstr(lh_os_error_desc_ptr dest, lh_usize_t dest_size, lh_str_cptr text)
{
    lh_assert_runtime_ref(dest);
    lh_assert_runtime_if(dest_size == 0,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_range));

    if (lh_null_eq(text) || text[0] == '\0' || dest_size == 1U)
    {
        lh_os_error_desc_ptr_clear(dest);
        return lh_os_error_desc_make(dest);
    }
#if LH_LIBRARY_OPTION_OS_WERROR
#    if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (MultiByteToWideChar(CP_ACP, 0, text, -1, dest, lh_cast_static(int, dest_size)) <= 0)
    {
        lh_os_error_desc_ptr_clear(dest);
        return lh_os_error_desc_make(dest);
    }
#    else
    {
        const lh_usize_t n = lh_cast_static(lh_usize_t,
            mbstowcs(dest, text, lh_cast_static(size_t, dest_size - 1U)));

        if (n == LH_WSTR_PTR_INVALID)
        {
            lh_os_error_desc_ptr_clear(dest);
            return lh_os_error_desc_make(dest);
        }
        lh_os_error_desc_ptr_erase(dest, n);
    }
#    endif
#else
    {
        lh_str_ptr end;

        end = lh_str_ptr_copy(dest, dest_size - 1U, lh_cast_const(lh_str_ptr, text),
                              lh_str_ptr_len(lh_cast_const(lh_str_ptr, text)));
        lh_os_error_desc_ptr_clear(end);
    }
#endif
    lh_os_error_desc_ptr_rtrim(dest);
    return lh_os_error_desc_make(dest);
}
