#include <lh/str/list/span.h>
#include <lh/assert.h>

lh_usize_t
lh_str_list_span_get_offset(const lh_str_list_span_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset;
}

lh_usize_t
lh_str_list_span_get_size(const lh_str_list_span_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_void
lh_str_list_span_set_offset(lh_str_list_span_t *self, lh_usize_t offset)
{
    lh_assert_runtime_ref(self);
    self->offset = offset;
}

lh_void
lh_str_list_span_set_size(lh_str_list_span_t *self, lh_usize_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

lh_void
lh_str_list_span_init(lh_str_list_span_t *self, lh_usize_t offset, lh_usize_t size)
{
    lh_str_list_span_set_offset(self, offset);
    lh_str_list_span_set_size(self, size);
}
