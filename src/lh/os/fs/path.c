#include <lh/os/fs/path.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/fs/file.h>
#include <lh/memory.h>
#include <lh/str/view.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#elif LH_COMPILER_OS == LH_COMPILER_OS_MAC
#    include <mach-o/dyld.h>
#    include <sys/stat.h>
#    include <unistd.h>
#else
#    include <sys/stat.h>
#    include <unistd.h>
#endif

#define LH_OS_FS_FILETIME_UNIX_EPOCH 116444736000000000ULL
#define LH_OS_FS_FILETIME_HZ 10000000ULL

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    ifndef IO_REPARSE_TAG_SYMLINK
#        define IO_REPARSE_TAG_SYMLINK 0xA000000CUL
#    endif
#endif

typedef struct
{
    lh_usize_t offset;
    lh_usize_t size;
} lh_os_fs_path_span_t;

static const lh_u8_t lh_os_fs_path_shortcut_magic[20] = {
    0x4CU, 0x00U, 0x00U, 0x00U, 0x01U, 0x14U, 0x02U, 0x00U, 0x00U, 0x00U,
    0x00U, 0x00U, 0xC0U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x46U};

static void
lh_os_fs_path_fail_empty(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("path is empty"));
}

static void
lh_os_fs_path_fail_too_small(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("path buffer is too small"));
}

static void
lh_os_fs_path_fail_kind(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("kind is invalid"));
}

lh_char_t *
lh_os_fs_path_get_sep(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sep);
}

const lh_char_t *
lh_os_fs_path_get_sep_as_const(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sep);
}

lh_str_t *
lh_os_fs_path_get_text(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

const lh_str_t *
lh_os_fs_path_get_text_as_const(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_vector_t *
lh_os_fs_path_get_parts(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->parts);
}

const lh_vector_t *
lh_os_fs_path_get_parts_as_const(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->parts);
}

static const lh_os_fs_path_span_t *
lh_os_fs_path_get_span(const lh_os_fs_path_t *self, lh_uindex_t index)
{
    return lh_ptr_cast(const lh_os_fs_path_span_t,
                       lh_vector_get_ptr(lh_os_fs_path_get_parts_as_const(self), index));
}

lh_str_view_t
lh_os_fs_path_get_part(const lh_os_fs_path_t *self, lh_uindex_t index)
{
    const lh_os_fs_path_span_t *span;
    lh_str_view_t view;

    span = lh_os_fs_path_get_span(self, index);
    if (span->size == 0U)
    {
        lh_str_view_init_empty(lh_addr_of(view));
        return view;
    }
    lh_str_init_by_size(
        lh_addr_of(view),
        lh_ptr_add_by_offset(const lh_char_t,
                             lh_str_get_data(lh_os_fs_path_get_text_as_const(self)), span->offset),
        span->size);
    return view;
}

lh_str_view_t
lh_os_fs_path_as_view(const lh_os_fs_path_t *self)
{
    const lh_str_t *text;
    lh_str_view_t view;

    text = lh_os_fs_path_get_text_as_const(self);
    if (lh_str_is_empty(text))
    {
        lh_str_view_init_empty(lh_addr_of(view));
        return view;
    }
    return lh_str_as_view(text);
}

lh_bool_t
lh_os_fs_path_is_empty(const lh_os_fs_path_t *self)
{
    return lh_vector_is_empty(lh_os_fs_path_get_parts_as_const(self));
}

static lh_str_cptr
lh_os_fs_path_cstr(const lh_os_fs_path_t *self)
{
    return lh_str_get_data(lh_os_fs_path_get_text_as_const(self));
}

static lh_bool_t
lh_os_fs_path_require(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_os_fs_path_is_empty(self))
    {
        lh_os_fs_path_fail_empty();
        return lh_bool_false;
    }
    return lh_bool_true;
}

static lh_bool_t
lh_os_fs_path_is_sep(lh_char_t ch)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return lh_cast_static(lh_bool_t, ch == '\\' || ch == '/');
#else
    return lh_cast_static(lh_bool_t, ch == '/');
#endif
}

static lh_bool_t
lh_os_fs_path_is_letter(lh_char_t ch)
{
    return lh_cast_static(lh_bool_t, (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

static lh_bool_t
lh_os_fs_path_is_drive_view(lh_str_view_t part)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_str_view_get_size(lh_addr_of(part)) != 2U)
    {
        return lh_bool_false;
    }
    return lh_cast_static(lh_bool_t,
                          lh_os_fs_path_is_letter(lh_str_view_get_char_from_begin(lh_addr_of(part), 0U)) &&
                              lh_str_view_get_char_from_begin(lh_addr_of(part), 1U) == ':');
#else
    (void)part;
    return lh_bool_false;
#endif
}

lh_char_t
lh_os_fs_path_sep(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return '\\';
#else
    return '/';
#endif
}

static lh_bool_t
lh_os_fs_path_text_ends_with_sep(const lh_os_fs_path_t *self)
{
    lh_str_view_t view;

    view = lh_os_fs_path_as_view(self);
    if (lh_str_view_is_empty(lh_addr_of(view)))
    {
        return lh_bool_false;
    }
    return lh_os_fs_path_is_sep(lh_str_view_get_char_from_end(lh_addr_of(view), 0U));
}

static lh_bool_t
lh_os_fs_path_has_root_suffix(const lh_os_fs_path_t *self)
{
    lh_str_view_t first;

    if (lh_vector_get_size(lh_os_fs_path_get_parts_as_const(self)) != 1U)
    {
        return lh_bool_false;
    }
    if (!lh_os_fs_path_text_ends_with_sep(self))
    {
        return lh_bool_false;
    }
    first = lh_os_fs_path_get_part(self, 0U);
    return lh_cast_static(lh_bool_t, lh_str_view_is_empty(lh_addr_of(first)) ||
                                         lh_os_fs_path_is_drive_view(first));
}

static void
lh_os_fs_path_append_part(lh_os_fs_path_t *self, lh_str_view_t piece)
{
    lh_str_t *text;
    lh_os_fs_path_span_t span;
    lh_usize_t n;

    text = lh_os_fs_path_get_text(self);
    if (!lh_vector_is_empty(lh_os_fs_path_get_parts_as_const(self)) &&
        !lh_os_fs_path_has_root_suffix(self))
    {
        lh_str_push_back(text, lh_ptr_deref(lh_os_fs_path_get_sep_as_const(self)));
    }
    n = lh_str_view_is_empty(lh_addr_of(piece)) ? 0U : lh_str_view_get_size(lh_addr_of(piece));
    span.offset = lh_str_get_size(text);
    span.size = n;
    if (n > 0U)
    {
        lh_str_append(text, lh_str_view_get_data(lh_addr_of(piece)), n);
    }
    lh_vector_push_back(lh_os_fs_path_get_parts(self), lh_addr_of(span));
}

static void
lh_os_fs_path_finish_singleton(lh_os_fs_path_t *self)
{
    lh_str_view_t part;

    if (lh_vector_get_size(lh_os_fs_path_get_parts_as_const(self)) != 1U)
    {
        return;
    }
    part = lh_os_fs_path_get_part(self, 0U);
    if (lh_str_view_is_empty(lh_addr_of(part)) || lh_os_fs_path_is_drive_view(part))
    {
        if (!lh_os_fs_path_text_ends_with_sep(self))
        {
            lh_str_push_back(lh_os_fs_path_get_text(self),
                             lh_ptr_deref(lh_os_fs_path_get_sep_as_const(self)));
        }
    }
}

static lh_bool_t
lh_os_fs_path_commit(lh_os_fs_path_t *self)
{
    if (lh_str_get_size(lh_os_fs_path_get_text_as_const(self)) + 1U > LH_OS_FS_PATH_MAX)
    {
        lh_os_fs_path_clear(self);
        lh_os_fs_path_fail_too_small();
        return lh_bool_false;
    }
    return lh_bool_true;
}

void
lh_os_fs_path_clear(lh_os_fs_path_t *self)
{
    lh_str_clear(lh_os_fs_path_get_text(self));
    lh_vector_clear(lh_os_fs_path_get_parts(self));
}

void
lh_os_fs_path_init(lh_os_fs_path_t *self)
{
    lh_ptr_deref(lh_os_fs_path_get_sep(self)) = lh_os_fs_path_sep();
    lh_str_init(lh_os_fs_path_get_text(self));
    lh_vector_init(lh_os_fs_path_get_parts(self), sizeof(lh_os_fs_path_span_t));
}

void
lh_os_fs_path_deinit(lh_os_fs_path_t *self)
{
    lh_str_deinit(lh_os_fs_path_get_text(self));
    lh_vector_deinit(lh_os_fs_path_get_parts(self));
}

void
lh_os_fs_path_assign(lh_os_fs_path_t *self, const lh_os_fs_path_t *other)
{
    const lh_str_t *src;
    const lh_vector_t *parts;
    lh_usize_t n;

    lh_assert_runtime_ref(other);
    if (self == other)
    {
        return;
    }
    lh_os_fs_path_clear(self);
    lh_ptr_deref(lh_os_fs_path_get_sep(self)) =
        lh_ptr_deref(lh_os_fs_path_get_sep_as_const(other));
    src = lh_os_fs_path_get_text_as_const(other);
    if (!lh_str_is_empty(src))
    {
        lh_str_append(lh_os_fs_path_get_text(self), lh_str_get_data(src), lh_str_get_size(src));
    }
    parts = lh_os_fs_path_get_parts_as_const(other);
    n = lh_vector_get_size(parts);
    if (n > 0U)
    {
        lh_vector_push_back_of(lh_os_fs_path_get_parts(self), lh_vector_get_data(parts), n);
    }
}

lh_bool_t
lh_os_fs_path_set(lh_os_fs_path_t *self, lh_str_view_t text)
{
    lh_str_cptr data;
    lh_usize_t n;
    lh_usize_t i;

    lh_os_fs_path_clear(self);
    if (lh_str_view_is_empty(lh_addr_of(text)))
    {
        return lh_bool_true;
    }

    data = lh_str_view_get_data(lh_addr_of(text));
    n = lh_str_view_get_size(lh_addr_of(text));
    i = 0U;

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (n >= 2U && lh_ptr_deref(lh_ptr_add_by_offset(const lh_char_t, data, 1U)) == ':' &&
        lh_os_fs_path_is_letter(lh_ptr_deref(data)))
    {
        lh_str_view_t drive;

        lh_str_init_by_size(lh_addr_of(drive), data, 2U);
        lh_os_fs_path_append_part(self, drive);
        i = 2U;
        if (i < n && lh_os_fs_path_is_sep(lh_ptr_deref(lh_ptr_add_by_offset(const lh_char_t, data, i))))
        {
            i += 1U;
        }
    }
#endif

    while (i < n && lh_os_fs_path_is_sep(lh_ptr_deref(lh_ptr_add_by_offset(const lh_char_t, data, i))))
    {
        lh_str_view_t empty;

        lh_str_view_init_empty(lh_addr_of(empty));
        lh_os_fs_path_append_part(self, empty);
        i += 1U;
    }

    while (i < n)
    {
        lh_usize_t start;
        lh_str_view_t piece;

        start = i;
        while (i < n &&
               !lh_os_fs_path_is_sep(lh_ptr_deref(lh_ptr_add_by_offset(const lh_char_t, data, i))))
        {
            i += 1U;
        }
        if (i > start)
        {
            lh_str_init_by_size(lh_addr_of(piece),
                                lh_ptr_add_by_offset(const lh_char_t, data, start), i - start);
            lh_os_fs_path_append_part(self, piece);
        }
        if (i < n)
        {
            i += 1U;
        }
    }

    lh_os_fs_path_finish_singleton(self);
    return lh_os_fs_path_commit(self);
}

static lh_bool_t
lh_os_fs_path_from_os_buf(lh_os_fs_path_t *out, lh_str_cptr buf)
{
    return lh_os_fs_path_set(out, lh_str_view_make(buf));
}

lh_bool_t
lh_os_fs_path_exe(lh_os_fs_path_t *out)
{
    lh_char_t buf[LH_OS_FS_PATH_MAX];

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD cap;
        DWORD n;

        cap = LH_OS_FS_PATH_MAX > 0xFFFFFFFFU ? 0xFFFFFFFFU
                                              : lh_cast_static(DWORD, LH_OS_FS_PATH_MAX);
        n = GetModuleFileNameA(lh_null, buf, cap);
        if (n == 0)
        {
            lh_os_fs_path_clear(out);
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (n >= cap)
        {
            lh_os_fs_path_clear(out);
            lh_os_fs_path_fail_too_small();
            return lh_bool_false;
        }
        return lh_os_fs_path_from_os_buf(out, buf);
    }
#elif LH_COMPILER_OS == LH_COMPILER_OS_MAC
    {
        uint32_t cap = lh_cast_static(uint32_t, LH_OS_FS_PATH_MAX);

        if (_NSGetExecutablePath(buf, lh_addr_of(cap)) != 0)
        {
            lh_os_fs_path_clear(out);
            lh_os_fs_path_fail_too_small();
            return lh_bool_false;
        }
        return lh_os_fs_path_from_os_buf(out, buf);
    }
#else
    {
        lh_ssize_t n;

        n = lh_cast_static(lh_ssize_t, readlink("/proc/self/exe", buf, LH_OS_FS_PATH_MAX - 1U));
        if (n <= 0)
        {
            lh_os_fs_path_clear(out);
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        lh_ptr_deref(lh_ptr_add_by_offset(lh_char_t, buf, lh_cast_static(lh_usize_t, n))) = '\0';
        return lh_os_fs_path_from_os_buf(out, buf);
    }
#endif
}

static lh_bool_t
lh_os_fs_path_drop_last(lh_os_fs_path_t *self)
{
    const lh_vector_t *parts;
    lh_usize_t n;
    lh_os_fs_path_t tmp;
    lh_uindex_t i;
    lh_str_view_t first;

    parts = lh_os_fs_path_get_parts_as_const(self);
    n = lh_vector_get_size(parts);
    if (n == 0U)
    {
        lh_os_fs_path_fail_empty();
        return lh_bool_false;
    }
    if (n == 1U)
    {
        first = lh_os_fs_path_get_part(self, 0U);
        if (lh_str_view_is_empty(lh_addr_of(first)) || lh_os_fs_path_is_drive_view(first))
        {
            return lh_bool_true;
        }
        return lh_os_fs_path_set(self, lh_str_view_lit("."));
    }

    lh_os_fs_path_init(lh_addr_of(tmp));
    lh_ptr_deref(lh_os_fs_path_get_sep(lh_addr_of(tmp))) =
        lh_ptr_deref(lh_os_fs_path_get_sep_as_const(self));
    for (i = 0U; i + 1U < n; ++i)
    {
        lh_os_fs_path_append_part(lh_addr_of(tmp), lh_os_fs_path_get_part(self, i));
    }
    lh_os_fs_path_finish_singleton(lh_addr_of(tmp));
    if (!lh_os_fs_path_commit(lh_addr_of(tmp)))
    {
        lh_os_fs_path_deinit(lh_addr_of(tmp));
        lh_os_fs_path_clear(self);
        return lh_bool_false;
    }
    lh_os_fs_path_assign(self, lh_addr_of(tmp));
    lh_os_fs_path_deinit(lh_addr_of(tmp));
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_path_exe_dir(lh_os_fs_path_t *out)
{
    if (!lh_os_fs_path_exe(out))
    {
        return lh_bool_false;
    }
    return lh_os_fs_path_drop_last(out);
}

lh_bool_t
lh_os_fs_path_dir(const lh_os_fs_path_t *path, lh_os_fs_path_t *out)
{
    if (!lh_os_fs_path_require(path))
    {
        lh_os_fs_path_clear(out);
        return lh_bool_false;
    }
    lh_os_fs_path_assign(out, path);
    return lh_os_fs_path_drop_last(out);
}

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static lh_bool_t
lh_os_fs_path_attrs(lh_str_cptr cstr, DWORD *attrs)
{
    WIN32_FILE_ATTRIBUTE_DATA info;

    if (!GetFileAttributesExA(cstr, GetFileExInfoStandard, lh_addr_of(info)))
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
    *attrs = info.dwFileAttributes;
    return lh_bool_true;
}
#endif

lh_bool_t
lh_os_fs_path_is(const lh_os_fs_path_t *path, lh_os_fs_kind_t kind)
{
    lh_str_cptr cstr;

    if (kind == lh_os_fs_kind_other)
    {
        return lh_bool_false;
    }
    if (kind != lh_os_fs_kind_file && kind != lh_os_fs_kind_dir &&
        kind != lh_os_fs_kind_symlink && kind != lh_os_fs_kind_shortcut)
    {
        lh_os_fs_path_fail_kind();
        return lh_bool_false;
    }
    if (!lh_os_fs_path_require(path))
    {
        return lh_bool_false;
    }

    if (kind == lh_os_fs_kind_shortcut)
    {
        lh_os_fs_file_t file;
        lh_u8_t buf[20];
        lh_ssize_t n;

        if (!lh_os_fs_path_is(path, lh_os_fs_kind_file))
        {
            return lh_bool_false;
        }
        lh_os_fs_file_init(lh_addr_of(file));
        if (!lh_os_fs_file_open(lh_addr_of(file), path, lh_os_fs_file_mode_read))
        {
            return lh_bool_false;
        }
        n = lh_os_fs_file_read(lh_addr_of(file), buf, sizeof(buf));
        lh_os_fs_file_close(lh_addr_of(file));
        if (n != lh_cast_static(lh_ssize_t, sizeof(buf)))
        {
            return lh_bool_false;
        }
        return lh_memory_compare(buf, sizeof(buf),
                                 lh_cast_const(lh_ptr, lh_os_fs_path_shortcut_magic),
                                 sizeof(lh_os_fs_path_shortcut_magic)) == lh_null
                   ? lh_bool_true
                   : lh_bool_false;
    }

    cstr = lh_os_fs_path_cstr(path);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD attrs;

        if (!lh_os_fs_path_attrs(cstr, lh_addr_of(attrs)))
        {
            return lh_bool_false;
        }
        if (kind == lh_os_fs_kind_dir)
        {
            return lh_cast_static(lh_bool_t, (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0U);
        }
        if (kind == lh_os_fs_kind_file)
        {
            return lh_cast_static(lh_bool_t, (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0U);
        }
        if ((attrs & FILE_ATTRIBUTE_REPARSE_POINT) == 0U)
        {
            return lh_bool_false;
        }
        {
            WIN32_FIND_DATAA data;
            HANDLE find;

            find = FindFirstFileA(cstr, lh_addr_of(data));
            if (find == INVALID_HANDLE_VALUE)
            {
                lh_os_capture_last_error();
                return lh_bool_false;
            }
            FindClose(find);
            return lh_cast_static(lh_bool_t, data.dwReserved0 == IO_REPARSE_TAG_SYMLINK);
        }
    }
#else
    {
        struct stat info;

        if (kind == lh_os_fs_kind_symlink)
        {
            if (lstat(cstr, lh_addr_of(info)) != 0)
            {
                lh_os_capture_last_error();
                return lh_bool_false;
            }
            return lh_cast_static(lh_bool_t, S_ISLNK(info.st_mode));
        }
        if (stat(cstr, lh_addr_of(info)) != 0)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (kind == lh_os_fs_kind_dir)
        {
            return lh_cast_static(lh_bool_t, S_ISDIR(info.st_mode));
        }
        return lh_cast_static(lh_bool_t, S_ISREG(info.st_mode));
    }
#endif
}

lh_bool_t
lh_os_fs_path_is_dir(const lh_os_fs_path_t *path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_dir);
}

lh_bool_t
lh_os_fs_path_is_file(const lh_os_fs_path_t *path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_file);
}

lh_bool_t
lh_os_fs_path_is_symlink(const lh_os_fs_path_t *path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_symlink);
}

lh_bool_t
lh_os_fs_path_is_shortcut(const lh_os_fs_path_t *path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_shortcut);
}

lh_bool_t
lh_os_fs_path_join(lh_os_fs_path_t *out, const lh_os_fs_path_t *dir, const lh_os_fs_path_t *name)
{
    lh_os_fs_path_t tmp;
    lh_usize_t n;
    lh_uindex_t i;

    lh_assert_runtime_ref(dir);
    if (!lh_os_fs_path_require(name))
    {
        lh_os_fs_path_clear(out);
        return lh_bool_false;
    }

    lh_os_fs_path_init(lh_addr_of(tmp));
    lh_os_fs_path_assign(lh_addr_of(tmp), dir);
    n = lh_vector_get_size(lh_os_fs_path_get_parts_as_const(name));
    for (i = 0U; i < n; ++i)
    {
        lh_os_fs_path_append_part(lh_addr_of(tmp), lh_os_fs_path_get_part(name, i));
    }
    lh_os_fs_path_finish_singleton(lh_addr_of(tmp));
    if (!lh_os_fs_path_commit(lh_addr_of(tmp)))
    {
        lh_os_fs_path_deinit(lh_addr_of(tmp));
        lh_os_fs_path_clear(out);
        return lh_bool_false;
    }
    lh_os_fs_path_assign(out, lh_addr_of(tmp));
    lh_os_fs_path_deinit(lh_addr_of(tmp));
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_path_mtime(const lh_os_fs_path_t *path, lh_s64_t *out)
{
    lh_str_cptr cstr;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_path_require(path))
    {
        return lh_bool_false;
    }
    cstr = lh_os_fs_path_cstr(path);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        WIN32_FILE_ATTRIBUTE_DATA info;
        ULARGE_INTEGER ticks;

        if (!GetFileAttributesExA(cstr, GetFileExInfoStandard, lh_addr_of(info)))
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        ticks.LowPart = info.ftLastWriteTime.dwLowDateTime;
        ticks.HighPart = info.ftLastWriteTime.dwHighDateTime;
        if (ticks.QuadPart < LH_OS_FS_FILETIME_UNIX_EPOCH)
        {
            *out = 0;
            return lh_bool_true;
        }
        *out = lh_cast_static(lh_s64_t,
                              (ticks.QuadPart - LH_OS_FS_FILETIME_UNIX_EPOCH) / LH_OS_FS_FILETIME_HZ);
        return lh_bool_true;
    }
#else
    {
        struct stat info;

        if (stat(cstr, lh_addr_of(info)) != 0)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        *out = lh_cast_static(lh_s64_t, info.st_mtime);
        return lh_bool_true;
    }
#endif
}

lh_bool_t
lh_os_fs_path_read(const lh_os_fs_path_t *path, lh_ptr buf, lh_usize_t buf_size, lh_usize_t *out_size)
{
    lh_os_fs_file_t file;
    lh_u64_t size;

    lh_assert_runtime_ref(out_size);

    lh_os_fs_file_init(lh_addr_of(file));
    if (!lh_os_fs_file_open(lh_addr_of(file), path, lh_os_fs_file_mode_read))
    {
        return lh_bool_false;
    }
    if (!lh_os_fs_file_get_size(lh_addr_of(file), lh_addr_of(size)))
    {
        lh_os_fs_file_close(lh_addr_of(file));
        return lh_bool_false;
    }
    if (size > buf_size)
    {
        lh_os_fs_file_close(lh_addr_of(file));
        lh_os_fs_path_fail_too_small();
        return lh_bool_false;
    }
    if (!lh_os_fs_file_read_all(lh_addr_of(file), buf, lh_cast_static(lh_usize_t, size)))
    {
        lh_os_fs_file_close(lh_addr_of(file));
        return lh_bool_false;
    }
    lh_os_fs_file_close(lh_addr_of(file));
    *out_size = lh_cast_static(lh_usize_t, size);
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_path_remove(const lh_os_fs_path_t *path)
{
    lh_str_cptr cstr;

    if (!lh_os_fs_path_require(path))
    {
        return lh_bool_false;
    }
    cstr = lh_os_fs_path_cstr(path);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (!DeleteFileA(cstr))
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
#else
    if (unlink(cstr) != 0)
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
#endif
    return lh_bool_true;
}
