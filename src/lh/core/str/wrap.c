/**
 * @file wrap.c
 * @brief Implementation of `lh/core/str/wrap.h`.
 */

#include <lh/core/str/wrap.h>

#include <lh/core/assert.h>
#include <lh/core/char/map.h>
#include <lh/core/str/utf8.h>
#include <lh/core/util/return.h>

const lh_char_t *
lh_str_wrap_get_begin(const lh_str_wrap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->begin;
}

lh_void
lh_str_wrap_set_begin(lh_str_wrap_t *self, const lh_char_t *begin)
{
    lh_assert_runtime_ref(self);
    self->begin = begin;
}

const lh_char_t *
lh_str_wrap_get_end(const lh_str_wrap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->end;
}

lh_void
lh_str_wrap_set_end(lh_str_wrap_t *self, const lh_char_t *end)
{
    lh_assert_runtime_ref(self);
    self->end = end;
}

const lh_char_t *
lh_str_wrap_get_next(const lh_str_wrap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_void
lh_str_wrap_set_next(lh_str_wrap_t *self, const lh_char_t *next)
{
    lh_assert_runtime_ref(self);
    self->next = next;
}

lh_s32_t
lh_str_wrap_get_width(const lh_str_wrap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_str_wrap_set_width(lh_str_wrap_t *self, lh_s32_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_bool_t
lh_str_wrap_is_line_end(lh_u32_t code)
{
    return code == 0U || code == lh_cast_static(lh_u32_t, lh_char_map_lf);
}

lh_void
lh_str_wrap_init(lh_str_wrap_t *self)
{
    lh_str_wrap_set_begin(self, lh_null);
    lh_str_wrap_set_end(self, lh_null);
    lh_str_wrap_set_next(self, lh_null);
    lh_str_wrap_set_width(self, 0);
}

const lh_char_t *
lh_str_wrap_get_next_of(const lh_char_t *end, lh_s32_t width)
{
    const lh_char_t *cursor = end;
    const lh_u32_t code = lh_str_utf8_get_next_code(lh_addr_of(cursor));

    lh_return_if(code == 0U, lh_null);
    lh_return_if(lh_str_wrap_is_line_end(code), cursor);
    /* Broken at a space: the spaces are where the break is, not the next line's. */
    if (width > 0 && code == lh_cast_static(lh_u32_t, lh_char_map_space))
    {
        while (lh_ptr_deref(cursor) == lh_char_map_space)
        {
            ++cursor;
        }
        lh_return_if(lh_ptr_deref(cursor) == lh_char_map_nul, lh_null);
        return cursor;
    }
    return end;
}

lh_bool_t
lh_str_wrap_measure(lh_str_wrap_t *self, const lh_char_t *text, lh_s32_t width,
                    lh_str_wrap_advance_fn *advance, lh_ptr context)
{
    lh_str_wrap_init(self);
    lh_return_if(lh_null_eq(text), lh_bool_false);
    lh_str_wrap_set_begin(self, text);
    /* The line as it stood at the last space: where it breaks when a word runs over.
       No space yet: a null end. */
    const lh_char_t *space_end = lh_null;
    lh_s32_t space_width = 0;
    const lh_char_t *cursor = text;
    const lh_char_t *end = text;
    lh_s32_t taken = 0;

    for (;;)
    {
        const lh_char_t *before = cursor;
        const lh_u32_t code = lh_str_utf8_get_next_code(lh_addr_of(cursor));

        if (lh_str_wrap_is_line_end(code))
        {
            end = before;
            break;
        }
        const lh_s32_t step = advance(context, code);
        const lh_bool_t space = code == lh_cast_static(lh_u32_t, lh_char_map_space);

        /* A space may hang past the edge: it is where the line breaks, not ink. */
        if (width > 0 && !space && taken + step > width)
        {
            if (!lh_null_eq(space_end))
            {
                end = space_end;
                taken = space_width;
                break;
            }
            if (before == text)
            {
                /* A first letter wider than the line still takes it. */
                taken = taken + step;
                end = cursor;
                break;
            }
            end = before;
            break;
        }
        if (space)
        {
            space_end = before;
            space_width = taken;
        }
        taken = taken + step;
    }
    lh_str_wrap_set_end(self, end);
    lh_str_wrap_set_width(self, taken);
    lh_str_wrap_set_next(self, lh_str_wrap_get_next_of(end, width));
    return lh_bool_true;
}
