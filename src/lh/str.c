#include <lh/str.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/str/format/text.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

static lh_void
lh_str_terminate(lh_str_t *self)
{
    lh_ptr_deref_of(lh_char_t, lh_vector_get_end(self)) = '\0';
}

lh_void
lh_str_init(lh_str_t *self)
{
    lh_vector_init(self, sizeof(lh_char_t));
    lh_vector_reserve(self, 1);
    lh_str_terminate(self);
}

lh_void
lh_str_deinit(lh_str_t *self)
{
    lh_vector_deinit(self);
}

lh_str_cptr
lh_str_get_data(const lh_str_t *self)
{
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
    lh_vector_push_back_of(self, text, count);

    /* push_back_of only guarantees capacity >= size; top up by one more slot
     * when there's no room left for the terminator. */
    const lh_usize_t size = lh_vector_get_size(self);
    if (lh_vector_get_capacity(self) == size)
    {
        lh_vector_reserve(self, lh_vector_get_grown_capacity(size, lh_math_add_one(size)));
    }

    lh_str_terminate(self);
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
lh_str_clear(lh_str_t *self)
{
    lh_vector_clear(self);
    lh_str_terminate(self);
}

lh_void
lh_str_truncate(lh_str_t *self, lh_usize_t n)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(n > lh_str_get_size(self),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_range));
    while (lh_vector_get_size(self) > n)
    {
        lh_vector_pop_back(self, lh_null);
    }
    lh_str_terminate(self);
}

lh_str_view_t
lh_str_as_view(const lh_str_t *self)
{
    lh_str_view_t view;
    lh_str_init_by_size(&view, lh_str_get_data(self), lh_str_get_size(self));
    return view;
}
