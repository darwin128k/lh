#include <lh/http/response.h>
#include <lh/assert.h>

const lh_str_view_t *
lh_http_response_get_reason(const lh_http_response_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->reason);
}

lh_void
lh_http_response_set_reason(lh_http_response_t *self, lh_str_view_t reason)
{
    lh_assert_runtime_ref(self);
    self->reason = reason;
}

lh_u16_t
lh_http_response_get_status(const lh_http_response_t *self)
{
    lh_assert_runtime_ref(self);
    return self->status;
}

lh_void
lh_http_response_set_status(lh_http_response_t *self, lh_u16_t status)
{
    lh_assert_runtime_ref(self);
    self->status = status;
}

lh_u8_t
lh_http_response_get_version_minor(const lh_http_response_t *self)
{
    lh_assert_runtime_ref(self);
    return self->version_minor;
}

lh_void
lh_http_response_set_version_minor(lh_http_response_t *self, lh_u8_t version_minor)
{
    lh_assert_runtime_ref(self);
    self->version_minor = version_minor;
}

lh_usize_t
lh_http_response_get_header_count(const lh_http_response_t *self)
{
    lh_assert_runtime_ref(self);
    return self->header_count;
}

lh_void
lh_http_response_set_header_count(lh_http_response_t *self, lh_usize_t header_count)
{
    lh_assert_runtime_ref(self);
    self->header_count = header_count;
}
