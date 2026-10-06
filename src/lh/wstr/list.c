#include <lh/wstr/list.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/memory/view.h>
#include <lh/null.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/math.h>
#include <lh/util/ptr.h>

/* Where one element sits in the shared buffer; its NUL follows at
   offset + size. */
struct lh_wstr_list_span
{
    lh_usize_t offset;
    lh_usize_t size;
};
typedef struct lh_wstr_list_span lh_wstr_list_span_t;

LH_ATTRIBUTE_STATIC
lh_wstr_t *
lh_wstr_list_get_chars(lh_wstr_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->chars);
}

LH_ATTRIBUTE_STATIC
const lh_wstr_t *
lh_wstr_list_get_chars_as_const(const lh_wstr_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->chars);
}

LH_ATTRIBUTE_STATIC
lh_array_t *
lh_wstr_list_get_spans(lh_wstr_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->spans);
}

LH_ATTRIBUTE_STATIC
const lh_array_t *
lh_wstr_list_get_spans_as_const(const lh_wstr_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->spans);
}

LH_ATTRIBUTE_STATIC
lh_wstr_list_span_t
lh_wstr_list_get_span(const lh_wstr_list_t *self, lh_uindex_t index)
{
    return lh_ptr_deref(
        lh_ptr_cast(const lh_wstr_list_span_t, lh_array_get_ptr(lh_wstr_list_get_spans_as_const(self), index)));
}

void
lh_wstr_list_init(lh_wstr_list_t *self)
{
    lh_wstr_init(lh_wstr_list_get_chars(self));
    lh_array_init(lh_wstr_list_get_spans(self), sizeof(lh_wstr_list_span_t));
}

void
lh_wstr_list_deinit(lh_wstr_list_t *self)
{
    lh_wstr_deinit(lh_wstr_list_get_chars(self));
    lh_array_deinit(lh_wstr_list_get_spans(self));
}

void
lh_wstr_list_clear(lh_wstr_list_t *self)
{
    lh_wstr_clear(lh_wstr_list_get_chars(self));
    lh_array_clear(lh_wstr_list_get_spans(self));
}

lh_bool_t
lh_wstr_list_is_empty(const lh_wstr_list_t *self)
{
    return lh_array_is_empty(lh_wstr_list_get_spans_as_const(self));
}

lh_usize_t
lh_wstr_list_get_size(const lh_wstr_list_t *self)
{
    return lh_array_get_size(lh_wstr_list_get_spans_as_const(self));
}

lh_wstr_view_t
lh_wstr_list_get(const lh_wstr_list_t *self, lh_uindex_t index)
{
    lh_wstr_list_span_t span;
    lh_wstr_view_t chars;

    span = lh_wstr_list_get_span(self, index);
    if (lh_math_is_zero(span.size))
    {
        lh_wstr_view_t _lh_tmp;
    lh_wstr_view_init(lh_addr_of(_lh_tmp), lh_null);
    return _lh_tmp; /* views reject size 0 */
    }
    chars = lh_wstr_as_view(lh_wstr_list_get_chars_as_const(self));
    return lh_wstr_view_from_offset(lh_addr_of(chars), span.offset, span.size);
}

lh_wstr_cptr
lh_wstr_list_get_data(const lh_wstr_list_t *self, lh_uindex_t index)
{
    lh_wstr_list_span_t span;
    lh_wstr_view_t chars;

    span = lh_wstr_list_get_span(self, index);
    chars = lh_wstr_as_view(lh_wstr_list_get_chars_as_const(self));
    return lh_ptr_cast(const lh_wchar_t,
                       lh_memory_view_get_ptr_from_begin(
                           lh_addr_of(chars), lh_math_mul(span.offset, lh_cast_static(lh_usize_t, LH_WCHAR_T_SIZE))));
}

lh_uindex_t
lh_wstr_list_push_back(lh_wstr_list_t *self, lh_wstr_view_t text)
{
    lh_wstr_t *chars;
    lh_wstr_list_span_t span;

    chars = lh_wstr_list_get_chars(self);
    span.offset = lh_wstr_get_size(chars);
    span.size = lh_wstr_view_is_empty(lh_addr_of(text)) ? 0U : lh_wstr_view_get_size(lh_addr_of(text));
    lh_wstr_append_view(chars, text);
    lh_wstr_push_back(chars, lh_cast_static(lh_wchar_t, 0));
    return lh_array_push_back(lh_wstr_list_get_spans(self), lh_addr_of(span));
}

lh_uindex_t
lh_wstr_list_push_back_str(lh_wstr_list_t *self, const lh_wstr_t *value)
{
    return lh_wstr_list_push_back(self, lh_wstr_as_view(value));
}

void
lh_wstr_list_assign(lh_wstr_list_t *self, const lh_wstr_list_t *other)
{
    if (lh_math_eq(self, other))
    {
        return;
    }
    lh_wstr_assign(lh_wstr_list_get_chars(self), lh_wstr_list_get_chars_as_const(other));
    lh_array_assign(lh_wstr_list_get_spans(self), lh_wstr_list_get_spans_as_const(other));
}

void
lh_wstr_list_append(lh_wstr_list_t *self, const lh_wstr_list_t *other)
{
    lh_usize_t base;
    lh_usize_t n;
    lh_uindex_t i;
    lh_wstr_list_span_t span;

    /* Read other's size before growing self: they may be the same list. */
    n = lh_wstr_list_get_size(other);
    base = lh_wstr_get_size(lh_wstr_list_get_chars(self));
    lh_wstr_append_str(lh_wstr_list_get_chars(self), lh_wstr_list_get_chars_as_const(other));
    for (i = 0U; lh_math_lt(i, n); i = lh_math_add_one(i))
    {
        span = lh_wstr_list_get_span(other, i);
        span.offset = lh_math_add(span.offset, base);
        lh_array_push_back(lh_wstr_list_get_spans(self), lh_addr_of(span));
    }
}

void
lh_wstr_list_join(const lh_wstr_list_t *self, lh_wstr_t *out, lh_wchar_t sep)
{
    lh_usize_t n;
    lh_uindex_t i;

    n = lh_wstr_list_get_size(self);
    for (i = 0U; lh_math_lt(i, n); i = lh_math_add_one(i))
    {
        if (lh_math_gt(i, 0U))
        {
            lh_wstr_push_back(out, sep);
        }
        lh_wstr_append_view(out, lh_wstr_list_get(self, i));
    }
}
