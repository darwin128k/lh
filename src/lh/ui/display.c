/**
 * @file display.c
 * @brief ::lh_ui_display_t — host buffer + sw canvas + present sinks.
 */
#include <lh/ui/display.h>

#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/canvas/backend.h>
#include <lh/util/addr.h>

static lh_void
display_rebind_pixmap(lh_ui_display_t *self, lh_byte_ptr bits, lh_s32_t stride)
{
    lh_s32_t w = lh_cast_static(lh_s32_t, lh_ui_size_get_width(lh_addr_of(self->panel)));
    lh_s32_t h = lh_cast_static(lh_s32_t, lh_ui_size_get_height(lh_addr_of(self->panel)));
    lh_s32_t bh = h;

    if (lh_math_gt(self->strip_height, lh_ui_scalar(0)))
    {
        const lh_s32_t sh = lh_cast_static(lh_s32_t, self->strip_height);
        if (sh > 0 && sh < h)
        {
            bh = sh;
        }
    }
    if (bits == lh_null || w <= 0 || bh <= 0 || stride <= 0)
    {
        lh_ui_pixmap_init_empty(lh_addr_of(self->pixmap));
    }
    else
    {
        lh_ui_pixmap_init(lh_addr_of(self->pixmap), bits, w, bh, stride, self->format);
    }
    lh_ui_canvas_sw_set_pixmap(lh_addr_of(self->sw), lh_addr_of(self->pixmap));
}

lh_void
lh_ui_display_init(lh_ui_display_t *self)
{
    lh_u32_t i;

    lh_ui_size_init(lh_addr_of(self->panel), lh_ui_scalar(0), lh_ui_scalar(0));
    self->strip_height = lh_ui_scalar(0);
    self->strip_y = lh_ui_scalar(0);
    self->format = lh_ui_pixmap_format_argb8888;
    lh_ui_pixmap_init_empty(lh_addr_of(self->pixmap));
    lh_ui_canvas_sw_init(lh_addr_of(self->sw));
    lh_ui_canvas_init(lh_addr_of(self->canvas), &lh_ui_canvas_backend_sw, lh_addr_of(self->sw));
    for (i = 0; i < (lh_u32_t)LH_UI_PRESENT_SINKS_MAX; ++i)
    {
        lh_ui_present_sink_init(lh_addr_of(self->sinks[i]), lh_null, lh_null, lh_null);
    }
    self->sink_count = 0;
    self->sink_enable = 0;
}

lh_void
lh_ui_display_set_panel(lh_ui_display_t *self, lh_ui_size_t panel)
{
    self->panel = panel;
    lh_ui_canvas_set_size(lh_addr_of(self->canvas), panel);
}

lh_void
lh_ui_display_set_format(lh_ui_display_t *self, lh_ui_pixmap_format_t format)
{
    self->format = format;
}

lh_void
lh_ui_display_set_strip_height(lh_ui_display_t *self, lh_ui_scalar_t height)
{
    self->strip_height = height;
}

lh_void
lh_ui_display_set_buffer(lh_ui_display_t *self, lh_byte_ptr bits, lh_s32_t stride)
{
    display_rebind_pixmap(self, bits, stride);
}

lh_void
lh_ui_display_set_strip_y(lh_ui_display_t *self, lh_ui_scalar_t y)
{
    self->strip_y = y;
}

lh_void
lh_ui_display_set_sink(lh_ui_display_t *self, lh_u32_t index, const lh_ui_present_sink_t *sink)
{
    if (index >= (lh_u32_t)LH_UI_PRESENT_SINKS_MAX || sink == lh_null)
    {
        return;
    }
    self->sinks[index] = *sink;
    if (self->sink_count <= index)
    {
        self->sink_count = index + 1u;
    }
    self->sink_enable |= (1u << index);
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
        self->sink_enable |= (1u << index);
    }
    else
    {
        self->sink_enable &= ~(1u << index);
    }
}

lh_bool_t
lh_ui_display_is_sink_enabled(const lh_ui_display_t *self, lh_u32_t index)
{
    if (index >= self->sink_count)
    {
        return lh_bool_false;
    }
    return ((self->sink_enable & (1u << index)) != 0u) ? lh_bool_true : lh_bool_false;
}

lh_ui_canvas_t *
lh_ui_display_get_canvas(lh_ui_display_t *self)
{
    return lh_addr_of(self->canvas);
}

const lh_ui_pixmap_t *
lh_ui_display_get_pixmap(const lh_ui_display_t *self)
{
    return lh_addr_of(self->pixmap);
}

lh_void
lh_ui_display_present(lh_ui_display_t *self, const lh_ui_rects_t *drawn)
{
    lh_ui_present_run(self->sinks, self->sink_count, self->sink_enable, lh_addr_of(self->pixmap),
                      drawn);
}
