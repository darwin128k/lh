#include <lh/str.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/null.h>
#include <lh/str/format/text.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

/* What an empty string with no buffer of its own reads as. */
static const lh_char_t m_str_empty[1] = {'\0'};

/* Write the NUL after the last character. Only for a string that owns a
   buffer: every buffer is sized for size + 1, so the slot exists. */
LH_ATTRIBUTE_STATIC
lh_void
lh_str_write_terminator(lh_str_t *self)
{
    lh_ptr_deref_of(lh_char_t, lh_vector_get_end(self)) = '\0';
}

/* Same, for callers that may run on a string that never grew (no buffer,
   nothing to write). */
LH_ATTRIBUTE_STATIC
lh_void
lh_str_terminate(lh_str_t *self)
{
    if (lh_math_is_zero(lh_vector_get_capacity(self)))
    {
        return;
    }
    lh_str_write_terminator(self);
}

lh_void
lh_str_init(lh_str_t *self)
{
    lh_vector_init(self, sizeof(lh_char_t));
}

lh_void
lh_str_deinit(lh_str_t *self)
{
    lh_vector_deinit(self);
}

lh_str_cptr
lh_str_get_data(const lh_str_t *self)
{
    if (lh_math_is_zero(lh_vector_get_capacity(self)))
    {
        return m_str_empty;
    }
    return lh_ptr_cast(lh_char_t, lh_vector_get_data(self));
}

lh_usize_t
lh_str_get_size(const lh_str_t *self)
{
    return lh_vector_get_size(self);
}

lh_bool_t
lh_str_is_empty(const lh_str_t *self)
{
    return lh_vector_is_empty(self);
}

lh_void
lh_str_append(lh_str_t *self, lh_str_cptr text, lh_usize_t count)
{
    lh_usize_t needed;
    lh_usize_t capacity;

    if (lh_math_is_zero(count))
    {
        return;
    }
    /* Grow once, for the text and the terminator together — push_back_of
       alone would size for the text and then need a second grow for the NUL. */
    needed = lh_math_add_one(lh_math_add(lh_vector_get_size(self), count));
    capacity = lh_vector_get_capacity(self);
    if (lh_math_lt(capacity, needed))
    {
        lh_vector_reserve(self, lh_vector_get_grown_capacity(capacity, needed));
    }
    lh_vector_push_back_of(self, text, count);
    lh_str_write_terminator(self);
}

lh_void
lh_str_push_back(lh_str_t *self, lh_char_t ch)
{
    lh_str_append(self, lh_addr_of(ch), 1);
}

lh_void
lh_str_append_view(lh_str_t *self, lh_str_view_t view)
{
    if (lh_str_view_is_empty(lh_addr_of(view)))
    {
        return;
    }
    lh_str_append(self, lh_str_view_get_data(lh_addr_of(view)),
                  lh_str_view_get_size(lh_addr_of(view)));
}

lh_void
lh_str_append_str(lh_str_t *self, const lh_str_t *other)
{
    lh_assert_runtime_ref(other);
    if (self == other)
    {
        const lh_usize_t n = lh_str_get_size(self);
        if (n == 0U)
        {
            return;
        }
        lh_vector_reserve(self, lh_vector_get_grown_capacity(
                                    lh_vector_get_capacity(self), lh_math_add(n + n, 1U)));
        lh_str_append(self, lh_str_get_data(self), n);
        return;
    }
    lh_str_append_view(self, lh_str_as_view(other));
}

lh_void
lh_str_assign(lh_str_t *self, const lh_str_t *other)
{
    lh_assert_runtime_ref(other);
    if (self == other)
    {
        return;
    }
    lh_str_clear(self);
    lh_str_append_str(self, other);
}

lh_void
lh_str_assign_view(lh_str_t *self, lh_str_view_t view)
{
    lh_str_clear(self);
    lh_str_append_view(self, view);
}

lh_void
lh_str_join(lh_str_t *self, const lh_str_view_t *parts, lh_usize_t count, lh_char_t sep)
{
    lh_usize_t i;

    if (count > 0U)
    {
        lh_assert_runtime_ref(parts);
    }
    lh_str_clear(self);
    for (i = 0U; i < count; ++i)
    {
        if (i > 0U)
        {
            lh_str_push_back(self, sep);
        }
        lh_str_append_view(self, parts[i]);
    }
}

lh_usize_t
lh_str_format_v(lh_str_t *self, lh_str_cptr fmt, va_list args)
{
    va_list measure;
    lh_usize_t n;

    lh_assert_runtime_ref(self);
    va_copy(measure, args);
    n = lh_str_ptr_format_text_size_v(fmt, measure);
    va_end(measure);
    if (n == LH_STR_PTR_INVALID)
    {
        return 0;
    }

    lh_vector_reserve(self, lh_math_add_one(n));
    lh_vector_resize(self, n);
    if (n > 0U)
    {
        lh_str_ptr_format_text_v(lh_ptr_cast(lh_char_t, lh_vector_get_data(self)), n, fmt, args);
    }
    lh_str_terminate(self);
    return n;
}

lh_usize_t
lh_str_format(lh_str_t *self, lh_str_cptr fmt, ...)
{
    lh_usize_t result;
    va_list args;

    va_start(args, fmt);
    result = lh_str_format_v(self, fmt, args);
    va_end(args);
    return result;
}

lh_void
lh_str_reserve(lh_str_t *self, lh_usize_t count)
{
    lh_vector_reserve(self, lh_math_add_one(count));
}

lh_void
lh_str_clear(lh_str_t *self)
{
    lh_vector_clear(self);
    lh_str_terminate(self);
}

lh_void
lh_str_truncate(lh_str_t *self, lh_usize_t n)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(n > lh_str_get_size(self), lh_runtime_error_code_invalid_range);
    lh_vector_resize(self, n);
    lh_str_terminate(self);
}

lh_str_view_t
lh_str_as_view(const lh_str_t *self)
{
    lh_str_view_t view;

    /* A sized view rejects size 0; an empty string is the empty view. */
    if (lh_str_is_empty(self))
    {
        lh_str_view_init_empty(lh_addr_of(view));
        return view;
    }
    lh_str_init_by_size(lh_addr_of(view), lh_str_get_data(self), lh_str_get_size(self));
    return view;
}
