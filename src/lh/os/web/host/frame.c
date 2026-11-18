/**
 * @file frame.c
 * @brief Level-1 accessors of ::lh_os_web_host_frame_t.
 */

#include <lh/os/web/host/frame.h>
#include <lh/core/assert.h>

struct lh_os_web_host_frame *
lh_os_web_host_frame_get_next(const lh_os_web_host_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_void
lh_os_web_host_frame_set_next(lh_os_web_host_frame_t *self, struct lh_os_web_host_frame *next)
{
    lh_assert_runtime_ref(self);
    self->next = next;
}

lh_byte_ptr
lh_os_web_host_frame_get_bytes(const lh_os_web_host_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bytes;
}

lh_void
lh_os_web_host_frame_set_bytes(lh_os_web_host_frame_t *self, lh_byte_ptr bytes)
{
    lh_assert_runtime_ref(self);
    self->bytes = bytes;
}

lh_uint_t
lh_os_web_host_frame_get_len(const lh_os_web_host_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->len;
}

lh_void
lh_os_web_host_frame_set_len(lh_os_web_host_frame_t *self, lh_uint_t len)
{
    lh_assert_runtime_ref(self);
    self->len = len;
}

lh_uint_t
lh_os_web_host_frame_get_off(const lh_os_web_host_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->off;
}

lh_void
lh_os_web_host_frame_set_off(lh_os_web_host_frame_t *self, lh_uint_t off)
{
    lh_assert_runtime_ref(self);
    self->off = off;
}
