#include <lh/os/fs/path.h>
#include "local.h"
#include <lh/char/letter.h>
#include <lh/char/map.h>
#include <lh/compiler/os.h>
#include <lh/memory/view.h>
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
            lh_str_view_get_char_from_begin(lh_addr_of(part), 1U) == lh_char_map_colon)
               ? lh_bool_true
               : lh_bool_false;
#else
    (void)part;
    return lh_bool_false;
#endif
}

static lh_bool_t
lh_os_fs_path_is_root_part(lh_str_view_t part)
{
    return (lh_str_view_is_empty(lh_addr_of(part)) || lh_os_fs_path_is_drive_view(part))
               ? lh_bool_true
               : lh_bool_false;
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
    return lh_os_fs_path_is_root_part(first);
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
    lh_str_append_view(text, piece);
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
    if (lh_os_fs_path_is_root_part(part))
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
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_too_small,
                             lh_os_error_desc_lit("path buffer is too small")));
        return lh_bool_false;
    }
    return lh_bool_true;
}

static lh_bool_t
lh_os_fs_path_append_path(lh_os_fs_path_t *self, const lh_os_fs_path_t *name)
{
    const lh_vector_t *parts;
    lh_usize_t n;
    lh_uindex_t i;

    parts = lh_os_fs_path_get_parts_as_const(name);
    n = lh_vector_get_size(parts);
    for (i = 0U; i < n; ++i)
    {
        lh_os_fs_path_append_part(self, lh_os_fs_path_get_part(name, i));
    }
    lh_os_fs_path_finish_singleton(self);
    return lh_os_fs_path_commit(self);
}

static lh_usize_t
lh_os_fs_path_take_drive(lh_os_fs_path_t *self, const lh_str_view_t *text, lh_usize_t n)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    lh_str_view_t drive;

    if (n < 2U)
    {
        return 0U;
    }
    drive = lh_memory_view_make_from_offset(text, 0U, 2U);
    if (!lh_os_fs_path_is_drive_view(drive))
    {
        return 0U;
    }
    lh_os_fs_path_append_part(self, drive);
    return (2U < n && lh_os_fs_path_is_sep_at(text, 2U)) ? 3U : 2U;
#else
    (void)self;
    (void)text;
    (void)n;
    return 0U;
#endif
}

static lh_usize_t
lh_os_fs_path_take_leading_seps(lh_os_fs_path_t *self, const lh_str_view_t *text, lh_usize_t i,
                               lh_usize_t n)
{
    while (i < n && lh_os_fs_path_is_sep_at(text, i))
    {
        lh_str_view_t empty;

        lh_str_view_init_empty(lh_addr_of(empty));
        lh_os_fs_path_append_part(self, empty);
        i += 1U;
    }
    return i;
}

static void
lh_os_fs_path_take_names(lh_os_fs_path_t *self, const lh_str_view_t *text, lh_usize_t i, lh_usize_t n)
{
    while (i < n)
    {
        lh_usize_t start;

        start = i;
        while (i < n && !lh_os_fs_path_is_sep_at(text, i))
        {
            i += 1U;
        }
        if (i > start)
        {
            lh_os_fs_path_append_part(self, lh_memory_view_make_from_offset(text, start, i - start));
        }
        if (i < n)
        {
            i += 1U;
        }
    }
}

lh_bool_t
lh_os_fs_path_set(lh_os_fs_path_t *self, lh_str_view_t text)
{
    lh_usize_t n;
    lh_usize_t i;

    lh_os_fs_path_clear(self);
    if (lh_str_view_is_empty(lh_addr_of(text)))
    {
        return lh_bool_true;
    }
    n = lh_str_view_get_size(lh_addr_of(text));
    i = lh_os_fs_path_take_drive(self, lh_addr_of(text), n);
    i = lh_os_fs_path_take_leading_seps(self, lh_addr_of(text), i, n);
    lh_os_fs_path_take_names(self, lh_addr_of(text), i, n);
    lh_os_fs_path_finish_singleton(self);
    return lh_os_fs_path_commit(self);
}

static lh_bool_t
lh_os_fs_path_drop_last(lh_os_fs_path_t *self)
{
    lh_vector_t *parts;
    lh_usize_t n;
    lh_os_fs_path_span_t last;
    lh_str_view_t first;
    lh_str_view_t view;
    lh_usize_t keep;

    if (!lh_os_fs_path_require(self))
    {
        return lh_bool_false;
    }
    parts = lh_os_fs_path_get_parts(self);
    n = lh_vector_get_size(parts);
    if (n == 1U)
    {
        first = lh_os_fs_path_get_part(self, 0U);
        if (lh_os_fs_path_is_root_part(first))
        {
            return lh_bool_true;
        }
        return lh_os_fs_path_set(self, lh_str_view_lit("."));
    }

    lh_vector_pop_back(parts, lh_addr_of(last));
    n = lh_vector_get_size(parts);
    keep = last.offset;
    if (keep > 0U)
    {
        view = lh_os_fs_path_as_view(self);
        if (lh_os_fs_path_is_sep_at(lh_addr_of(view), keep - 1U) &&
            (n != 1U || !lh_os_fs_path_is_root_part(lh_os_fs_path_get_part(self, 0U))))
        {
            keep -= 1U;
        }
    }
    lh_str_truncate(lh_os_fs_path_get_text(self), keep);
    if (n == 1U)
    {
        lh_os_fs_path_finish_singleton(self);
    }
    return lh_bool_true;
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

    lh_assert_runtime_ref(dir);
    if (!lh_os_fs_path_require(name))
    {
        lh_os_fs_path_clear(out);
        return lh_bool_false;
    }

    if (out == name)
    {
        lh_os_fs_path_init(lh_addr_of(tmp));
        lh_os_fs_path_assign(lh_addr_of(tmp), dir);
        if (!lh_os_fs_path_append_path(lh_addr_of(tmp), name))
        {
            lh_os_fs_path_deinit(lh_addr_of(tmp));
            lh_os_fs_path_clear(out);
            return lh_bool_false;
        }
        lh_os_fs_path_assign(out, lh_addr_of(tmp));
        lh_os_fs_path_deinit(lh_addr_of(tmp));
        return lh_bool_true;
    }

    if (out != dir)
    {
        lh_os_fs_path_assign(out, dir);
    }
    return lh_os_fs_path_append_path(out, name);
}
