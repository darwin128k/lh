/**
 * @file frame.c
 * @brief Level-1 accessors of ::lh_ui_web_host_frame_t.
 */

#include <lh/ui/web/host/frame.h>


struct lh_ui_web_host_frame *
lh_ui_web_host_frame_get_next(const lh_ui_web_host_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_void
lh_ui_web_host_frame_set_next(lh_ui_web_host_frame_t *self, struct lh_ui_web_host_frame * next)
{
    lh_assert_runtime_ref(self);
    self->next = next;
}

lh_byte_ptr
lh_ui_web_host_frame_get_bytes(const lh_ui_web_host_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bytes;
}

lh_void
lh_ui_web_host_frame_set_bytes(lh_ui_web_host_frame_t *self, lh_byte_ptr bytes)
{
    lh_assert_runtime_ref(self);
    self->bytes = bytes;
}

lh_uint_t
lh_ui_web_host_frame_get_len(const lh_ui_web_host_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->len;
}

lh_void
lh_ui_web_host_frame_set_len(lh_ui_web_host_frame_t *self, lh_uint_t len)
{
    lh_assert_runtime_ref(self);
    self->len = len;
}

lh_uint_t
lh_ui_web_host_frame_get_off(const lh_ui_web_host_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->off;
}

lh_void
lh_ui_web_host_frame_set_off(lh_ui_web_host_frame_t *self, lh_uint_t off)
{
    lh_assert_runtime_ref(self);
    self->off = off;
}
