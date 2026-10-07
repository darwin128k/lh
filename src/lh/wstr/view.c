#include <lh/wstr/view.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/math.h>
#include <lh/util/ptr.h>
#include <lh/util/wstr/ptr.h>

lh_void
lh_wstr_view_init_by_size(lh_wstr_view_t *self, lh_wstr_cptr data, lh_usize_t size)
{
    lh_memory_view_init_by_size(self, data, size * lh_cast_static(lh_usize_t, LH_WCHAR_T_SIZE));
}

lh_void
lh_wstr_view_init(lh_wstr_view_t *self, lh_wstr_cptr data)
{
    const lh_usize_t size = lh_wstr_ptr_len(data);

    /* An empty wide string is a present value: L"" is not a null pointer. by_size
       rejects size 0, so the endpoints are stored here instead, which is exactly
       what lh_wstr_view_lit(L"") yields — initialized and empty, not uninitialized.
       A null data pointer dies in ptr_len above: null is an error, not empty. */
    if (lh_math_is_zero(size))
    {
        lh_memory_view_set(self, data, data);
        return;
    }
    lh_wstr_view_init_by_size(self, data, size);
}

lh_void
lh_wstr_view_init_empty(lh_wstr_view_t *self)
{
    lh_memory_view_init_empty(self);
}

lh_void
lh_wstr_view_init_by_other(lh_wstr_view_t *self, const lh_wstr_view_t *other)
{
    lh_memory_view_init_by_other(self, other);
}

lh_wstr_view_t
lh_wstr_view_from_offset(const lh_wstr_view_t *self, lh_uoffset_t offset, lh_usize_t size)
{
    /* Memory views count bytes; wide views count characters. */
    return lh_memory_view_from_offset(
        self, lh_math_mul(offset, lh_cast_static(lh_uoffset_t, LH_WCHAR_T_SIZE)),
        lh_math_mul(size, lh_cast_static(lh_usize_t, LH_WCHAR_T_SIZE)));
}

lh_wstr_view_t
lh_wstr_view_tail(const lh_wstr_view_t *self, lh_uoffset_t offset)
{
    const lh_usize_t size = lh_wstr_view_is_empty(self) ? 0U : lh_wstr_view_get_size(self);

    lh_assert_runtime_if(lh_math_gt(offset, size), lh_runtime_error_code_out_of_range);
    if (lh_math_eq(offset, size))
    {
        /* At the boundary nothing is left. That is an explicitly empty view, not a
           null pointer: views reject size 0, so init(lh_null) would die here. */
        lh_wstr_view_t _lh_tmp;
        lh_wstr_view_init_empty(lh_addr_of(_lh_tmp));
        return _lh_tmp;
    }
    return lh_wstr_view_from_offset(self, offset, lh_math_sub(size, offset));
}

lh_wstr_cptr
lh_wstr_view_get_begin(const lh_wstr_view_t *self)
{
    return lh_ptr_cast(lh_wchar_t, lh_memory_view_get_begin_v(self));
}

lh_wstr_cptr
lh_wstr_view_get_end(const lh_wstr_view_t *self)
{
    return lh_ptr_cast(lh_wchar_t, lh_memory_view_get_end_v(self));
}

lh_wstr_cptr
lh_wstr_view_get_data(const lh_wstr_view_t *self)
{
    return lh_ptr_cast(lh_wchar_t, lh_memory_view_get_data(self));
}

lh_usize_t
lh_wstr_view_get_size(const lh_wstr_view_t *self)
{
    return lh_memory_view_get_size(self) / lh_cast_static(lh_usize_t, LH_WCHAR_T_SIZE);
}

lh_bool_t
lh_wstr_view_is_empty(const lh_wstr_view_t *self)
{
    return lh_memory_view_is_empty(self);
}
