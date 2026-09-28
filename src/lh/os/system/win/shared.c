#include <lh/os/system/shared.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/alloc.h>
#include <lh/os/error/code.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/runtime/allocator.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

/* The longest path Windows accepts, in characters, NUL included. */
#define LH_OS_SYSTEM_WIN_SHARED_PATH_MAX 32768U

lh_str_view_t
lh_os_system_shared_get_ext(void)
{
    return lh_str_view_lit(".dll");
}

lh_os_system_shared_handle_t
lh_os_system_shared_open(lh_str_cptr path)
{
    lh_os_system_win_handle_t native;

    lh_assert_runtime_ref(path);
    native = LoadLibraryA(path);
    if (lh_null_eq(native))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_SHARED_HANDLE_INVALID;
    }
    return native;
}

lh_bool_t
lh_os_system_shared_close(lh_os_system_shared_handle_t handle)
{
    if (lh_math_is_zero(FreeLibrary(handle)))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_ptr
lh_os_system_shared_get_sym(lh_os_system_shared_handle_t handle, lh_str_cptr name)
{
    lh_ptr sym;

    lh_assert_runtime_ref(name);
    sym = GetProcAddress(handle, name);
    if (lh_null_eq(sym))
    {
        lh_os_system_error_capture();
    }
    return sym;
}

lh_os_system_shared_handle_t
lh_os_system_shared_get_executable(lh_bool_t *owned)
{
    lh_os_system_win_handle_t native;

    lh_ptr_deref(owned) = lh_bool_false;
    native = lh_null;
    if (lh_math_is_zero(GetModuleHandleExA(LH_OS_SYSTEM_WIN_GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, lh_null,
                                           lh_addr_of(native))))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_SHARED_HANDLE_INVALID;
    }
    return native;
}

lh_os_system_shared_handle_t
lh_os_system_shared_get_by_addr(lh_ptr addr, lh_bool_t *owned)
{
    lh_os_system_win_handle_t native;

    lh_assert_runtime_ref(addr);
    lh_ptr_deref(owned) = lh_bool_false;
    native = lh_null;
    if (lh_math_is_zero(GetModuleHandleExA(LH_OS_SYSTEM_WIN_GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                               LH_OS_SYSTEM_WIN_GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                           addr, lh_addr_of(native))))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_SHARED_HANDLE_INVALID;
    }
    return native;
}

lh_bool_t
lh_os_system_shared_get_path(lh_os_system_shared_handle_t handle, lh_str_t *out)
{
    lh_os_system_win_dword_t cap;
    lh_os_system_win_dword_t n;
    lh_str_ptr buf;

    /* GetModuleFileName reports truncation as n == cap: grow and retry. */
    for (cap = 256U; lh_math_le(cap, LH_OS_SYSTEM_WIN_SHARED_PATH_MAX); cap = lh_math_mul(cap, 2U))
    {
        buf = lh_ptr_cast(lh_char_t, lh_os_alloc(cap));
        if (lh_null_eq(buf))
        {
            return lh_bool_false;
        }
        n = GetModuleFileNameA(handle, buf, cap);
        if (lh_math_is_zero(n))
        {
            lh_os_system_error_capture();
            lh_runtime_allocator_free(buf);
            return lh_bool_false;
        }
        if (lh_math_lt(n, cap))
        {
            lh_str_clear(out);
            lh_str_append(out, buf, n);
            lh_runtime_allocator_free(buf);
            return lh_bool_true;
        }
        lh_runtime_allocator_free(buf);
    }
    lh_os_set_last_error_lit(lh_os_error_code_name_too_long, "image path is too long");
    return lh_bool_false;
}
