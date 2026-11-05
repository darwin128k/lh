#include <lh/os/system/shared.h>
#include <lh/core/memory/std.h>
#include <lh/core/cast/const.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/assert.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/str.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/math.h>

/* The longest path Windows accepts, in UTF-16 units, NUL included. */
#define LH_OS_SYSTEM_WIN_SHARED_PATH_MAX 32768U

lh_void
lh_os_system_shared_get_ext(lh_str_view_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_str_view_lit(".dll");
}

lh_os_system_shared_handle_t
lh_os_system_shared_open(lh_str_cptr path)
{
    lh_assert_runtime_ref(path);
    lh_os_str_t os_path;
    const lh_bool_t ok = lh_os_system_str_init_by_utf8(lh_addr_of(os_path), path);
    const lh_os_system_win_handle_t native =
        ok ? LoadLibraryW(lh_os_str_get_data(lh_addr_of(os_path))) : lh_null;

    lh_os_str_deinit(lh_addr_of(os_path));
    if (lh_null_eq(native))
    {
        /* A failed conversion already set its own error. */
        if (ok)
        {
            lh_os_system_error_capture();
        }
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
    lh_assert_runtime_ref(name);
    lh_ptr const sym = GetProcAddress(handle, name);
    if (lh_null_eq(sym))
    {
        lh_os_system_error_capture();
    }
    return sym;
}

lh_os_system_shared_handle_t
lh_os_system_shared_get_executable(void)
{
    lh_os_system_win_handle_t native = lh_null;

    /* No UNCHANGED_REFCOUNT: the handle holds a reference, closed like any other. */
    if (lh_math_is_zero(GetModuleHandleExW(0UL, lh_null, lh_addr_of(native))))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_SHARED_HANDLE_INVALID;
    }
    return native;
}

lh_os_system_shared_handle_t
lh_os_system_shared_get_by_addr(lh_ptr addr)
{
    lh_os_system_win_handle_t native = lh_null;

    lh_assert_runtime_ref(addr);
    if (lh_math_is_zero(GetModuleHandleExW(LH_OS_SYSTEM_WIN_GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
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
    /* GetModuleFileName reports truncation as n == cap: grow and retry. */
    for (lh_os_system_win_dword_t cap = 256U; lh_math_le(cap, LH_OS_SYSTEM_WIN_SHARED_PATH_MAX);
         cap = lh_math_mul(cap, 2U))
    {
        lh_wstr_ptr const buf =
            lh_ptr_cast(lh_wchar_t, lh_os_alloc(lh_math_mul(cap, sizeof(lh_wchar_t))));
        if (lh_null_eq(buf))
        {
            return lh_bool_false;
        }
        const lh_os_system_win_dword_t n = GetModuleFileNameW(handle, buf, cap);
        if (lh_math_is_zero(n))
        {
            lh_os_system_error_capture();
            lh_runtime_allocator_free(buf);
            return lh_bool_false;
        }
        if (lh_math_lt(n, cap))
        {
            const lh_bool_t ok = lh_os_system_str_to_utf8(buf, lh_cast_static(lh_usize_t, n), out);
            lh_runtime_allocator_free(buf);
            return ok;
        }
        lh_runtime_allocator_free(buf);
    }
    lh_os_set_last_error_lit(lh_os_error_code_name_too_long, "image path is too long");
    return lh_bool_false;
}

lh_os_system_shared_handle_t
lh_os_system_shared_get_by_name(lh_str_cptr name)
{
    lh_os_str_t os_name;
    lh_os_system_win_handle_t native = lh_null;

    lh_assert_runtime_ref(name);
    const lh_bool_t ok = lh_os_system_str_init_by_utf8(lh_addr_of(os_name), name);
    /* Flags 0: take a reference, released by lh_os_system_shared_close. */
    const lh_bool_t found = lh_cast_static(
        lh_bool_t, ok && lh_math_ne(GetModuleHandleExW(0UL, lh_os_str_get_data(lh_addr_of(os_name)),
                                                       lh_addr_of(native)),
                                    0));
    lh_os_str_deinit(lh_addr_of(os_name));
    if (!found)
    {
        if (ok)
        {
            lh_os_system_error_capture();
        }
        return LH_OS_SYSTEM_SHARED_HANDLE_INVALID;
    }
    return native;
}

lh_bool_t
lh_os_system_shared_get_code(lh_os_system_shared_handle_t handle, lh_ptr *begin, lh_usize_t *size)
{
    lh_assert_runtime_ref(handle);
    /* The module handle is its base address: the DOS header ("MZ"), at e_lfanew
       the PE signature, the file header (section count at +6, optional header
       size at +20), then the section table: 40 bytes a section, its size at +8,
       its address at +12 and its characteristics at +36. */
    lh_byte_cptr base = lh_ptr_cast(const lh_byte_t, handle);
    if (lh_math_ne(base[0], 0x4DU) || lh_math_ne(base[1], 0x5AU))
    {
        return lh_bool_false;
    }
    lh_u32_t e_lfanew;
    lh_memory_std_copy(lh_addr_of(e_lfanew), base + 0x3C, sizeof(e_lfanew));
    lh_byte_cptr nt = base + e_lfanew;
    if (lh_math_ne(nt[0], 0x50U) || lh_math_ne(nt[1], 0x45U) || !lh_math_is_zero(nt[2]) ||
        !lh_math_is_zero(nt[3]))
    {
        return lh_bool_false;
    }
    lh_u16_t sections;
    lh_memory_std_copy(lh_addr_of(sections), nt + 6, sizeof(sections));
    lh_u16_t optional_size;
    lh_memory_std_copy(lh_addr_of(optional_size), nt + 20, sizeof(optional_size));
    lh_byte_cptr section = nt + 24 + optional_size;
    for (lh_u16_t i = 0U; lh_math_lt(i, sections); i = lh_math_add_one(i))
    {
        lh_u32_t characteristics;
        lh_memory_std_copy(lh_addr_of(characteristics), section + 36, sizeof(characteristics));
        if (!lh_math_is_zero(lh_bit_and(characteristics, 0x20000000UL))) /* MEM_EXECUTE */
        {
            lh_u32_t virtual_size;
            lh_memory_std_copy(lh_addr_of(virtual_size), section + 8, sizeof(virtual_size));
            lh_u32_t virtual_address;
            lh_memory_std_copy(lh_addr_of(virtual_address), section + 12, sizeof(virtual_address));
            lh_ptr_deref(begin) = lh_cast_const(lh_byte_t *, base + virtual_address);
            lh_ptr_deref(size) = virtual_size;
            return lh_bool_true;
        }
        section = section + 40;
    }
    return lh_bool_false;
}

lh_ptr
lh_os_system_shared_get_local_sym(lh_os_system_shared_handle_t handle, lh_str_cptr name)
{
    /* PE images carry no symbol table of their own; locals live in the .pdb. */
    LH_ATTRIBUTE_UNUSED(handle);
    LH_ATTRIBUTE_UNUSED(name);
    return lh_null;
}
