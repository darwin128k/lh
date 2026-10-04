#include <lh/entity/knob.h>
#include <lh/entity.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#include <math.h>

/* The ring runs from the lower left, through the top, to the lower right.
   Screen y grows downward, so the angle grows clockwise. */
lh_float_t
lh_entity_knob_angle(lh_float_t turns)
{
    const lh_float_t pi = 3.14159265f;
    return (0.75f + turns * 1.5f) * pi;
}

lh_float_t
lh_entity_knob_turns(lh_float_t angle)
{
    const lh_float_t pi = 3.14159265f;
    const lh_float_t start = lh_entity_knob_angle(0.0f);
    const lh_float_t sweep = lh_entity_knob_angle(1.0f) - start;
    const lh_float_t gap = (2.0f * pi - sweep) * 0.5f;
    if (angle < start)
    {
        angle += 2.0f * pi;
    }
    if (angle <= start + sweep)
    {
        return (angle - start) / sweep;
    }
    if (angle > start + sweep + gap)
    {
        return 0.0f;
    }
    return 1.0f;
}

lh_void
lh_entity_knob_apply(lh_entity_knob_t *self, const lh_math_vec2_t *point)
{
    const lh_math_rect_t bounds =
        lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_float_t cx =
        (lh_float_t)lh_math_rect_get_x(lh_addr_of(bounds)) +
        (lh_float_t)lh_math_rect_get_size_width(lh_addr_of(bounds)) * 0.5f;
    const lh_float_t cy =
        (lh_float_t)lh_math_rect_get_y(lh_addr_of(bounds)) +
        (lh_float_t)lh_math_rect_get_size_height(lh_addr_of(bounds)) * 0.5f;
    const lh_float_t angle =
        atan2f(lh_math_vec2_get_y(point) - cy, lh_math_vec2_get_x(point) - cx);
    const lh_float_t turns = lh_entity_knob_turns(angle);
    const lh_int_t before = lh_entity_range_get_value(lh_addr_of(self->range));
    lh_entity_range_set_from_pos(lh_addr_of(self->range), (lh_int_t)(turns * 1000.0f), 1000);
    if (lh_entity_range_get_value(lh_addr_of(self->range)) != before)
    {
        lh_entity_send_event(lh_ptr_rcast(lh_entity_t, self), LH_ENTITY_EVENT_CLICKED, lh_null);
    }
}

lh_void
lh_entity_knob_point(lh_int_t cx, lh_int_t cy, lh_int_t orbit, lh_float_t angle, lh_int_t *x,
                     lh_int_t *y)
{
    const lh_float_t across = cosf(angle) * (lh_float_t)orbit;
    const lh_float_t down = sinf(angle) * (lh_float_t)orbit;
    *x = cx + (lh_int_t)(across >= 0.0f ? across + 0.5f : across - 0.5f);
    *y = cy + (lh_int_t)(down >= 0.0f ? down + 0.5f : down - 0.5f);
}

lh_void
lh_entity_knob_paint(const lh_entity_knob_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_style_t *const style =
        lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_math_rect_t bounds =
        lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_int_t left = lh_math_rect_get_x(lh_addr_of(bounds));
    const lh_int_t top = lh_math_rect_get_y(lh_addr_of(bounds));
    const lh_int_t width = lh_math_rect_get_size_width(lh_addr_of(bounds));
    const lh_int_t height = lh_math_rect_get_size_height(lh_addr_of(bounds));
    const lh_int_t radius = (width < height ? width : height) / 2;
    const lh_int_t cx = left + width / 2;
    const lh_int_t cy = top + height / 2;
    const lh_float_t turns =
        radius <= 0 ? 0.0f
                    : (lh_float_t)lh_entity_range_to_pos(lh_addr_of(self->range), 1000) / 1000.0f;
    const lh_float_t start = lh_entity_knob_angle(0.0f);
    lh_int_t thumb;
    lh_int_t half;
    lh_int_t orbit;
    lh_int_t x;
    lh_int_t y;
    lh_ui_color_t mark;
    lh_ui_color_t track;
    if (lh_ptr_is_null(style) || radius <= 4)
    {
        return;
    }
    thumb = radius / 6;
    if (thumb < 4)
    {
        thumb = 4;
    }
    half = thumb / 2;
    if (half < 2)
    {
        half = 2;
    }
    orbit = radius - thumb - 1;
    if (orbit <= half)
    {
        return;
    }
    mark = lh_ui_style_get_text_color(style);
    track = lh_ui_style_get_bg_color(style);
    lh_ui_canvas_fill_arc(canvas, cx, cy, orbit + half, orbit - half, start,
                          lh_entity_knob_angle(1.0f), track);
    lh_entity_knob_point(cx, cy, orbit, start, lh_addr_of(x), lh_addr_of(y));
    lh_ui_canvas_fill_disc(canvas, x, y, half, track);
    lh_entity_knob_point(cx, cy, orbit, lh_entity_knob_angle(1.0f), lh_addr_of(x), lh_addr_of(y));
    lh_ui_canvas_fill_disc(canvas, x, y, half, track);
    if (turns > 0.0f)
    {
        lh_ui_canvas_fill_arc(canvas, cx, cy, orbit + half, orbit - half, start,
                              lh_entity_knob_angle(turns), mark);
        lh_entity_knob_point(cx, cy, orbit, start, lh_addr_of(x), lh_addr_of(y));
        lh_ui_canvas_fill_disc(canvas, x, y, half, mark);
    }
    lh_entity_knob_point(cx, cy, orbit, lh_entity_knob_angle(turns), lh_addr_of(x), lh_addr_of(y));
    lh_ui_canvas_fill_disc(canvas, x, y, thumb, mark);
}

lh_void
lh_entity_knob_construct(lh_entity_t *self)
{
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    lh_entity_range_reset(lh_addr_of(lh_ptr_rcast(lh_entity_knob_t, self)->range));
}

lh_void
lh_entity_knob_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_knob_t *const knob = lh_ptr_rcast(lh_entity_knob_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    if (code == LH_ENTITY_EVENT_DRAW)
    {
        lh_entity_knob_paint(knob, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        knob->dragging = lh_bool_true;
        lh_entity_knob_apply(knob, lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_MOVE && knob->dragging)
    {
        lh_entity_knob_apply(knob, lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_UP)
    {
        knob->dragging = lh_bool_false;
    }
}

const lh_entity_class_t lh_entity_knob_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_knob_t),
                                lh_entity_knob_construct, lh_null, lh_entity_knob_on_event);
