#include <lh/str/list.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/char/map.h>
#include <lh/memory/view.h>
#include <lh/null.h>
#include <lh/runtime/error.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

/* Where one element sits in the shared buffer; its NUL follows at
   offset + size. */
typedef struct lh_str_list_span
{
    lh_usize_t offset;
    lh_usize_t size;
} lh_str_list_span_t;

LH_ATTRIBUTE_STATIC
lh_str_t *
lh_str_list_get_chars(lh_str_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->chars);
}

LH_ATTRIBUTE_STATIC
const lh_str_t *
lh_str_list_get_chars_as_const(const lh_str_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->chars);
}

LH_ATTRIBUTE_STATIC
lh_vector_t *
lh_str_list_get_spans(lh_str_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->spans);
}

LH_ATTRIBUTE_STATIC
const lh_vector_t *
lh_str_list_get_spans_as_const(const lh_str_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->spans);
}

LH_ATTRIBUTE_STATIC
lh_str_list_span_t
lh_str_list_get_span(const lh_str_list_t *self, lh_uindex_t index)
{
    return lh_ptr_deref(
        lh_ptr_cast(const lh_str_list_span_t, lh_vector_get_ptr(lh_str_list_get_spans_as_const(self), index)));
}

void
lh_str_list_init(lh_str_list_t *self)
{
    lh_str_init(lh_str_list_get_chars(self));
    lh_vector_init(lh_str_list_get_spans(self), sizeof(lh_str_list_span_t));
}

void
lh_str_list_deinit(lh_str_list_t *self)
{
    lh_str_deinit(lh_str_list_get_chars(self));
    lh_vector_deinit(lh_str_list_get_spans(self));
}

void
lh_str_list_clear(lh_str_list_t *self)
{
    lh_str_clear(lh_str_list_get_chars(self));
    lh_vector_clear(lh_str_list_get_spans(self));
}

lh_bool_t
lh_str_list_is_empty(const lh_str_list_t *self)
{
    return lh_vector_is_empty(lh_str_list_get_spans_as_const(self));
}

lh_usize_t
lh_str_list_get_size(const lh_str_list_t *self)
{
    return lh_vector_get_size(lh_str_list_get_spans_as_const(self));
}

lh_str_view_t
lh_str_list_get(const lh_str_list_t *self, lh_uindex_t index)
{
    lh_str_list_span_t span;
    lh_str_view_t chars;

    span = lh_str_list_get_span(self, index);
    if (lh_math_is_zero(span.size))
    {
        return lh_str_view_make(lh_null); /* views reject size 0 */
    }
    chars = lh_str_as_view(lh_str_list_get_chars_as_const(self));
    return lh_str_view_make_from_offset(lh_addr_of(chars), span.offset, span.size);
}

lh_str_cptr
lh_str_list_get_data(const lh_str_list_t *self, lh_uindex_t index)
{
    lh_str_list_span_t span;
    lh_str_view_t chars;

    span = lh_str_list_get_span(self, index);
    chars = lh_str_as_view(lh_str_list_get_chars_as_const(self));
    return lh_ptr_cast(const lh_char_t,
                       lh_memory_view_get_ptr_from_begin(lh_addr_of(chars), span.offset));
}

lh_uindex_t
lh_str_list_push_back(lh_str_list_t *self, lh_str_view_t text)
{
    lh_str_t *chars;
    lh_str_list_span_t span;

    chars = lh_str_list_get_chars(self);
    span.offset = lh_str_get_size(chars);
    span.size = lh_str_view_is_empty(lh_addr_of(text)) ? 0U : lh_str_view_get_size(lh_addr_of(text));
    lh_str_append_view(chars, text);
    lh_str_push_back(chars, lh_char_map_nul);
    return lh_vector_push_back(lh_str_list_get_spans(self), lh_addr_of(span));
}

lh_uindex_t
lh_str_list_push_back_str(lh_str_list_t *self, const lh_str_t *value)
{
    return lh_str_list_push_back(self, lh_str_as_view(value));
}

void
lh_str_list_assign(lh_str_list_t *self, const lh_str_list_t *other)
{
    if (lh_math_eq(self, other))
    {
        return;
    }
    lh_str_assign(lh_str_list_get_chars(self), lh_str_list_get_chars_as_const(other));
    lh_vector_assign(lh_str_list_get_spans(self), lh_str_list_get_spans_as_const(other));
}

void
lh_str_list_append(lh_str_list_t *self, const lh_str_list_t *other)
{
    lh_usize_t base;
    lh_usize_t n;
    lh_uindex_t i;
    lh_str_list_span_t span;

    /* Read other's size before growing self: they may be the same list. */
    n = lh_str_list_get_size(other);
    base = lh_str_get_size(lh_str_list_get_chars(self));
    lh_str_append_str(lh_str_list_get_chars(self), lh_str_list_get_chars_as_const(other));
    for (i = 0U; lh_math_lt(i, n); i = lh_math_add_one(i))
    {
        span = lh_str_list_get_span(other, i);
        span.offset = lh_math_add(span.offset, base);
        lh_vector_push_back(lh_str_list_get_spans(self), lh_addr_of(span));
    }
}

void
lh_str_list_join(const lh_str_list_t *self, lh_str_t *out, lh_char_t sep)
{
    lh_usize_t n;
    lh_uindex_t i;

    n = lh_str_list_get_size(self);
    for (i = 0U; lh_math_lt(i, n); i = lh_math_add_one(i))
    {
        if (lh_math_gt(i, 0U))
        {
            lh_str_push_back(out, sep);
        }
        lh_str_append_view(out, lh_str_list_get(self, i));
    }
}

void
lh_str_list_split_of(lh_str_list_t *self, lh_str_view_t text, lh_str_cptr delims, lh_usize_t delim_count)
{
    lh_usize_t pos = 0U;
    lh_str_view_t piece;
    lh_bool_t had_delim;

    while (lh_str_view_split_next_of(lh_addr_of(text), delims, delim_count, lh_addr_of(pos),
                                     lh_addr_of(piece), lh_addr_of(had_delim)))
    {
        if (!lh_str_view_is_empty(lh_addr_of(piece)))
        {
            lh_str_list_push_back(self, piece);
        }
    }
}
