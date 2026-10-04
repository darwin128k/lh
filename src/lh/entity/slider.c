#include <lh/entity/slider.h>
#include <lh/entity.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_slider_paint(const lh_entity_slider_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_style_t *const style =
        lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_math_rect_t bounds =
        lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_int_t x = lh_math_rect_get_x(lh_addr_of(bounds));
    const lh_int_t y = lh_math_rect_get_y(lh_addr_of(bounds));
    const lh_int_t width = lh_math_rect_get_size_width(lh_addr_of(bounds));
    const lh_int_t height = lh_math_rect_get_size_height(lh_addr_of(bounds));
    const lh_int_t vertical = height > width;
    const lh_int_t length = vertical != 0 ? height : width;
    const lh_int_t cross = vertical != 0 ? width : height;
    lh_int_t thick = 4;
    lh_int_t radius = cross / 2;
    lh_int_t filled;
    lh_int_t at;
    if (lh_ptr_is_null(style) || width <= 0 || height <= 0)
    {
        return;
    }
    if (thick > cross)
    {
        thick = cross;
    }
    if (radius > 7)
    {
        radius = 7;
    }
    if (radius < thick)
    {
        radius = thick;
    }
    filled = lh_entity_range_to_pos(lh_addr_of(self->range), length);
    if (filled < radius)
    {
        at = radius;
    }
    else if (filled > length - radius)
    {
        at = length - radius;
    }
    else
    {
        at = filled;
    }
    if (vertical != 0)
    {
        const lh_int_t track_x = x + (width - thick) / 2;
        const lh_int_t cap = thick / 2;
        lh_ui_canvas_fill_round(canvas, lh_math_rect_make(track_x, y, thick, height), cap,
                                lh_ui_style_get_bg_color(style));
        if (filled > 0)
        {
            lh_ui_canvas_fill_round(canvas,
                                    lh_math_rect_make(track_x, y + height - filled, thick, filled),
                                    cap, lh_ui_style_get_text_color(style));
        }
        lh_ui_canvas_fill_disc(canvas, x + width / 2, y + height - at, radius,
                              lh_ui_style_get_text_color(style));
    }
    else
    {
        const lh_int_t track_y = y + (height - thick) / 2;
        const lh_int_t cap = thick / 2;
        lh_ui_canvas_fill_round(canvas, lh_math_rect_make(x, track_y, width, thick), cap,
                                lh_ui_style_get_bg_color(style));
        if (filled > 0)
        {
            lh_ui_canvas_fill_round(canvas, lh_math_rect_make(x, track_y, filled, thick), cap,
                                    lh_ui_style_get_text_color(style));
        }
        lh_ui_canvas_fill_disc(canvas, x + at, y + height / 2, radius,
                              lh_ui_style_get_text_color(style));
    }
}

lh_void
lh_entity_slider_apply(lh_entity_slider_t *self, const lh_math_vec2_t *point)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_math_rect_t bounds = lh_entity_2d_get_screen_bounds(box);
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    const lh_int_t before = lh_entity_range_get_value(lh_addr_of(self->range));
    lh_int_t pos;
    lh_int_t span;
    if (lh_math_vec2_get_y(lh_addr_of(size)) > lh_math_vec2_get_x(lh_addr_of(size)))
    {
        pos = (lh_int_t)lh_math_vec2_get_y(point) - lh_math_rect_get_y(lh_addr_of(bounds));
        span = lh_math_rect_get_size_height(lh_addr_of(bounds));
    }
    else
    {
        pos = (lh_int_t)lh_math_vec2_get_x(point) - lh_math_rect_get_x(lh_addr_of(bounds));
        span = lh_math_rect_get_size_width(lh_addr_of(bounds));
    }
    lh_entity_range_set_from_pos(lh_addr_of(self->range), pos, span);
    if (lh_entity_range_get_value(lh_addr_of(self->range)) != before)
    {
        lh_entity_send_event(lh_ptr_rcast(lh_entity_t, self), LH_ENTITY_EVENT_CLICKED, lh_null);
    }
}

lh_void
lh_entity_slider_construct(lh_entity_t *self)
{
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    lh_entity_range_reset(lh_addr_of(lh_ptr_rcast(lh_entity_slider_t, self)->range));
}

lh_void
lh_entity_slider_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_slider_t *const slider = lh_ptr_rcast(lh_entity_slider_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    if (code == LH_ENTITY_EVENT_DRAW)
    {
        lh_entity_slider_paint(slider, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        slider->dragging = lh_bool_true;
        lh_entity_slider_apply(slider, lh_ptr_rcast(const lh_math_vec2_t,
                                                    lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_MOVE && slider->dragging)
    {
        lh_entity_slider_apply(slider, lh_ptr_rcast(const lh_math_vec2_t,
                                                    lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_UP)
    {
        slider->dragging = lh_bool_false;
    }
}

const lh_entity_class_t lh_entity_slider_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_slider_t),
                                lh_entity_slider_construct, lh_null, lh_entity_slider_on_event);
