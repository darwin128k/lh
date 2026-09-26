#include <lh/wstr/view.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
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
    lh_wstr_view_init_by_size(self, data, lh_wstr_ptr_len(data));
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
lh_wstr_view_make(lh_wstr_cptr data)
{
    lh_wstr_view_t self;

    if (lh_null_eq(data) || data[0] == L'\0')
    {
        lh_wstr_view_init_empty(lh_addr_of(self));
        return self;
    }
    lh_wstr_view_init(lh_addr_of(self), data);
    return self;
}

lh_wstr_view_t
lh_wstr_view_make_from_offset(const lh_wstr_view_t *self, lh_uoffset_t offset, lh_usize_t size)
{
    /* Memory views count bytes; wide views count characters. */
    return lh_memory_view_make_from_offset(
        self, lh_math_mul(offset, lh_cast_static(lh_uoffset_t, LH_WCHAR_T_SIZE)),
        lh_math_mul(size, lh_cast_static(lh_usize_t, LH_WCHAR_T_SIZE)));
}

lh_wstr_view_t
lh_wstr_view_make_tail(const lh_wstr_view_t *self, lh_uoffset_t offset)
{
    const lh_usize_t size = lh_wstr_view_is_empty(self) ? 0U : lh_wstr_view_get_size(self);

    lh_assert_runtime_if(lh_math_gt(offset, size), lh_runtime_error_code_out_of_range);
    if (lh_math_eq(offset, size))
    {
        return lh_wstr_view_make(lh_null); /* views reject size 0 */
    }
    return lh_wstr_view_make_from_offset(self, offset, lh_math_sub(size, offset));
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
