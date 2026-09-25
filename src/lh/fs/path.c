#include <lh/fs/path.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/char/dot.h>
#include <lh/char/letter.h>
#include <lh/char/map.h>
#include <lh/char/slash.h>
#include <lh/memory/view.h>
#include <lh/runtime/error.h>
#include <lh/str.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>

void
lh_fs_path_init(lh_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    self->root_kind = lh_fs_path_root_kind_none;
    self->root_drive = 0;
    lh_str_list_init(lh_addr_of(self->segments));
}

void
lh_fs_path_deinit(lh_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    lh_str_list_deinit(lh_addr_of(self->segments));
}

void
lh_fs_path_clear(lh_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    self->root_kind = lh_fs_path_root_kind_none;
    self->root_drive = 0;
    lh_str_list_clear(lh_fs_path_get_segments(self));
}

void
lh_fs_path_assign(lh_fs_path_t *self, const lh_fs_path_t *other)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);
    if (lh_math_eq(self, other))
    {
        return;
    }
    self->root_kind = lh_fs_path_get_root_kind(other);
    self->root_drive = lh_fs_path_get_root_drive(other);
    lh_str_list_assign(lh_fs_path_get_segments(self), lh_fs_path_get_segments_as_const(other));
}

lh_fs_path_root_kind_t
lh_fs_path_get_root_kind(const lh_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root_kind;
}

lh_char_t
lh_fs_path_get_root_drive(const lh_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root_drive;
}

lh_str_list_t *
lh_fs_path_get_segments(lh_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->segments);
}

const lh_str_list_t *
lh_fs_path_get_segments_as_const(const lh_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->segments);
}

lh_usize_t
lh_fs_path_get_segment_count(const lh_fs_path_t *self)
{
    return lh_str_list_get_size(lh_fs_path_get_segments_as_const(self));
}

lh_bool_t
lh_fs_path_is_empty(const lh_fs_path_t *self)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_fs_path_get_root_kind(self), lh_fs_path_root_kind_none) &&
                                     lh_str_list_is_empty(lh_fs_path_get_segments_as_const(self)));
}

lh_bool_t
lh_fs_path_is_absolute(const lh_fs_path_t *self)
{
    return lh_cast_static(lh_bool_t, lh_math_ne(lh_fs_path_get_root_kind(self), lh_fs_path_root_kind_none));
}

lh_bool_t
lh_fs_path_is_root(const lh_fs_path_t *self)
{
    return lh_cast_static(lh_bool_t, lh_fs_path_is_absolute(self) &&
                                     lh_math_eq(lh_fs_path_get_segment_count(self), 0U));
}

lh_bool_t
lh_fs_path_is_hidden(const lh_fs_path_t *self)
{
    const lh_str_list_t *segments;
    lh_str_view_t last;
    lh_usize_t n;
    lh_usize_t size;

    segments = lh_fs_path_get_segments_as_const(self);
    n = lh_str_list_get_size(segments);
    if (lh_math_eq(n, 0U))
    {
        return lh_bool_false;
    }
    last = lh_str_as_view(lh_str_list_get_as_const(segments, n - 1U));
    if (lh_str_view_is_empty(lh_addr_of(last)))
    {
        return lh_bool_false;
    }
    size = lh_str_view_get_size(lh_addr_of(last));
    if (!lh_char_is_dot(lh_str_view_get_char_from_begin(lh_addr_of(last), 0U)))
    {
        return lh_bool_false;
    }
    if (lh_math_eq(size, 1U))
    {
        return lh_bool_false;
    }
    if (lh_math_eq(size, 2U) && lh_char_is_dot(lh_str_view_get_char_from_begin(lh_addr_of(last), 1U)))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

LH_ATTRIBUTE_STATIC
lh_bool_t
lh_fs_path_is_sep(lh_char_t ch, lh_fs_path_style_t style)
{
    if (lh_char_is_slash(ch))
    {
        return lh_bool_true;
    }
    return lh_cast_static(lh_bool_t, lh_math_eq(style, lh_fs_path_style_windows) && lh_char_is_backslash(ch));
}

LH_ATTRIBUTE_STATIC
lh_char_t
lh_fs_path_sep(lh_fs_path_style_t style)
{
    return lh_math_eq(style, lh_fs_path_style_windows) ? lh_char_map_backslash : lh_char_map_slash;
}

lh_bool_t
lh_fs_path_is_drive(lh_str_view_t part)
{
    if (lh_math_ne(lh_str_view_get_size(lh_addr_of(part)), 2U))
    {
        return lh_bool_false;
    }
    return lh_cast_static(lh_bool_t,
                          lh_char_is_letter(lh_str_view_get_char_from_begin(lh_addr_of(part), 0U)) &&
                              lh_math_eq(lh_str_view_get_char_from_begin(lh_addr_of(part), 1U),
                                        lh_char_map_colon));
}

LH_ATTRIBUTE_STATIC
lh_usize_t
lh_fs_path_take_drive(lh_fs_path_t *self, const lh_str_view_t *text, lh_usize_t n)
{
    lh_str_view_t drive;

    if (lh_math_lt(n, 2U))
    {
        return 0U;
    }
    drive = lh_memory_view_make_from_offset(text, 0U, 2U);
    if (!lh_fs_path_is_drive(drive))
    {
        return 0U;
    }
    self->root_kind = lh_fs_path_root_kind_drive;
    self->root_drive = lh_str_view_get_char_from_begin(lh_addr_of(drive), 0U);
    return (lh_math_lt(2U, n) &&
            lh_fs_path_is_sep(lh_str_view_get_char_from_begin(text, 2U), lh_fs_path_style_windows))
               ? 3U
               : 2U;
}

void
lh_fs_path_set(lh_fs_path_t *self, lh_str_view_t text, lh_fs_path_style_t style)
{
    static const lh_char_t posix_seps[] = {lh_char_map_slash};
    static const lh_char_t windows_seps[] = {lh_char_map_slash, lh_char_map_backslash};
    lh_usize_t n;
    lh_usize_t pos;

    lh_assert_runtime_if(lh_math_ne(style, lh_fs_path_style_posix) && lh_math_ne(style, lh_fs_path_style_windows),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    lh_fs_path_clear(self);
    if (lh_str_view_is_empty(lh_addr_of(text)))
    {
        return;
    }
    n = lh_str_view_get_size(lh_addr_of(text));
    pos = lh_math_eq(style, lh_fs_path_style_windows) ? lh_fs_path_take_drive(self, lh_addr_of(text), n) : 0U;
    if (lh_math_eq(pos, 0U) && lh_fs_path_is_sep(lh_str_view_get_char_from_begin(lh_addr_of(text), 0U), style))
    {
        self->root_kind = lh_fs_path_root_kind_posix;
        pos = 1U;
    }
    if (lh_math_eq(style, lh_fs_path_style_windows))
    {
        lh_str_list_split_of(lh_fs_path_get_segments(self), lh_memory_view_drop_first(lh_addr_of(text), pos),
                             windows_seps, sizeof(windows_seps) / sizeof(windows_seps[0]));
    }
    else
    {
        lh_str_list_split_of(lh_fs_path_get_segments(self), lh_memory_view_drop_first(lh_addr_of(text), pos),
                             posix_seps, sizeof(posix_seps) / sizeof(posix_seps[0]));
    }
}

void
lh_fs_path_to_str(const lh_fs_path_t *self, lh_fs_path_style_t style, lh_str_t *out)
{
    lh_char_t sep;

    lh_assert_runtime_ref(self);
    lh_str_clear(out);
    sep = lh_fs_path_sep(style);

    if (lh_math_eq(lh_fs_path_get_root_kind(self), lh_fs_path_root_kind_drive))
    {
        lh_str_push_back(out, lh_fs_path_get_root_drive(self));
        lh_str_push_back(out, lh_char_map_colon);
        lh_str_push_back(out, sep);
    }
    else if (lh_math_eq(lh_fs_path_get_root_kind(self), lh_fs_path_root_kind_posix))
    {
        lh_str_push_back(out, sep);
    }

    lh_str_list_join(lh_fs_path_get_segments_as_const(self), out, sep);
}

lh_str_cptr
lh_fs_path_to_cstr(const lh_fs_path_t *self, lh_fs_path_style_t style, lh_str_t *scratch)
{
    lh_str_init(scratch);
    lh_fs_path_to_str(self, style, scratch);
    return lh_str_get_data(scratch);
}

lh_bool_t
lh_fs_path_join(lh_fs_path_t *self, const lh_fs_path_t *dir, const lh_fs_path_t *name)
{
    lh_assert_runtime_ref(dir);
    if (lh_math_eq(lh_fs_path_get_segment_count(name), 0U))
    {
        lh_fs_path_clear(self);
        return lh_bool_false;
    }

    if (lh_math_eq(self, name))
    {
        lh_fs_path_t tmp;

        lh_fs_path_init(lh_addr_of(tmp));
        lh_fs_path_assign(lh_addr_of(tmp), name);
        if (lh_math_ne(self, dir))
        {
            lh_fs_path_assign(self, dir);
        }
        lh_str_list_append(lh_fs_path_get_segments(self), lh_fs_path_get_segments_as_const(lh_addr_of(tmp)));
        lh_fs_path_deinit(lh_addr_of(tmp));
        return lh_bool_true;
    }

    if (lh_math_ne(self, dir))
    {
        lh_fs_path_assign(self, dir);
    }
    lh_str_list_append(lh_fs_path_get_segments(self), lh_fs_path_get_segments_as_const(name));
    return lh_bool_true;
}
