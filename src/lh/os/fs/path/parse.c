#include <lh/os/fs/path.h>
#include "local.h"
#include <lh/cast/static.h>
#include <lh/char/letter.h>
#include <lh/compiler/os.h>
#include <lh/str/view.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

static lh_bool_t
lh_os_fs_path_is_drive_view(lh_str_view_t part)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_str_view_get_size(lh_addr_of(part)) != 2U)
    {
        return lh_bool_false;
    }
    return (lh_char_is_letter(lh_str_view_get_char_from_begin(lh_addr_of(part), 0U)) &&
            lh_str_view_get_char_from_begin(lh_addr_of(part), 1U) == ':')
               ? lh_bool_true
               : lh_bool_false;
#else
    (void)part;
    return lh_bool_false;
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
    return (lh_str_view_is_empty(lh_addr_of(first)) || lh_os_fs_path_is_drive_view(first))
               ? lh_bool_true
               : lh_bool_false;
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
        lh_os_set_last_error(lh_os_error_code_too_small,
                             lh_os_error_desc_lit("path buffer is too small"));
        return lh_bool_false;
    }
    return lh_bool_true;
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
        lh_char_is_letter(lh_ptr_deref(data)))
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
        lh_os_set_last_error(lh_os_error_code_path_empty, lh_os_error_desc_lit("path is empty"));
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
