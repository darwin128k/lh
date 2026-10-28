#include <lh/http/header.h>
#include <lh/assert.h>

const lh_str_view_t *
lh_http_header_get_name(const lh_http_header_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

lh_void
lh_http_header_set_name(lh_http_header_t *self, lh_str_view_t name)
{
    lh_assert_runtime_ref(self);
    self->name = name;
}

const lh_str_view_t *
lh_http_header_get_value(const lh_http_header_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->value);
}

lh_void
lh_http_header_set_value(lh_http_header_t *self, lh_str_view_t value)
{
    lh_assert_runtime_ref(self);
    self->value = value;
}
