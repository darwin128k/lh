#include <lh/str.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

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
