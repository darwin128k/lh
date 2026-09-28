/* RTLD_NOLOAD, RTLD_DI_LINKMAP, dladdr and dlinfo are GNU extensions. */
#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include <lh/os/system/shared.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/null.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/str.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr/empty.h>

#include <dlfcn.h>
#include <link.h>

lh_str_view_t
lh_os_system_shared_get_ext(void)
{
    return lh_str_view_lit(".so");
}

/* Name of the image behind `map`. dladdr on its own dynamic section also
   names the executable, whose l_name is empty. */
LH_ATTRIBUTE_STATIC
lh_str_cptr
lh_os_system_shared_map_name(const struct link_map *map)
{
    Dl_info info;

    if (!lh_str_ptr_is_empty(map->l_name))
    {
        return map->l_name;
    }
    if (lh_math_is_zero(dladdr(map->l_ld, lh_addr_of(info))) || lh_str_ptr_is_empty(info.dli_fname))
    {
        return lh_null;
    }
    return info.dli_fname;
}

/* True when `base` is where the executable is mapped. dladdr may name it
   by argv[0], which dlopen(RTLD_NOLOAD) does not find; only dlopen(NULL) does. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_shared_is_executable(lh_ptr base)
{
    struct link_map *map = lh_null;
    Dl_info info;

    const lh_os_system_shared_handle_t program = lh_os_system_shared_get_executable();
    if (lh_null_eq(program))
    {
        return lh_bool_false;
    }
    const lh_bool_t is_executable =
        lh_math_is_zero(dlinfo(program, RTLD_DI_LINKMAP, lh_addr_of(map))) && lh_null_ne(map) &&
        lh_math_ne(dladdr(map->l_ld, lh_addr_of(info)), 0) && lh_math_eq(info.dli_fbase, base);
    lh_os_system_shared_close(program);
    return is_executable;
}

lh_os_system_shared_handle_t
lh_os_system_shared_open(lh_str_cptr path)
{
    lh_assert_runtime_ref(path);
    const lh_os_system_shared_handle_t handle = dlopen(path, RTLD_NOW);
    if (lh_null_eq(handle))
    {
        lh_os_system_error_capture();
    }
    return handle;
}

lh_bool_t
lh_os_system_shared_close(lh_os_system_shared_handle_t handle)
{
    if (lh_math_ne(dlclose(handle), 0))
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
    lh_ptr const sym = dlsym(handle, name);
    if (lh_null_eq(sym))
    {
        lh_os_system_error_capture();
    }
    return sym;
}

lh_os_system_shared_handle_t
lh_os_system_shared_get_executable(void)
{
    const lh_os_system_shared_handle_t handle = dlopen(lh_null, RTLD_NOW);
    if (lh_null_eq(handle))
    {
        lh_os_system_error_capture();
    }
    return handle;
}

lh_os_system_shared_handle_t
lh_os_system_shared_get_by_addr(lh_ptr addr)
{
    Dl_info info;

    lh_assert_runtime_ref(addr);
    if (lh_math_is_zero(dladdr(addr, lh_addr_of(info))))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_SHARED_HANDLE_INVALID;
    }
    /* The executable is found only as dlopen(NULL), never by its name. */
    lh_str_cptr const name =
        lh_os_system_shared_is_executable(info.dli_fbase) || lh_str_ptr_is_empty(info.dli_fname)
            ? lh_null
            : info.dli_fname;
    const lh_os_system_shared_handle_t handle = dlopen(name, RTLD_NOW | RTLD_NOLOAD);
    if (lh_null_eq(handle))
    {
        lh_os_system_error_capture();
    }
    return handle;
}

lh_bool_t
lh_os_system_shared_get_path(lh_os_system_shared_handle_t handle, lh_str_t *out)
{
    struct link_map *map = lh_null;
    lh_str_cptr name = lh_null;

    if (lh_math_is_zero(dlinfo(handle, RTLD_DI_LINKMAP, lh_addr_of(map))) && lh_null_ne(map))
    {
        name = lh_os_system_shared_map_name(map);
    }
    if (lh_null_eq(name))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    return lh_os_system_str_ptr_to_utf8(name, out);
}
