#include <lh/os/fs/file/map.h>
#include "local.h"
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/fs/file.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#else
#    include <sys/mman.h>
#    include <unistd.h>
#endif

void
lh_os_fs_file_map_init(lh_os_fs_file_map_t *self)
{
    lh_assert_runtime_ref(self);
    self->data = lh_null;
    self->size = 0U;
    self->base = lh_null;
    self->base_size = 0U;
}

void
lh_os_fs_file_map_unmap(lh_os_fs_file_map_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_null_eq(self->base))
    {
        lh_os_fs_file_map_init(self);
        return;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    (void)UnmapViewOfFile(self->base);
#else
    (void)munmap(self->base, self->base_size);
#endif
    lh_os_fs_file_map_init(self);
}

lh_ptr
lh_os_fs_file_map_get_data(const lh_os_fs_file_map_t *self)
{
    lh_assert_runtime_ref(self);
    return self->data;
}

lh_usize_t
lh_os_fs_file_map_get_size(const lh_os_fs_file_map_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_bool_t
lh_os_fs_file_map_is_valid(const lh_os_fs_file_map_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_bool_t, lh_null_ne(self->base));
}

static lh_u64_t
lh_os_fs_file_map_granule(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        SYSTEM_INFO info;

        GetSystemInfo(lh_addr_of(info));
        return lh_cast_static(lh_u64_t, info.dwAllocationGranularity);
    }
#else
    {
        long page;

        page = sysconf(_SC_PAGESIZE);
        if (page <= 0)
        {
            return 4096U;
        }
        return lh_cast_static(lh_u64_t, page);
    }
#endif
}

lh_bool_t
lh_os_fs_file_map(const struct lh_os_fs_file *file, lh_u64_t offset, lh_u64_t size,
                  lh_os_fs_file_map_prot_t prot, lh_os_fs_file_map_t *out)
{
    lh_u64_t file_size;
    lh_u64_t length;
    lh_u64_t granule;
    lh_u64_t aligned;
    lh_u64_t slack;
    lh_u64_t map_len;
    lh_bool_t writable;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_file_is_valid(file))
    {
        lh_os_set_last_error(lh_os_error_code_not_open, lh_os_error_desc_lit("file is not open"));
        return lh_bool_false;
    }
    if (!lh_os_fs_file_get_size(file, lh_addr_of(file_size)))
    {
        return lh_bool_false;
    }
    if (offset > file_size)
    {
        lh_os_set_last_error(lh_os_error_code_invalid_offset,
                             lh_os_error_desc_lit("offset is past the file"));
        return lh_bool_false;
    }
    length = size == 0U ? file_size - offset : size;
    if (offset + length < offset || offset + length > file_size)
    {
        lh_os_set_last_error(lh_os_error_code_invalid_offset,
                             lh_os_error_desc_lit("map range is past the file"));
        return lh_bool_false;
    }
    if (length == 0U)
    {
        lh_os_fs_file_map_init(out);
        return lh_bool_true;
    }
    if (length > LH_USIZE_T_MAX)
    {
        lh_os_set_last_error(lh_os_error_code_too_small,
                             lh_os_error_desc_lit("map is larger than address space"));
        return lh_bool_false;
    }

    granule = lh_os_fs_file_map_granule();
    aligned = offset - (offset % granule);
    slack = offset - aligned;
    map_len = slack + length;
    writable = lh_os_fs_file_map_prot_has(prot, lh_os_fs_file_map_prot_write);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        HANDLE section;
        ULARGE_INTEGER view;
        DWORD protect;
        DWORD access;
        lh_ptr base;

        protect = writable ? PAGE_READWRITE : PAGE_READONLY;
        access = writable ? FILE_MAP_WRITE : FILE_MAP_READ;
        section = CreateFileMappingA(lh_os_fs_file_native_handle(file), lh_null, protect, 0, 0,
                                     lh_null);
        if (section == lh_null)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        view.QuadPart = aligned;
        base = MapViewOfFile(section, access, view.HighPart, view.LowPart,
                             lh_cast_static(SIZE_T, map_len));
        (void)CloseHandle(section);
        if (lh_null_eq(base))
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        out->base = base;
        out->base_size = lh_cast_static(lh_usize_t, map_len);
        out->data = lh_ptr_add_by_offset(lh_void, base, lh_cast_static(lh_usize_t, slack));
        out->size = lh_cast_static(lh_usize_t, length);
        return lh_bool_true;
    }
#else
    {
        int flags;
        lh_ptr base;
        off_t map_off;

        flags = PROT_READ;
        if (writable)
        {
            flags |= PROT_WRITE;
        }
        map_off = lh_cast_static(off_t, aligned);
        if (lh_cast_static(lh_u64_t, map_off) != aligned)
        {
            lh_os_set_last_error(lh_os_error_code_invalid_offset,
                                 lh_os_error_desc_lit("offset does not fit"));
            return lh_bool_false;
        }
        base = mmap(lh_null, lh_cast_static(size_t, map_len), flags, MAP_SHARED,
                    lh_os_fs_file_native_handle(file), map_off);
        if (base == MAP_FAILED)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        out->base = base;
        out->base_size = lh_cast_static(lh_usize_t, map_len);
        out->data = lh_ptr_add_by_offset(lh_void, base, lh_cast_static(lh_usize_t, slack));
        out->size = lh_cast_static(lh_usize_t, length);
        return lh_bool_true;
    }
#endif
}
