#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include <lh/os/shared.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/fs/path.h>
#include <lh/char.h>
#include <lh/str.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

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
#    include <link.h>
#    ifndef RTLD_NOLOAD
#        define RTLD_NOLOAD 0
#    endif
#    ifndef RTLD_DI_LINKMAP
#        define RTLD_DI_LINKMAP 2
#    endif
#endif

lh_os_shared_handle_t
lh_os_shared_open(const lh_os_fs_path_t *path)
{
    lh_os_shared_handle_t handle;
    lh_str_cptr cstr;

    lh_assert_runtime_ref(path);
    if (lh_os_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_code_path_empty, lh_os_error_desc_lit("path is empty"));
        return lh_null;
    }
    cstr = lh_str_get_data(lh_os_fs_path_get_text_as_const(path));
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    handle = lh_cast_reinterpret(lh_os_shared_handle_t, LoadLibraryA(cstr));
    if (lh_null_eq(handle))
    {
        lh_os_capture_last_error();
        return lh_null;
    }
#else
    (void)dlerror();
    handle = lh_cast_reinterpret(lh_os_shared_handle_t, dlopen(cstr, RTLD_NOW));
    if (lh_null_eq(handle))
    {
        lh_os_set_last_error_cstr(lh_os_error_code_failed, dlerror());
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
        lh_os_set_last_error(lh_os_error_code_null_pointer, lh_os_error_desc_lit("address is null"));
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
            lh_os_set_last_error(lh_os_error_code_not_in_image,
                                 lh_os_error_desc_lit("address is not in a loaded image"));
            return lh_null;
        }
        (void)dlerror();
        handle = lh_cast_reinterpret(lh_os_shared_handle_t, dlopen(info.dli_fname, RTLD_NOW | RTLD_NOLOAD));
        if (lh_null_eq(handle))
        {
            lh_os_set_last_error_cstr(lh_os_error_code_failed, dlerror());
            return lh_null;
        }
    }
#endif
    return handle;
}

static lh_bool_t
lh_os_shared_path_fill(lh_str_cptr src, lh_os_fs_path_t *out)
{
    if (lh_null_eq(src) || lh_ptr_deref(src) == '\0')
    {
        lh_os_fs_path_clear(out);
        lh_os_set_last_error(lh_os_error_code_path_empty, lh_os_error_desc_lit("path is empty"));
        return lh_bool_false;
    }
    return lh_os_fs_path_set(out, lh_str_view_make(src));
}

lh_bool_t
lh_os_shared_get_path_of(lh_os_shared_handle_t handle, lh_os_fs_path_t *out)
{
    lh_assert_runtime_ref(out);

    if (lh_null_eq(handle))
    {
        lh_os_fs_path_clear(out);
        lh_os_set_last_error(lh_os_error_code_null_pointer, lh_os_error_desc_lit("handle is null"));
        return lh_bool_false;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD cap;
        DWORD n;
        lh_char_t buf[LH_OS_FS_PATH_MAX];

        cap = LH_OS_FS_PATH_MAX > 0xFFFFFFFFU ? 0xFFFFFFFFU
                                              : lh_cast_static(DWORD, LH_OS_FS_PATH_MAX);
        n = GetModuleFileNameA(lh_cast_reinterpret(HMODULE, handle), buf, cap);
        if (n == 0)
        {
            lh_os_fs_path_clear(out);
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (n >= cap)
        {
            lh_os_fs_path_clear(out);
            lh_os_set_last_error(lh_os_error_code_too_small, lh_os_error_desc_lit("buffer too small"));
            return lh_bool_false;
        }
        return lh_os_fs_path_set(out, lh_str_view_make(buf));
    }
#else
    {
        struct link_map *map = lh_null;
        Dl_info info;
        lh_str_cptr src = lh_null;

        if (dlinfo(handle, RTLD_DI_LINKMAP, lh_addr_of(map)) == 0 && lh_null_ne(map) &&
            lh_null_ne(map->l_name) && map->l_name[0] != '\0')
        {
            src = map->l_name;
        }
        else if (dladdr(handle, lh_addr_of(info)) != 0)
        {
            src = info.dli_fname;
        }
        if (lh_null_eq(src))
        {
            lh_os_fs_path_clear(out);
            lh_os_set_last_error(lh_os_error_code_no_image_path,
                                 lh_os_error_desc_lit("handle has no image path"));
            return lh_bool_false;
        }
        return lh_os_shared_path_fill(src, out);
    }
#endif
}

lh_bool_t
lh_os_shared_get_path_of_addr(lh_ptr addr, lh_os_fs_path_t *out)
{
    lh_assert_runtime_ref(out);

    if (lh_null_eq(addr))
    {
        lh_os_fs_path_clear(out);
        lh_os_set_last_error(lh_os_error_code_null_pointer, lh_os_error_desc_lit("address is null"));
        return lh_bool_false;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        lh_os_shared_handle_t handle;

        handle = lh_os_shared_of_addr(addr);
        if (lh_null_eq(handle))
        {
            lh_os_fs_path_clear(out);
            return lh_bool_false;
        }
        return lh_os_shared_get_path_of(handle, out);
    }
#else
    {
        Dl_info info;

        if (dladdr(addr, lh_addr_of(info)) == 0)
        {
            lh_os_fs_path_clear(out);
            lh_os_set_last_error(lh_os_error_code_not_in_image,
                                 lh_os_error_desc_lit("address is not in a loaded image"));
            return lh_bool_false;
        }
        return lh_os_shared_path_fill(info.dli_fname, out);
    }
#endif
}

lh_bool_t
lh_os_shared_close(lh_os_shared_handle_t handle)
{
    if (lh_null_eq(handle))
    {
        lh_os_set_last_error(lh_os_error_code_null_pointer, lh_os_error_desc_lit("handle is null"));
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
        lh_os_set_last_error_cstr(lh_os_error_code_failed, dlerror());
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
        lh_os_set_last_error(lh_os_error_code_null_pointer,
                             lh_os_error_desc_lit("handle or name is null"));
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
        lh_os_set_last_error_cstr(lh_os_error_code_failed, dlerror());
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

lh_str_cptr
lh_os_shared_ext(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return ".dll";
#else
    return ".so";
#endif
}

lh_bool_t
lh_os_shared_is(const lh_str_view_t *name)
{
    lh_str_view_t ext;

    if (lh_null_eq(name) || lh_str_view_is_empty(name))
    {
        return lh_bool_false;
    }
    ext = lh_str_view_make(lh_os_shared_ext());
    return lh_str_view_ends_with(name, lh_addr_of(ext), lh_bool_true);
}

lh_bool_t
lh_os_shared_path_is(const lh_os_fs_path_t *path)
{
    const lh_vector_t *parts;
    lh_usize_t n;
    lh_str_view_t name;

    if (lh_null_eq(path) || lh_os_fs_path_is_empty(path))
    {
        return lh_bool_false;
    }
    parts = lh_os_fs_path_get_parts_as_const(path);
    n = lh_vector_get_size(parts);
    name = lh_os_fs_path_get_part(path, n - 1U);
    if (lh_str_view_is_empty(lh_addr_of(name)))
    {
        return lh_bool_false;
    }
    if (!lh_os_shared_is(lh_addr_of(name)))
    {
        return lh_bool_false;
    }
    if (lh_os_fs_path_is(path, lh_os_fs_kind_dir) || lh_os_fs_path_is(path, lh_os_fs_kind_shortcut))
    {
        return lh_bool_false;
    }
    return lh_os_fs_path_is(path, lh_os_fs_kind_file);
}
