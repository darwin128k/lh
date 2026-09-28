/* RTLD_NOLOAD, RTLD_DI_LINKMAP, dladdr and dlinfo are GNU extensions. */
#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include <lh/os/system/shared.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/null.h>
#include <lh/os/system/error/capture.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

#include <dlfcn.h>
#include <link.h>

lh_str_view_t
lh_os_system_shared_ext(void)
{
    return lh_str_view_lit(".so");
}

/* NULL for an unnamed image: the main program's link_map name is "". */
LH_ATTRIBUTE_STATIC
lh_str_cptr
lh_os_system_shared_name_or_null(lh_str_cptr name)
{
    return lh_null_eq(name) || lh_math_eq(lh_ptr_deref(name), '\0') ? lh_null : name;
}

/* Name of the image behind `map`. dladdr on its own dynamic section also
   names the main program, whose l_name is empty. */
LH_ATTRIBUTE_STATIC
lh_str_cptr
lh_os_system_shared_map_name(const struct link_map *map)
{
    Dl_info info;

    if (lh_null_ne(lh_os_system_shared_name_or_null(map->l_name)))
    {
        return map->l_name;
    }
    if (lh_math_is_zero(dladdr(map->l_ld, lh_addr_of(info))))
    {
        return lh_null;
    }
    return lh_os_system_shared_name_or_null(info.dli_fname);
}

/* True when `base` is where the main program is mapped. dladdr may name it
   by argv[0], which dlopen(RTLD_NOLOAD) does not find; only dlopen(NULL) does. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_shared_is_main(lh_ptr base)
{
    lh_os_system_shared_handle_t self;
    struct link_map *map;
    Dl_info info;
    lh_bool_t is_main;

    self = dlopen(lh_null, RTLD_NOW);
    if (lh_null_eq(self))
    {
        return lh_bool_false;
    }
    map = lh_null;
    is_main = lh_math_is_zero(dlinfo(self, RTLD_DI_LINKMAP, lh_addr_of(map))) && lh_null_ne(map) &&
              lh_math_ne(dladdr(map->l_ld, lh_addr_of(info)), 0) && lh_math_eq(info.dli_fbase, base);
    dlclose(self);
    return is_main;
}

lh_os_system_shared_handle_t
lh_os_system_shared_open(lh_str_cptr path)
{
    lh_os_system_shared_handle_t handle;

    lh_assert_runtime_ref(path);
    handle = dlopen(path, RTLD_NOW);
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
lh_os_system_shared_sym(lh_os_system_shared_handle_t handle, lh_str_cptr name)
{
    lh_ptr sym;

    lh_assert_runtime_ref(name);
    sym = dlsym(handle, name);
    if (lh_null_eq(sym))
    {
        lh_os_system_error_capture();
    }
    return sym;
}

lh_os_system_shared_handle_t
lh_os_system_shared_of_addr(lh_ptr addr, lh_bool_t *owned)
{
    Dl_info info;
    lh_str_cptr name;
    lh_os_system_shared_handle_t handle;

    lh_assert_runtime_ref(addr);
    lh_ptr_deref(owned) = lh_bool_false;
    if (lh_math_is_zero(dladdr(addr, lh_addr_of(info))))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_SHARED_HANDLE_INVALID;
    }
    name = lh_os_system_shared_is_main(info.dli_fbase) ? lh_null : lh_os_system_shared_name_or_null(info.dli_fname);
    handle = dlopen(name, RTLD_NOW | RTLD_NOLOAD);
    if (lh_null_eq(handle))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_SHARED_HANDLE_INVALID;
    }
    lh_ptr_deref(owned) = lh_bool_true;
    return handle;
}

lh_bool_t
lh_os_system_shared_path(lh_os_system_shared_handle_t handle, lh_str_t *out)
{
    struct link_map *map;
    lh_str_cptr name;

    map = lh_null;
    name = lh_null;
    if (lh_math_is_zero(dlinfo(handle, RTLD_DI_LINKMAP, lh_addr_of(map))) && lh_null_ne(map))
    {
        name = lh_os_system_shared_map_name(map);
    }
    if (lh_null_eq(name))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_str_assign_view(out, lh_str_view_make(name));
    return lh_bool_true;
}
