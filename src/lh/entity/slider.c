#include <lh/entity/slider.h>
#include <lh/cast/static.h>
#include <lh/entity.h>
#include <lh/entity/circle.h>
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
    const lh_int_t width = lh_math_rect_get_size_width(lh_addr_of(bounds));
    const lh_int_t height = lh_math_rect_get_size_height(lh_addr_of(bounds));
    lh_math_rect_t track;
    lh_int_t cap;
    lh_int_t filled;
    if (lh_ptr_is_null(style) || width <= 0 || height <= 0)
    {
        return;
    }
    track = lh_entity_range_track(lh_addr_of(bounds), lh_entity_range_get_thickness(
                                                          lh_addr_of(self->range)));
    cap = lh_entity_range_cap(lh_addr_of(track));
    filled = lh_entity_range_to_local_pos(lh_addr_of(self->range));
    lh_ui_canvas_fill_round(canvas, track, cap, lh_ui_style_get_bg_color(style));
    if (filled > 0)
    {
        lh_ui_canvas_fill_round(canvas, lh_entity_range_fill(lh_addr_of(track), filled), cap,
                                lh_ui_style_get_text_color(style));
    }
}

lh_int_t
lh_entity_slider_cap(const lh_entity_2d_t *box)
{
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    const lh_math_rect_t bounds =
        lh_math_rect_make(0, 0, lh_cast_static(lh_int_t, lh_math_vec2_get_x(lh_addr_of(size))),
                          lh_cast_static(lh_int_t, lh_math_vec2_get_y(lh_addr_of(size))));
    return lh_entity_range_cap(lh_addr_of(bounds));
}

lh_void
lh_entity_slider_seat(lh_entity_slider_t *self)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_ui_style_t *const style = lh_entity_2d_get_style(box);
    lh_entity_2d_t *const thumb = lh_ptr_rcast(lh_entity_2d_t, self->thumb);
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    const lh_int_t width = lh_cast_static(lh_int_t, lh_math_vec2_get_x(lh_addr_of(size)));
    const lh_int_t height = lh_cast_static(lh_int_t, lh_math_vec2_get_y(lh_addr_of(size)));
    const lh_int_t vertical = height > width;
    const lh_int_t length = vertical != 0 ? height : width;
    const lh_int_t cross = vertical != 0 ? width : height;
    lh_int_t painted;
    lh_int_t at;
    lh_int_t filled;
    lh_math_vec2_t place;
    lh_math_vec2_t have;
    if (lh_ptr_is_null(thumb) || lh_ptr_is_null(style) || cross <= 1)
    {
        return;
    }
    lh_ui_style_set_bg_color(lh_addr_of(self->thumb_style), lh_ui_style_get_text_color(style));
    if (lh_entity_slider_cap(thumb) != cross / 2)
    {
        const lh_float_t across = lh_cast_static(lh_float_t, cross);
        lh_entity_2d_set_size(thumb, lh_math_vec2_make(across, across));
    }
    painted = lh_entity_slider_cap(thumb);
    filled = lh_entity_range_to_local_pos(lh_addr_of(self->range));
    if (filled < painted)
    {
        at = painted;
    }
    else if (filled > length - painted)
    {
        at = length - painted;
    }
    else
    {
        at = filled;
    }
    if (vertical != 0)
    {
        place = lh_math_vec2_make(lh_cast_static(lh_float_t, (width - cross) / 2),
                                  lh_cast_static(lh_float_t, height - at - painted));
    }
    else
    {
        place = lh_math_vec2_make(lh_cast_static(lh_float_t, at - painted),
                                  lh_cast_static(lh_float_t, (height - cross) / 2));
    }
    have = lh_entity_2d_get_position(thumb);
    if (lh_math_vec2_get_x(lh_addr_of(have)) != lh_math_vec2_get_x(lh_addr_of(place)) ||
        lh_math_vec2_get_y(lh_addr_of(have)) != lh_math_vec2_get_y(lh_addr_of(place)))
    {
        lh_entity_2d_set_position(thumb, place);
    }
}

lh_void
lh_entity_slider_apply(lh_entity_slider_t *self, const lh_math_vec2_t *point)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_math_rect_t bounds = lh_entity_2d_get_screen_bounds(box);
    const lh_int_t before = lh_entity_range_get_value(lh_addr_of(self->range));
    lh_int_t pos;
    lh_int_t span;
    if (lh_math_rect_get_size_height(lh_addr_of(bounds)) >
        lh_math_rect_get_size_width(lh_addr_of(bounds)))
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
    lh_entity_slider_t *const slider = lh_ptr_rcast(lh_entity_slider_t, self);
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    lh_entity_range_reset(lh_addr_of(slider->range));
    slider->thumb = lh_ptr_rcast(lh_entity_circle_t, lh_entity_create(&lh_entity_circle_class, self));
    lh_entity_add_flags(lh_ptr_rcast(lh_entity_t, slider->thumb), lh_entity_flags_event_bubble);
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, slider->thumb), lh_addr_of(slider->thumb_style));
}

lh_void
lh_entity_slider_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_slider_t *const slider = lh_ptr_rcast(lh_entity_slider_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    if (code == LH_ENTITY_EVENT_DRAW)
    {
        lh_entity_slider_seat(slider);
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
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_slider_t),
                                lh_entity_slider_construct, lh_null, lh_entity_slider_on_event);


lh_entity_range_value_defs(lh_entity_slider, lh_entity_slider_t)
