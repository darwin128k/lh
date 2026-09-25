#include <lh/fs/path.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/char/dot.h>
#include <lh/char/letter.h>
#include <lh/char/map.h>
#include <lh/char/slash.h>
#include <lh/memory/view.h>
#include <lh/null.h>
#include <lh/runtime/error.h>
#include <lh/str.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>

/*
 * Layout: `text` is the whole path, normalized, with `/` as the only
 * separator — root prefix first (`/` or `C:/`), then segments joined by a
 * single `/`, never a trailing one. A segment is a slice of `text`, not a
 * string of its own: parsing is one buffer and one pass, reading a segment
 * allocates nothing.
 */

static const lh_char_t m_fs_path_posix_seps[] = {lh_char_map_slash};
static const lh_char_t m_fs_path_windows_seps[] = {lh_char_map_slash, lh_char_map_backslash};

LH_ATTRIBUTE_STATIC
lh_usize_t
lh_fs_path_root_len(lh_fs_path_root_kind_t kind)
{
    if (lh_math_eq(kind, lh_fs_path_root_kind_drive))
    {
        return 3U; /* "C:/" */
    }
    return lh_math_eq(kind, lh_fs_path_root_kind_posix) ? 1U : 0U;
}

LH_ATTRIBUTE_STATIC
const lh_str_t *
lh_fs_path_get_text(const lh_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

LH_ATTRIBUTE_STATIC
lh_str_t *
lh_fs_path_get_text_mut(lh_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

LH_ATTRIBUTE_STATIC
void
lh_fs_path_set_root_kind(lh_fs_path_t *self, lh_fs_path_root_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->root_kind = kind;
}

/* `view` from `offset` to its end; the empty view when nothing is left
   (views reject size 0). */
LH_ATTRIBUTE_STATIC
lh_str_view_t
lh_fs_path_view_tail(const lh_str_view_t *view, lh_usize_t offset)
{
    lh_usize_t size;

    size = lh_str_view_is_empty(view) ? 0U : lh_str_view_get_size(view);
    if (!lh_math_lt(offset, size))
    {
        return lh_str_view_make(lh_null);
    }
    return lh_memory_view_make_from_offset(view, offset, lh_math_sub(size, offset));
}

/* The segments part of `text`, root prefix dropped. */
LH_ATTRIBUTE_STATIC
lh_str_view_t
lh_fs_path_get_rest(const lh_fs_path_t *self)
{
    lh_str_view_t text;

    text = lh_str_as_view(lh_fs_path_get_text(self));
    return lh_fs_path_view_tail(lh_addr_of(text), lh_fs_path_root_len(lh_fs_path_get_root_kind(self)));
}

void
lh_fs_path_init(lh_fs_path_t *self)
{
    lh_fs_path_set_root_kind(self, lh_fs_path_root_kind_none);
    lh_str_init(lh_fs_path_get_text_mut(self));
}

void
lh_fs_path_deinit(lh_fs_path_t *self)
{
    lh_str_deinit(lh_fs_path_get_text_mut(self));
}

void
lh_fs_path_clear(lh_fs_path_t *self)
{
    lh_fs_path_set_root_kind(self, lh_fs_path_root_kind_none);
    lh_str_clear(lh_fs_path_get_text_mut(self));
}

void
lh_fs_path_assign(lh_fs_path_t *self, const lh_fs_path_t *other)
{
    if (lh_math_eq(self, other))
    {
        return;
    }
    lh_fs_path_set_root_kind(self, lh_fs_path_get_root_kind(other));
    lh_str_assign(lh_fs_path_get_text_mut(self), lh_fs_path_get_text(other));
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
    lh_str_view_t text;

    if (lh_math_ne(lh_fs_path_get_root_kind(self), lh_fs_path_root_kind_drive))
    {
        return 0;
    }
    text = lh_str_as_view(lh_fs_path_get_text(self));
    return lh_str_view_get_first_char(lh_addr_of(text));
}

lh_usize_t
lh_fs_path_get_segment_count(const lh_fs_path_t *self)
{
    lh_str_view_t rest;
    lh_str_view_t field;
    lh_usize_t pos;
    lh_usize_t count;
    lh_bool_t had_delim;

    rest = lh_fs_path_get_rest(self);
    pos = 0U;
    count = 0U;
    while (lh_str_view_split_next_of(lh_addr_of(rest), m_fs_path_posix_seps, 1U, lh_addr_of(pos),
                                     lh_addr_of(field), lh_addr_of(had_delim)))
    {
        count = lh_math_add_one(count);
    }
    return count;
}

lh_str_view_t
lh_fs_path_get_segment(const lh_fs_path_t *self, lh_uindex_t index)
{
    lh_str_view_t rest;
    lh_str_view_t field;
    lh_usize_t pos;
    lh_uindex_t seen;
    lh_bool_t had_delim;

    rest = lh_fs_path_get_rest(self);
    pos = 0U;
    seen = 0U;
    while (lh_str_view_split_next_of(lh_addr_of(rest), m_fs_path_posix_seps, 1U, lh_addr_of(pos),
                                     lh_addr_of(field), lh_addr_of(had_delim)))
    {
        if (lh_math_eq(seen, index))
        {
            return field;
        }
        seen = lh_math_add_one(seen);
    }
    lh_assert_runtime_if(lh_bool_true, lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));
    return lh_str_view_make(lh_null);
}

lh_bool_t
lh_fs_path_is_empty(const lh_fs_path_t *self)
{
    return lh_str_is_empty(lh_fs_path_get_text(self));
}

lh_bool_t
lh_fs_path_is_absolute(const lh_fs_path_t *self)
{
    return lh_cast_static(lh_bool_t, lh_math_ne(lh_fs_path_get_root_kind(self), lh_fs_path_root_kind_none));
}

lh_bool_t
lh_fs_path_is_root(const lh_fs_path_t *self)
{
    lh_str_view_t rest;

    rest = lh_fs_path_get_rest(self);
    return lh_cast_static(lh_bool_t, lh_fs_path_is_absolute(self) && lh_str_view_is_empty(lh_addr_of(rest)));
}

lh_bool_t
lh_fs_path_is_hidden(const lh_fs_path_t *self)
{
    lh_str_view_t rest;
    lh_str_view_t last;
    lh_usize_t slash;
    lh_usize_t size;

    rest = lh_fs_path_get_rest(self);
    if (lh_str_view_is_empty(lh_addr_of(rest)))
    {
        return lh_bool_false;
    }
    slash = lh_str_view_rfind_char(lh_addr_of(rest), lh_char_map_slash);
    last = lh_math_eq(slash, LH_STR_VIEW_INVALID) ? rest
                                                   : lh_fs_path_view_tail(lh_addr_of(rest), lh_math_add_one(slash));
    if (!lh_char_is_dot(lh_str_view_get_first_char(lh_addr_of(last))))
    {
        return lh_bool_false;
    }
    size = lh_str_view_get_size(lh_addr_of(last));
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

/* Recognize the root at the start of `text` (non-empty); write its
   canonical prefix into `self` and return how many input characters it
   consumed. */
LH_ATTRIBUTE_STATIC
lh_usize_t
lh_fs_path_take_root(lh_fs_path_t *self, const lh_str_view_t *text, lh_fs_path_style_t style)
{
    lh_str_t *out;

    out = lh_fs_path_get_text_mut(self);
    if (lh_math_eq(style, lh_fs_path_style_windows) && lh_math_ge(lh_str_view_get_size(text), 2U) &&
        lh_fs_path_is_drive(lh_memory_view_make_from_offset(text, 0U, 2U)))
    {
        lh_fs_path_set_root_kind(self, lh_fs_path_root_kind_drive);
        lh_str_push_back(out, lh_str_view_get_first_char(text));
        lh_str_push_back(out, lh_char_map_colon);
        lh_str_push_back(out, lh_char_map_slash);
        return 2U; /* a separator after "C:" is skipped with the others */
    }
    if (lh_fs_path_is_sep(lh_str_view_get_first_char(text), style))
    {
        lh_fs_path_set_root_kind(self, lh_fs_path_root_kind_posix);
        lh_str_push_back(out, lh_char_map_slash);
        return 1U;
    }
    return 0U;
}

void
lh_fs_path_set(lh_fs_path_t *self, lh_str_view_t text, lh_fs_path_style_t style)
{
    lh_str_t *out;
    lh_str_view_t rest;
    lh_str_view_t field;
    lh_usize_t root_len;
    lh_usize_t pos;
    lh_bool_t had_delim;

    lh_assert_runtime_if(lh_math_ne(style, lh_fs_path_style_posix) && lh_math_ne(style, lh_fs_path_style_windows),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    lh_fs_path_clear(self);
    if (lh_str_view_is_empty(lh_addr_of(text)))
    {
        return;
    }
    out = lh_fs_path_get_text_mut(self);
    /* Normalizing never grows the text by more than the "C:" -> "C:/" slash. */
    lh_str_reserve(out, lh_math_add_one(lh_str_view_get_size(lh_addr_of(text))));
    rest = lh_fs_path_view_tail(lh_addr_of(text), lh_fs_path_take_root(self, lh_addr_of(text), style));
    root_len = lh_str_get_size(out);

    pos = 0U;
    while (lh_math_eq(style, lh_fs_path_style_windows)
               ? lh_str_view_split_next_of(lh_addr_of(rest), m_fs_path_windows_seps, 2U, lh_addr_of(pos),
                                           lh_addr_of(field), lh_addr_of(had_delim))
               : lh_str_view_split_next_of(lh_addr_of(rest), m_fs_path_posix_seps, 1U, lh_addr_of(pos),
                                           lh_addr_of(field), lh_addr_of(had_delim)))
    {
        if (lh_str_view_is_empty(lh_addr_of(field)))
        {
            continue; /* repeated separator */
        }
        if (lh_math_gt(lh_str_get_size(out), root_len))
        {
            lh_str_push_back(out, lh_char_map_slash);
        }
        lh_str_append_view(out, field);
    }
}

void
lh_fs_path_to_str(const lh_fs_path_t *self, lh_fs_path_style_t style, lh_str_t *out)
{
    lh_str_view_t text;
    lh_str_view_t field;
    lh_usize_t pos;
    lh_bool_t had_delim;

    lh_str_clear(out);
    if (lh_math_eq(style, lh_fs_path_style_posix))
    {
        lh_str_append_str(out, lh_fs_path_get_text(self));
        return;
    }

    /* Only `/` is ever a separator in `text`; copy the runs between them. */
    lh_str_reserve(out, lh_str_get_size(lh_fs_path_get_text(self)));
    text = lh_str_as_view(lh_fs_path_get_text(self));
    pos = 0U;
    while (lh_str_view_split_next_of(lh_addr_of(text), m_fs_path_posix_seps, 1U, lh_addr_of(pos), lh_addr_of(field),
                                     lh_addr_of(had_delim)))
    {
        lh_str_append_view(out, field);
        if (had_delim)
        {
            lh_str_push_back(out, lh_fs_path_sep(style));
        }
    }
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
    lh_str_view_t rest;
    lh_str_t copy;
    lh_bool_t aliased;

    rest = lh_fs_path_get_rest(name);
    if (lh_str_view_is_empty(lh_addr_of(rest)))
    {
        lh_fs_path_clear(self);
        return lh_bool_false;
    }

    /* self == name: name's text is about to be overwritten by dir's. */
    aliased = lh_cast_static(lh_bool_t, lh_math_eq(self, name));
    if (aliased)
    {
        lh_str_init(lh_addr_of(copy));
        lh_str_append_view(lh_addr_of(copy), rest);
        rest = lh_str_as_view(lh_addr_of(copy));
    }
    lh_fs_path_assign(self, dir);
    if (!lh_fs_path_is_root(self) && !lh_fs_path_is_empty(self))
    {
        lh_str_push_back(lh_fs_path_get_text_mut(self), lh_char_map_slash);
    }
    lh_str_append_view(lh_fs_path_get_text_mut(self), rest);
    if (aliased)
    {
        lh_str_deinit(lh_addr_of(copy));
    }
    return lh_bool_true;
}
