/**
 * @file cursor.c
 * @brief Implementation of `lh/core/str/cursor.h`.
 */

#include <lh/core/str/cursor.h>
#include <lh/core/assert.h>
#include <lh/core/char/space.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

const lh_str_view_t *
lh_str_cursor_get_text(const lh_str_cursor_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_void
lh_str_cursor_set_text(lh_str_cursor_t *self, lh_str_view_t text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

lh_usize_t
lh_str_cursor_get_pos(const lh_str_cursor_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pos;
}

lh_void
lh_str_cursor_set_pos(lh_str_cursor_t *self, lh_usize_t pos)
{
    lh_assert_runtime_ref(self);
    self->pos = pos;
}

/* ── init ────────────────────────────────────────────────────────────────── */

lh_void
lh_str_cursor_init(lh_str_cursor_t *self, lh_str_view_t text)
{
    lh_str_cursor_set_text(self, text);
    lh_str_cursor_set_pos(self, 0U);
}

/* ── position ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_str_cursor_is_end(const lh_str_cursor_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_ge(lh_str_cursor_get_pos(self),
                                     lh_str_view_get_size(lh_str_cursor_get_text(self))));
}

lh_usize_t
lh_str_cursor_get_left(const lh_str_cursor_t *self)
{
    return lh_math_sub(lh_str_view_get_size(lh_str_cursor_get_text(self)),
                       lh_str_cursor_get_pos(self));
}

lh_char_t
lh_str_cursor_peek(const lh_str_cursor_t *self, lh_usize_t ahead)
{
    const lh_str_view_t *text = lh_str_cursor_get_text(self);
    const lh_usize_t at = lh_math_add(lh_str_cursor_get_pos(self), ahead);

    if (lh_math_ge(at, lh_str_view_get_size(text)))
    {
        return lh_char_map_nul;
    }
    return lh_str_view_get_char_from_begin(text, at);
}

lh_void
lh_str_cursor_get_rest(const lh_str_cursor_t *self, lh_str_view_t *out)
{
    lh_str_view_tail(lh_str_cursor_get_text(self), lh_str_cursor_get_pos(self), out);
}

lh_void
lh_str_cursor_get_since(const lh_str_cursor_t *self, lh_usize_t start, lh_str_view_t *out)
{
    lh_str_view_from_offset(lh_str_cursor_get_text(self), start,
                            lh_math_sub(lh_str_cursor_get_pos(self), start), out);
}

lh_void
lh_str_cursor_advance(lh_str_cursor_t *self, lh_usize_t count)
{
    lh_str_cursor_set_pos(self, lh_math_add(lh_str_cursor_get_pos(self),
                                            lh_math_min(count, lh_str_cursor_get_left(self))));
}

/* ── matching ────────────────────────────────────────────────────────────── */

lh_char_t
lh_str_cursor_get_char(const lh_str_cursor_t *self)
{
    return lh_str_cursor_peek(self, 0U);
}

lh_bool_t
lh_str_cursor_is_word(const lh_str_cursor_t *self, lh_str_view_t word, lh_bool_t ignore_case)
{
    lh_str_view_t rest;

    lh_str_cursor_get_rest(self, lh_addr_of(rest));
    return lh_str_view_starts_with(lh_addr_of(rest), lh_addr_of(word), ignore_case);
}

lh_usize_t
lh_str_cursor_find(const lh_str_cursor_t *self, lh_str_view_t word, lh_bool_t ignore_case)
{
    lh_str_view_t rest;

    lh_str_cursor_get_rest(self, lh_addr_of(rest));
    return lh_str_view_find(lh_addr_of(rest), lh_addr_of(word), ignore_case);
}

lh_bool_t
lh_str_cursor_skip_char(lh_str_cursor_t *self, lh_char_t ch)
{
    if (lh_str_cursor_is_end(self) || lh_math_ne(lh_str_cursor_get_char(self), ch))
    {
        return lh_bool_false;
    }
    lh_str_cursor_advance(self, 1U);
    return lh_bool_true;
}

lh_bool_t
lh_str_cursor_skip_word(lh_str_cursor_t *self, lh_str_view_t word, lh_bool_t ignore_case)
{
    if (!lh_str_cursor_is_word(self, word, ignore_case))
    {
        return lh_bool_false;
    }
    lh_str_cursor_advance(self, lh_str_view_get_size(lh_addr_of(word)));
    return lh_bool_true;
}

lh_usize_t
lh_str_cursor_skip_whitespace(lh_str_cursor_t *self)
{
    const lh_usize_t start = lh_str_cursor_get_pos(self);

    while (!lh_str_cursor_is_end(self) && lh_char_is_whitespace(lh_str_cursor_get_char(self)))
    {
        lh_str_cursor_advance(self, 1U);
    }
    return lh_math_sub(lh_str_cursor_get_pos(self), start);
}
