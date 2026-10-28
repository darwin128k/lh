#include <lh/http/request.h>
#include <lh/assert.h>

const lh_str_view_t *
lh_http_request_get_method(const lh_http_request_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->method);
}

lh_void
lh_http_request_set_method(lh_http_request_t *self, lh_str_view_t method)
{
    lh_assert_runtime_ref(self);
    self->method = method;
}

const lh_str_view_t *
lh_http_request_get_target(const lh_http_request_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->target);
}

lh_void
lh_http_request_set_target(lh_http_request_t *self, lh_str_view_t target)
{
    lh_assert_runtime_ref(self);
    self->target = target;
}

lh_u8_t
lh_http_request_get_version_minor(const lh_http_request_t *self)
{
    lh_assert_runtime_ref(self);
    return self->version_minor;
}

lh_void
lh_http_request_set_version_minor(lh_http_request_t *self, lh_u8_t version_minor)
{
    lh_assert_runtime_ref(self);
    self->version_minor = version_minor;
}

lh_usize_t
lh_http_request_get_header_count(const lh_http_request_t *self)
{
    lh_assert_runtime_ref(self);
    return self->header_count;
}

lh_void
lh_http_request_set_header_count(lh_http_request_t *self, lh_usize_t header_count)
{
    lh_assert_runtime_ref(self);
    self->header_count = header_count;
}
