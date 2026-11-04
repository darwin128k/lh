#include <lh/core/http/header.h>
#include <lh/core/assert.h>

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

lh_void
lh_http_header_set(lh_http_header_t *self, lh_str_view_t name, lh_str_view_t value)
{
    lh_http_header_set_name(self, name);
    lh_http_header_set_value(self, value);
}

const lh_http_header_t *
lh_http_header_find(const lh_http_header_t *headers, lh_usize_t count, const lh_str_view_t *name)
{
    for (lh_usize_t i = 0U; i < count; i = lh_math_add_one(i))
    {
        if (lh_str_view_equals(lh_http_header_get_name(lh_addr_of(headers[i])), name, lh_bool_true))
        {
            return lh_addr_of(headers[i]);
        }
    }
    return lh_null;
}
