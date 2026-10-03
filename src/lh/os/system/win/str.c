#include <lh/os/system/str.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/numeric/limits.h>
#include <lh/os/alloc.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/runtime/allocator.h>
#include <lh/math.h>
#include <lh/util/ptr.h>

lh_bool_t
lh_os_system_str_from_utf8(lh_os_str_t *out, lh_str_cptr text)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(text);
    /* -1: the input is NUL-terminated, and the count includes the NUL. */
    const lh_int_t units = MultiByteToWideChar(
        LH_OS_SYSTEM_WIN_CP_UTF8, LH_OS_SYSTEM_WIN_MB_ERR_INVALID_CHARS, text, -1, lh_null, 0);
    if (lh_math_is_zero(units))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_wstr_ptr const buf = lh_ptr_cast(
        lh_wchar_t,
        lh_os_alloc(lh_math_mul(lh_cast_static(lh_usize_t, units), sizeof(lh_wchar_t))));
    if (lh_null_eq(buf))
    {
        return lh_bool_false;
    }
    const lh_bool_t ok = lh_cast_static(
        lh_bool_t, !lh_math_is_zero(MultiByteToWideChar(LH_OS_SYSTEM_WIN_CP_UTF8,
                                                        LH_OS_SYSTEM_WIN_MB_ERR_INVALID_CHARS, text,
                                                        -1, buf, units)));
    if (ok)
    {
        lh_os_str_clear(out);
        lh_os_str_append(out, buf, lh_cast_static(lh_usize_t, lh_math_sub_one(units)));
    }
    else
    {
        lh_os_system_error_capture();
    }
    lh_runtime_allocator_free(buf);
    return ok;
}

lh_bool_t
lh_os_system_str_to_utf8(lh_os_str_cptr text, lh_usize_t count, lh_str_t *out)
{
    lh_assert_runtime_ref(text);
    lh_assert_runtime_ref(out);
    if (lh_math_is_zero(count))
    {
        lh_str_clear(out);
        return lh_bool_true;
    }
    const lh_int_t units =
        lh_cast_static(lh_int_t, lh_math_min(count, lh_cast_static(lh_usize_t, LH_INT_T_MAX)));
    /* No WC_ERR_INVALID_CHARS: it is Vista+, and lh still builds for XP. */
    const lh_int_t bytes = WideCharToMultiByte(LH_OS_SYSTEM_WIN_CP_UTF8, 0UL, text, units, lh_null,
                                               0, lh_null, lh_null);
    if (lh_math_is_zero(bytes))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_str_ptr const buf = lh_ptr_cast(lh_char_t, lh_os_alloc(lh_cast_static(lh_usize_t, bytes)));
    if (lh_null_eq(buf))
    {
        return lh_bool_false;
    }
    const lh_bool_t ok = lh_cast_static(
        lh_bool_t, !lh_math_is_zero(WideCharToMultiByte(LH_OS_SYSTEM_WIN_CP_UTF8, 0UL, text, units,
                                                        buf, bytes, lh_null, lh_null)));
    if (ok)
    {
        lh_str_clear(out);
        lh_str_append(out, buf, lh_cast_static(lh_usize_t, bytes));
    }
    else
    {
        lh_os_system_error_capture();
    }
    lh_runtime_allocator_free(buf);
    return ok;
}
