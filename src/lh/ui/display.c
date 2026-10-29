/**
 * @file display.c
 * @brief ::lh_ui_display_t — host buffer + sw canvas + present sinks.
 */
#include <lh/ui/display.h>
#include <lh/math.h>
#include <lh/assert.h>

lh_void
lh_ui_display_rebind_pixmap(lh_ui_display_t *self, lh_byte_ptr bits, lh_s32_t stride)
{
    lh_s32_t w = lh_cast_static(lh_s32_t, lh_ui_size_get_width(lh_ui_display_get_panel_mut(self)));
    lh_s32_t h = lh_cast_static(lh_s32_t, lh_ui_size_get_height(lh_ui_display_get_panel_mut(self)));
    lh_s32_t bh = h;

    if (lh_math_gt(lh_ui_display_get_strip_height(self), lh_ui_scalar(0)))
    {
        const lh_s32_t sh = lh_cast_static(lh_s32_t, lh_ui_display_get_strip_height(self));
        if (sh > 0 && sh < h)
        {
            bh = sh;
        }
    }
    if (bits == lh_null || w <= 0 || bh <= 0 || stride <= 0)
    {
        lh_ui_pixmap_init_empty(lh_ui_display_get_pixmap_mut(self));
    }
    else
    {
        lh_ui_pixmap_init(lh_ui_display_get_pixmap_mut(self), bits, w, bh, stride, lh_ui_display_get_format(self));
    }
    lh_ui_canvas_sw_use_pixmap(lh_ui_display_get_sw_mut(self), lh_ui_display_get_pixmap_mut(self));
}

lh_void
lh_ui_display_init(lh_ui_display_t *self)
{
    lh_u32_t i;

    lh_ui_size_init(lh_ui_display_get_panel_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_display_set_strip_height(self, lh_ui_scalar(0));
    lh_ui_display_set_strip_y(self, lh_ui_scalar(0));
    lh_ui_display_set_format(self, lh_ui_pixmap_format_argb8888);
    lh_ui_pixmap_init_empty(lh_ui_display_get_pixmap_mut(self));
    lh_ui_canvas_sw_init(lh_ui_display_get_sw_mut(self));
    lh_ui_canvas_init(lh_ui_display_get_canvas_mut(self), lh_addr_of(lh_ui_canvas_backend_sw),
                      lh_ui_display_get_sw_mut(self));
    for (i = 0; i < (lh_u32_t)LH_UI_PRESENT_SINKS_MAX; ++i)
    {
        lh_ui_present_sink_init(lh_addr_of(lh_ui_display_get_sinks(self)[i]), lh_null, lh_null, lh_null);
    }
    lh_ui_display_set_sink_count(self, 0);
    lh_ui_display_set_sink_enable(self, 0);
}

lh_void
lh_ui_display_set_panel_v(lh_ui_display_t *self, lh_ui_size_t panel)
{
    lh_ui_display_set_panel(self, panel);
    lh_ui_canvas_set_size(lh_ui_display_get_canvas_mut(self), panel);
}

lh_void
lh_ui_display_set_format(lh_ui_display_t *self, lh_ui_pixmap_format_t format)
{
    lh_assert_runtime_ref(self);
    self->format = format;
}

lh_void
lh_ui_display_set_strip_height(lh_ui_display_t *self, lh_ui_scalar_t height)
{
    lh_assert_runtime_ref(self);
    self->strip_height = height;
}

lh_void
lh_ui_display_set_buffer(lh_ui_display_t *self, lh_byte_ptr bits, lh_s32_t stride)
{
    lh_ui_display_rebind_pixmap(self, bits, stride);
}

lh_void
lh_ui_display_set_strip_y(lh_ui_display_t *self, lh_ui_scalar_t y)
{
    lh_assert_runtime_ref(self);
    self->strip_y = y;
}

lh_void
lh_ui_display_set_sink(lh_ui_display_t *self, lh_u32_t index, const lh_ui_present_sink_t *sink)
{
    if (index >= (lh_u32_t)LH_UI_PRESENT_SINKS_MAX || sink == lh_null)
    {
        return;
    }
    lh_ui_display_get_sinks_mut(self)[index] = lh_ptr_deref(sink);
    if (lh_ui_display_get_sink_count(self) <= index)
    {
        lh_ui_display_set_sink_count(self, index + 1u);
    }
    lh_ui_display_set_sink_enable(self, lh_ui_display_get_sink_enable(self) | ((1u << index)));
}

lh_void
lh_ui_display_set_sink_enabled(lh_ui_display_t *self, lh_u32_t index, lh_bool_t enabled)
{
    if (index >= (lh_u32_t)LH_UI_PRESENT_SINKS_MAX)
    {
        return;
    }
    if (enabled)
    {
        lh_ui_display_set_sink_enable(self, lh_ui_display_get_sink_enable(self) | ((1u << index)));
    }
    else
    {
        lh_ui_display_set_sink_enable(self, lh_ui_display_get_sink_enable(self) & (~(1u << index)));
    }
}

lh_bool_t
lh_ui_display_is_sink_enabled(const lh_ui_display_t *self, lh_u32_t index)
{
    if (index >= lh_ui_display_get_sink_count(self))
    {
        return lh_bool_false;
    }
    return ((lh_ui_display_get_sink_enable(self) & (1u << index)) != 0u) ? lh_bool_true : lh_bool_false;
}

lh_ui_canvas_t *
lh_ui_display_get_canvas(lh_ui_display_t *self)
{
    return lh_ui_display_get_canvas_mut(self);
}

const lh_ui_pixmap_t *
lh_ui_display_get_pixmap(const lh_ui_display_t *self)
{
    return lh_addr_of(self->pixmap);
}

lh_void
lh_ui_display_present(lh_ui_display_t *self, const lh_ui_rects_t *drawn)
{
    lh_ui_present_run(lh_ui_display_get_sinks(self), lh_ui_display_get_sink_count(self), lh_ui_display_get_sink_enable(self), lh_ui_display_get_pixmap_mut(self),
                      drawn);
}

const lh_ui_size_t *
lh_ui_display_get_panel(const lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->panel);
}

lh_ui_size_t *
lh_ui_display_get_panel_mut(lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->panel);
}

lh_ui_scalar_t
lh_ui_display_get_strip_height(const lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return self->strip_height;
}

lh_ui_scalar_t
lh_ui_display_get_strip_y(const lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return self->strip_y;
}

lh_ui_pixmap_format_t
lh_ui_display_get_format(const lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return self->format;
}

lh_ui_pixmap_t *
lh_ui_display_get_pixmap_mut(lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pixmap);
}

lh_void
lh_ui_display_set_pixmap(lh_ui_display_t *self, lh_ui_pixmap_t pixmap)
{
    lh_assert_runtime_ref(self);
    self->pixmap = pixmap;
}

const lh_ui_canvas_sw_t *
lh_ui_display_get_sw(const lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sw);
}

lh_ui_canvas_sw_t *
lh_ui_display_get_sw_mut(lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sw);
}

lh_void
lh_ui_display_set_sw(lh_ui_display_t *self, lh_ui_canvas_sw_t sw)
{
    lh_assert_runtime_ref(self);
    self->sw = sw;
}

lh_ui_canvas_t *
lh_ui_display_get_canvas_mut(lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->canvas);
}

lh_void
lh_ui_display_set_canvas(lh_ui_display_t *self, lh_ui_canvas_t canvas)
{
    lh_assert_runtime_ref(self);
    self->canvas = canvas;
}

const lh_ui_present_sink_t *
lh_ui_display_get_sinks(const lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sinks;
}

lh_ui_present_sink_t *
lh_ui_display_get_sinks_mut(lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sinks;
}

lh_u32_t
lh_ui_display_get_sink_count(const lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sink_count;
}

lh_void
lh_ui_display_set_sink_count(lh_ui_display_t *self, lh_u32_t sink_count)
{
    lh_assert_runtime_ref(self);
    self->sink_count = sink_count;
}

lh_u32_t
lh_ui_display_get_sink_enable(const lh_ui_display_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sink_enable;
}

lh_void
lh_ui_display_set_sink_enable(lh_ui_display_t *self, lh_u32_t sink_enable)
{
    lh_assert_runtime_ref(self);
    self->sink_enable = sink_enable;
}

lh_void
lh_ui_display_set_panel(lh_ui_display_t *self, lh_ui_size_t panel)
{
    lh_assert_runtime_ref(self);
    self->panel = panel;
}



