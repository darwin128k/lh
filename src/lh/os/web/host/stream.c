/**
 * @file stream.c
 * @brief Level-1 accessors of ::lh_os_web_host_stream_t.
 */

#include <lh/os/web/host/stream.h>
#include <lh/core/assert.h>

lh_str_ptr
lh_os_web_host_stream_get_last(const lh_os_web_host_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last;
}

lh_void
lh_os_web_host_stream_set_last(lh_os_web_host_stream_t *self, lh_str_ptr last)
{
    lh_assert_runtime_ref(self);
    self->last = last;
}

lh_usize_t
lh_os_web_host_stream_get_last_len(const lh_os_web_host_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_len;
}

lh_void
lh_os_web_host_stream_set_last_len(lh_os_web_host_stream_t *self, lh_usize_t last_len)
{
    lh_assert_runtime_ref(self);
    self->last_len = last_len;
}

lh_usize_t
lh_os_web_host_stream_get_last_cap(const lh_os_web_host_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_cap;
}

lh_void
lh_os_web_host_stream_set_last_cap(lh_os_web_host_stream_t *self, lh_usize_t last_cap)
{
    lh_assert_runtime_ref(self);
    self->last_cap = last_cap;
}
