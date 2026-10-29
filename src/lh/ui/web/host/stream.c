/**
 * @file stream.c
 * @brief Level-1 accessors of ::lh_ui_web_host_stream_t.
 */

#include <lh/ui/web/host/stream.h>


const lh_ui_canvas_web_glyphs_t *
lh_ui_web_host_stream_get_glyphs(const lh_ui_web_host_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->glyphs);
}

lh_ui_canvas_web_glyphs_t *
lh_ui_web_host_stream_get_glyphs_mut(lh_ui_web_host_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->glyphs);
}

lh_void
lh_ui_web_host_stream_set_glyphs(lh_ui_web_host_stream_t *self, lh_ui_canvas_web_glyphs_t glyphs)
{
    lh_assert_runtime_ref(self);
    self->glyphs = glyphs;
}

lh_str_ptr
lh_ui_web_host_stream_get_last(const lh_ui_web_host_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last;
}

lh_void
lh_ui_web_host_stream_set_last(lh_ui_web_host_stream_t *self, lh_str_ptr last)
{
    lh_assert_runtime_ref(self);
    self->last = last;
}

lh_usize_t
lh_ui_web_host_stream_get_last_len(const lh_ui_web_host_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_len;
}

lh_void
lh_ui_web_host_stream_set_last_len(lh_ui_web_host_stream_t *self, lh_usize_t last_len)
{
    lh_assert_runtime_ref(self);
    self->last_len = last_len;
}

lh_usize_t
lh_ui_web_host_stream_get_last_cap(const lh_ui_web_host_stream_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_cap;
}

lh_void
lh_ui_web_host_stream_set_last_cap(lh_ui_web_host_stream_t *self, lh_usize_t last_cap)
{
    lh_assert_runtime_ref(self);
    self->last_cap = last_cap;
}
