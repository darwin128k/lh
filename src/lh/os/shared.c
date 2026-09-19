#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include <lh/os/shared.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    ifndef GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
#        define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 0x00000004u
#    endif
#    ifndef GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT
#        define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT 0x00000002u
#    endif
#else
#    include <dlfcn.h>
#    ifndef RTLD_NOLOAD
#        define RTLD_NOLOAD 0
#    endif
#endif

lh_os_shared_handle_t
lh_os_shared_open(lh_str_cptr path)
{
    lh_os_shared_handle_t handle;

    if (lh_null_eq(path))
    {
        lh_os_set_last_error(1, lh_os_error_desc_lit("path is null"));
        return lh_null;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    handle = lh_cast_reinterpret(lh_os_shared_handle_t, LoadLibraryA(path));
    if (lh_null_eq(handle))
    {
        lh_os_capture_last_error();
        return lh_null;
    }
#else
    (void)dlerror();
    handle = lh_cast_reinterpret(lh_os_shared_handle_t, dlopen(path, RTLD_NOW));
    if (lh_null_eq(handle))
    {
        lh_os_set_last_error_cstr(1, dlerror());
        return lh_null;
    }
#endif
    return handle;
}

lh_os_shared_handle_t
lh_os_shared_of_addr(lh_ptr addr)
{
    lh_os_shared_handle_t handle;

    if (lh_null_eq(addr))
    {
        lh_os_set_last_error(1, lh_os_error_desc_lit("address is null"));
        return lh_null;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        HMODULE module = lh_null;

        if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                lh_ptr_cast(char, addr), lh_addr_of(module)))
        {
            lh_os_capture_last_error();
            return lh_null;
        }
        handle = lh_cast_reinterpret(lh_os_shared_handle_t, module);
    }
#else
    {
        Dl_info info;

        if (dladdr(addr, lh_addr_of(info)) == 0 || lh_null_eq(info.dli_fname))
        {
            lh_os_set_last_error(1, lh_os_error_desc_lit("address is not in a loaded image"));
            return lh_null;
        }
        (void)dlerror();
        handle = lh_cast_reinterpret(lh_os_shared_handle_t, dlopen(info.dli_fname, RTLD_NOW | RTLD_NOLOAD));
        if (lh_null_eq(handle))
        {
            lh_os_set_last_error_cstr(1, dlerror());
            return lh_null;
        }
    }
#endif
    return handle;
}

lh_bool_t
lh_os_shared_close(lh_os_shared_handle_t handle)
{
    if (lh_null_eq(handle))
    {
        lh_os_set_last_error(1, lh_os_error_desc_lit("handle is null"));
        return lh_bool_false;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (!FreeLibrary(lh_cast_reinterpret(HMODULE, handle)))
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
#else
    (void)dlerror();
    if (dlclose(handle) != 0)
    {
        lh_os_set_last_error_cstr(1, dlerror());
        return lh_bool_false;
    }
#endif
    return lh_bool_true;
}

lh_ptr
lh_os_shared_get_sym(lh_os_shared_handle_t handle, lh_str_cptr name)
{
    lh_ptr sym;

    if (lh_null_eq(handle) || lh_null_eq(name))
    {
        lh_os_set_last_error(1, lh_os_error_desc_lit("handle or name is null"));
        return lh_null;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    sym = lh_cast_reinterpret(lh_ptr, GetProcAddress(lh_cast_reinterpret(HMODULE, handle), name));
    if (lh_null_eq(sym))
    {
        lh_os_capture_last_error();
        return lh_null;
    }
#else
    (void)dlerror();
    sym = lh_cast_reinterpret(lh_ptr, dlsym(handle, name));
    if (lh_null_eq(sym))
    {
        lh_os_set_last_error_cstr(1, dlerror());
        return lh_null;
    }
#endif
    return sym;
}

lh_bool_t
lh_os_shared_has_sym(lh_os_shared_handle_t handle, lh_str_cptr name)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(lh_os_shared_get_sym(handle, name)));
}
