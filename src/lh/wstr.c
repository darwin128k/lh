#include <lh/wstr.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

static void
lh_wstr_terminate(lh_wstr_t *self)
{
    lh_ptr_deref_of(lh_wchar_t, lh_vector_get_end(self)) = 0;
}

void
lh_wstr_init(lh_wstr_t *self)
{
    lh_vector_init(self, sizeof(lh_wchar_t));
    lh_vector_reserve(self, 1);
    lh_wstr_terminate(self);
}

void
lh_wstr_deinit(lh_wstr_t *self)
{
    lh_vector_deinit(self);
}

lh_wstr_cptr
lh_wstr_get_data(const lh_wstr_t *self)
{
    return lh_ptr_cast(lh_wchar_t, lh_vector_get_data(self));
}

lh_usize_t
lh_wstr_get_size(const lh_wstr_t *self)
{
    return lh_vector_get_size(self);
}

lh_bool_t
lh_wstr_is_empty(const lh_wstr_t *self)
{
    return lh_vector_is_empty(self);
}

void
lh_wstr_append(lh_wstr_t *self, lh_wstr_cptr text, lh_usize_t count)
{
    lh_vector_push_back_of(self, text, count);

    /* push_back_of only guarantees capacity >= size; top up by one more slot
     * when there's no room left for the terminator. */
    const lh_usize_t size = lh_vector_get_size(self);
    if (lh_vector_get_capacity(self) == size)
    {
        lh_vector_reserve(self, lh_vector_get_grown_capacity(size, lh_math_add_one(size)));
    }

    lh_wstr_terminate(self);
}

void
lh_wstr_push_back(lh_wstr_t *self, lh_wchar_t ch)
{
    lh_wstr_append(self, lh_addr_of(ch), 1);
}

void
lh_wstr_append_view(lh_wstr_t *self, lh_wstr_view_t view)
{
    if (lh_wstr_view_is_empty(lh_addr_of(view)))
    {
        return;
    }
    lh_wstr_append(self, lh_wstr_view_get_data(lh_addr_of(view)),
                  lh_wstr_view_get_size(lh_addr_of(view)));
}

void
lh_wstr_append_str(lh_wstr_t *self, const lh_wstr_t *other)
{
    lh_assert_runtime_ref(other);
    if (self == other)
    {
        const lh_usize_t n = lh_wstr_get_size(self);
        if (n == 0U)
        {
            return;
        }
        lh_vector_reserve(self, lh_vector_get_grown_capacity(
                                    lh_vector_get_capacity(self), lh_math_add(n + n, 1U)));
        lh_wstr_append(self, lh_wstr_get_data(self), n);
        return;
    }
    lh_wstr_append_view(self, lh_wstr_as_view(other));
}

void
lh_wstr_assign(lh_wstr_t *self, const lh_wstr_t *other)
{
    lh_assert_runtime_ref(other);
    if (self == other)
    {
        return;
    }
    lh_wstr_clear(self);
    lh_wstr_append_str(self, other);
}

void
lh_wstr_assign_view(lh_wstr_t *self, lh_wstr_view_t view)
{
    lh_wstr_clear(self);
    lh_wstr_append_view(self, view);
}

void
lh_wstr_join(lh_wstr_t *self, const lh_wstr_view_t *parts, lh_usize_t count, lh_wchar_t sep)
{
    lh_usize_t i;

    if (count > 0U)
    {
        lh_assert_runtime_ref(parts);
    }
    lh_wstr_clear(self);
    for (i = 0U; i < count; ++i)
    {
        if (i > 0U)
        {
            lh_wstr_push_back(self, sep);
        }
        lh_wstr_append_view(self, parts[i]);
    }
}

void
lh_wstr_clear(lh_wstr_t *self)
{
    lh_vector_clear(self);
    lh_wstr_terminate(self);
}

void
lh_wstr_truncate(lh_wstr_t *self, lh_usize_t n)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(n > lh_wstr_get_size(self),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_range));
    while (lh_vector_get_size(self) > n)
    {
        lh_vector_pop_back(self, lh_null);
    }
    lh_wstr_terminate(self);
}

lh_wstr_view_t
lh_wstr_as_view(const lh_wstr_t *self)
{
    lh_wstr_view_t view;
    lh_wstr_view_init_by_size(lh_addr_of(view), lh_wstr_get_data(self), lh_wstr_get_size(self));
    return view;
}
