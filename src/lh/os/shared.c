#include <lh/os/shared.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/str/view.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <dlfcn.h>
#endif

lh_os_shared_handle_t
lh_os_shared_open(lh_str_cptr path)
{
    lh_os_shared_handle_t handle;

    if (lh_null_eq(path))
    {
        lh_os_set_last_error(1, lh_str_view_make("path is null"));
        return lh_null;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    handle = (lh_os_shared_handle_t)LoadLibraryA(path);
    if (lh_null_eq(handle))
    {
        lh_os_capture_last_error();
        return lh_null;
    }
#else
    (void)dlerror();
    handle = (lh_os_shared_handle_t)dlopen(path, RTLD_NOW);
    if (lh_null_eq(handle))
    {
        lh_os_set_last_error(1, lh_str_view_make(dlerror()));
        return lh_null;
    }
#endif
    return handle;
}

lh_bool_t
lh_os_shared_close(lh_os_shared_handle_t handle)
{
    if (lh_null_eq(handle))
    {
        lh_os_set_last_error(1, lh_str_view_make("handle is null"));
        return lh_bool_false;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (!FreeLibrary((HMODULE)handle))
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
#else
    (void)dlerror();
    if (dlclose(handle) != 0)
    {
        lh_os_set_last_error(1, lh_str_view_make(dlerror()));
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
        lh_os_set_last_error(1, lh_str_view_make("handle or name is null"));
        return lh_null;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    sym = (lh_ptr)GetProcAddress((HMODULE)handle, name);
    if (lh_null_eq(sym))
    {
        lh_os_capture_last_error();
        return lh_null;
    }
#else
    (void)dlerror();
    sym = (lh_ptr)dlsym(handle, name);
    if (lh_null_eq(sym))
    {
        lh_os_set_last_error(1, lh_str_view_make(dlerror()));
        return lh_null;
    }
#endif
    return sym;
}

lh_bool_t
lh_os_shared_has_sym(lh_os_shared_handle_t handle, lh_str_cptr name)
{
    return lh_null_ne(lh_os_shared_get_sym(handle, name)) ? lh_bool_true : lh_bool_false;
}
