/* RTLD_NOLOAD, RTLD_DI_LINKMAP, dladdr and dlinfo are GNU extensions. */
#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include <lh/os/system/shared.h>
#include <lh/core/attribute/unused.h>
#include <lh/os/system/posix/shared/search.h>
#include <lh/core/memory/std.h>
#include <lh/core/assert.h>
#include <lh/core/attribute/static.h>
#include <lh/os/alloc.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/str.h>
#include <lh/core/math.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/util/str/ptr/empty.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <link.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

lh_void
lh_os_system_shared_get_ext(lh_str_view_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_str_view_lit(".so");
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

/* Read exactly @p size bytes at @p offset of @p fd into @p out. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_shared_read_at(int fd, off_t offset, lh_ptr out, size_t size)
{
    lh_byte_t *bytes = lh_ptr_cast(lh_byte_t, out);
    size_t got = 0U;

    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        return lh_bool_false;
    }
    while (got < size)
    {
        const ssize_t n = read(fd, bytes + got, size - got);
        if (n <= 0)
        {
            return lh_bool_false;
        }
        got += (size_t)n;
    }
    return lh_bool_true;
}

lh_bool_t
lh_os_system_shared_get_code(lh_os_system_shared_handle_t handle, lh_ptr *begin, lh_usize_t *size)
{
    struct link_map *map = lh_null;

    lh_assert_runtime_ref(handle);
    if (dlinfo(handle, RTLD_DI_LINKMAP, lh_addr_of(map)) != 0 || lh_null_eq(map))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    /* dladdr identifies the mapped ELF headers; PT_LOAD marks executable code. */
    Dl_info info;
    if (lh_math_is_zero(dladdr(map->l_ld, lh_addr_of(info))) || lh_null_eq(info.dli_fbase))
    {
        return lh_bool_false;
    }
    const ElfW(Ehdr) *ehdr = (const ElfW(Ehdr) *)info.dli_fbase;
    if (lh_null_ne(lh_memory_std_compare(ehdr->e_ident, ELFMAG, SELFMAG)))
    {
        return lh_bool_false;
    }
    const ElfW(Phdr) *phdr = (const ElfW(Phdr) *)((const lh_byte_t *)ehdr + ehdr->e_phoff);
    for (lh_usize_t i = 0U; lh_math_lt(i, (lh_usize_t)ehdr->e_phnum); i = lh_math_add_one(i))
    {
        if (phdr[i].p_type == PT_LOAD && !lh_math_is_zero(lh_bit_and(phdr[i].p_flags, PF_X)))
        {
            lh_ptr_deref(begin) = (lh_ptr)(uintptr_t)(map->l_addr + phdr[i].p_vaddr);
            lh_ptr_deref(size) = (lh_usize_t)phdr[i].p_filesz;
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_str_cptr
lh_os_system_shared_search_get_name(const struct lh_os_system_shared_search *self)
{
    lh_assert_runtime_ref(self);
    return self->name;
}

lh_void
lh_os_system_shared_search_set_name(struct lh_os_system_shared_search *self, lh_str_cptr name)
{
    lh_assert_runtime_ref(self);
    self->name = name;
}

lh_str_cptr
lh_os_system_shared_search_get_path(const struct lh_os_system_shared_search *self)
{
    lh_assert_runtime_ref(self);
    return self->path;
}

lh_void
lh_os_system_shared_search_set_path(struct lh_os_system_shared_search *self, lh_str_cptr path)
{
    lh_assert_runtime_ref(self);
    self->path = path;
}

/* True when `base` is where the executable is mapped. dladdr may name it
   by argv[0], which dlopen(RTLD_NOLOAD) does not find; only dlopen(NULL) does. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_shared_is_executable(lh_ptr base)
{
    struct link_map *map = lh_null;

    const lh_os_system_shared_handle_t program = lh_os_system_shared_get_executable();
    if (lh_null_eq(program))
    {
        return lh_bool_false;
    }
    Dl_info info;
    const lh_bool_t is_executable =
        lh_math_is_zero(dlinfo(program, RTLD_DI_LINKMAP, lh_addr_of(map))) && lh_null_ne(map) &&
        lh_math_ne(dladdr(map->l_ld, lh_addr_of(info)), 0) && lh_math_eq(info.dli_fbase, base);
    lh_os_system_shared_close(program);
    return is_executable;
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

/* dl_iterate_phdr callback: stop at the image whose file name is
 * lh_os_system_shared_search_get_name(search). */
LH_ATTRIBUTE_STATIC
int
lh_os_system_shared_match(struct dl_phdr_info *info, size_t size, void *data)
{
    struct lh_os_system_shared_search *const search =
        lh_ptr_cast(struct lh_os_system_shared_search, data);

    LH_ATTRIBUTE_UNUSED(size);
    if (lh_str_ptr_is_empty(info->dlpi_name))
    {
        return 0;
    }
    lh_str_cptr const slash = strrchr(info->dlpi_name, lh_char_map_slash);
    lh_str_cptr const file = lh_null_eq(slash) ? info->dlpi_name : slash + 1;
    if (strcmp(file, lh_os_system_shared_search_get_name(search)) != 0)
    {
        return 0;
    }
    lh_os_system_shared_search_set_path(search, info->dlpi_name);
    return 1;
}

lh_os_system_shared_handle_t
lh_os_system_shared_get_by_name(lh_str_cptr name)
{
    lh_assert_runtime_ref(name);
    lh_os_system_shared_handle_t handle = dlopen(name, RTLD_NOW | RTLD_NOLOAD);
    if (lh_null_eq(handle))
    {
        /* Loaded by a full path and without that SONAME, the image is not
           found by its bare name: look for its file among the loaded ones. */
        struct lh_os_system_shared_search search = {name, lh_null};
        dl_iterate_phdr(lh_os_system_shared_match, lh_addr_of(search));
        handle = lh_null_eq(lh_os_system_shared_search_get_path(lh_addr_of(search)))
                     ? lh_null
                     : dlopen(lh_os_system_shared_search_get_path(lh_addr_of(search)),
                              RTLD_NOW | RTLD_NOLOAD);
    }
    if (lh_null_eq(handle))
    {
        lh_os_system_error_capture();
    }
    return handle;
}

/* A heap copy of @p size bytes at @p offset of @p fd, or null. Free it with
   lh_runtime_allocator_free. */
LH_ATTRIBUTE_STATIC
lh_ptr
lh_os_system_shared_read_alloc(int fd, off_t offset, size_t size)
{
    lh_ptr const out = lh_math_is_zero(size) ? lh_null : lh_os_alloc((lh_usize_t)size);

    if (lh_null_ne(out) && !lh_os_system_shared_read_at(fd, offset, out, size))
    {
        lh_runtime_allocator_free(out);
        return lh_null;
    }
    return out;
}

lh_os_system_shared_handle_t
lh_os_system_shared_get_by_addr(lh_ptr addr)
{
    lh_assert_runtime_ref(addr);
    Dl_info info;
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

/* Walk the .symtab of the ELF file @p fd for @p name; its value plus @p bias. */
LH_ATTRIBUTE_STATIC
lh_ptr
lh_os_system_shared_find_in_symtab(int fd, uintptr_t bias, lh_str_cptr name)
{
    ElfW(Ehdr) ehdr;
    ElfW(Shdr) *shdrs = lh_null;
    ElfW(Sym) *syms = lh_null;
    char *strs = lh_null;
    lh_ptr result = lh_null;

    if (!lh_os_system_shared_read_at(fd, 0, lh_addr_of(ehdr), sizeof ehdr) ||
        lh_memory_std_compare(ehdr.e_ident, ELFMAG, SELFMAG) != 0 ||
        ehdr.e_shentsize != sizeof(ElfW(Shdr)))
    {
        return lh_null;
    }
    shdrs =
        lh_ptr_cast(ElfW(Shdr), lh_os_system_shared_read_alloc(fd, (off_t)ehdr.e_shoff,
                                                               sizeof(ElfW(Shdr)) * ehdr.e_shnum));
    for (size_t i = 0U; lh_null_ne(shdrs) && i < ehdr.e_shnum && lh_null_eq(syms); ++i)
    {
        const ElfW(Shdr) *const symtab = shdrs + i;
        if (symtab->sh_type != SHT_SYMTAB || symtab->sh_link >= ehdr.e_shnum ||
            symtab->sh_entsize != sizeof(ElfW(Sym)))
        {
            continue;
        }
        const ElfW(Shdr) *const strtab = shdrs + symtab->sh_link;
        syms = lh_ptr_cast(ElfW(Sym), lh_os_system_shared_read_alloc(fd, (off_t)symtab->sh_offset,
                                                                     symtab->sh_size));
        strs = lh_ptr_cast(
            char, lh_os_system_shared_read_alloc(fd, (off_t)strtab->sh_offset, strtab->sh_size));
        const size_t count = symtab->sh_size / sizeof(ElfW(Sym));

        for (size_t j = 0U; lh_null_ne(syms) && lh_null_ne(strs) && j < count; ++j)
        {
            if (syms[j].st_name != 0U && syms[j].st_value != 0U &&
                syms[j].st_name < strtab->sh_size && strcmp(strs + syms[j].st_name, name) == 0)
            {
                result = (lh_ptr)(bias + (uintptr_t)syms[j].st_value);
                break;
            }
        }
    }
    if (lh_null_ne(strs))
    {
        lh_runtime_allocator_free(strs);
    }
    if (lh_null_ne(syms))
    {
        lh_runtime_allocator_free(syms);
    }
    if (lh_null_ne(shdrs))
    {
        lh_runtime_allocator_free(shdrs);
    }
    return result;
}

lh_ptr
lh_os_system_shared_get_local_sym(lh_os_system_shared_handle_t handle, lh_str_cptr name)
{
    struct link_map *map = lh_null;

    lh_assert_runtime_ref(handle);
    lh_assert_runtime_ref(name);
    if (dlinfo(handle, RTLD_DI_LINKMAP, lh_addr_of(map)) != 0 || lh_null_eq(map))
    {
        lh_os_system_error_capture();
        return lh_null;
    }
    /* The program itself has no name in its link map. */
    lh_str_cptr const path = lh_str_ptr_is_empty(map->l_name) ? "/proc/self/exe" : map->l_name;
    const int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
    {
        lh_os_system_error_capture();
        return lh_null;
    }
    lh_ptr const result = lh_os_system_shared_find_in_symtab(fd, (uintptr_t)map->l_addr, name);
    close(fd);
    return result;
}
