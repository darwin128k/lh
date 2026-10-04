#include <lh/entity/knob.h>
#include <lh/cast/static.h>
#include <lh/entity.h>
#include <lh/entity/circle.h>
#include <lh/math/pi.h>
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
    return (0.75f + turns * 1.5f) * LH_MATH_PI;
}

lh_float_t
lh_entity_knob_turns(lh_float_t angle)
{
    const lh_float_t start = lh_entity_knob_angle(0.0f);
    const lh_float_t sweep = lh_entity_knob_angle(1.0f) - start;
    const lh_float_t gap = (2.0f * LH_MATH_PI - sweep) * 0.5f;
    if (angle < start)
    {
        angle += 2.0f * LH_MATH_PI;
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

/* Half the shorter side. That is the radius the circle paints. */
lh_int_t
lh_entity_knob_radius(const lh_entity_2d_t *box)
{
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    const lh_int_t width = lh_cast_static(lh_int_t, lh_math_vec2_get_x(lh_addr_of(size)));
    const lh_int_t height = lh_cast_static(lh_int_t, lh_math_vec2_get_y(lh_addr_of(size)));
    const lh_math_rect_t bounds = lh_math_rect_make(0, 0, width, height);
    return lh_entity_range_cap(lh_addr_of(bounds));
}

/* The dial holds eight of the handle's radii: the reach from the middle,
   the handle, a gap, and a track two handles thick. An empty handle takes
   that size, so the track's round end is the handle. */
lh_void
lh_entity_knob_fit(lh_entity_knob_t *self)
{
    const lh_entity_2d_t *const thumb = lh_ptr_rcast(const lh_entity_2d_t, self->thumb);
    const lh_int_t dial = lh_entity_knob_radius(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_int_t radius = dial / (4 + 1 + 1 + 2);
    const lh_float_t across = lh_cast_static(lh_float_t, radius + radius);
    if (lh_ptr_is_null(thumb) || radius <= 0 || lh_entity_knob_radius(thumb) == radius)
    {
        return;
    }
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self->thumb),
                          lh_math_vec2_make(across, across));
}

lh_float_t
lh_entity_knob_value(const lh_entity_knob_t *self)
{
    return lh_cast_static(lh_float_t, lh_entity_range_to_pos(lh_addr_of(self->range), 1000)) /
           1000.0f;
}

lh_void
lh_entity_knob_point(lh_float_t cx, lh_float_t cy, lh_float_t orbit, lh_float_t angle, lh_float_t *x,
                     lh_float_t *y)
{
    lh_ptr_deref(x) = cx + cosf(angle) * orbit;
    lh_ptr_deref(y) = cy + sinf(angle) * orbit;
}

lh_void
lh_entity_knob_seat(lh_entity_knob_t *self)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_entity_2d_t *const thumb = lh_ptr_rcast(const lh_entity_2d_t, self->thumb);
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    lh_float_t radius;
    lh_float_t orbit;
    lh_float_t x;
    lh_float_t y;
    lh_math_vec2_t place;
    lh_math_vec2_t have;
    lh_entity_knob_fit(self);
    if (lh_ptr_is_null(thumb))
    {
        return;
    }
    radius = lh_cast_static(lh_float_t, lh_entity_knob_radius(thumb));
    orbit = lh_cast_static(lh_float_t, lh_entity_knob_radius(box)) - radius * 4.0f;
    if (radius <= 0.0f || orbit < 0.0f)
    {
        return;
    }
    lh_entity_knob_point(lh_math_vec2_get_x(lh_addr_of(size)) * 0.5f,
                         lh_math_vec2_get_y(lh_addr_of(size)) * 0.5f, orbit,
                         lh_entity_knob_angle(lh_entity_knob_value(self)), lh_addr_of(x),
                         lh_addr_of(y));
    place = lh_math_vec2_make(x - radius, y - radius);
    have = lh_entity_2d_get_position(thumb);
    if (lh_math_vec2_get_x(lh_addr_of(have)) != lh_math_vec2_get_x(lh_addr_of(place)) ||
        lh_math_vec2_get_y(lh_addr_of(have)) != lh_math_vec2_get_y(lh_addr_of(place)))
    {
        lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, self->thumb), place);
    }
}

lh_void
lh_entity_knob_apply(lh_entity_knob_t *self, const lh_math_vec2_t *point)
{
    const lh_math_rect_t bounds =
        lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_float_t cx =
        lh_cast_static(lh_float_t, lh_math_rect_get_x(lh_addr_of(bounds))) +
        lh_cast_static(lh_float_t, lh_math_rect_get_size_width(lh_addr_of(bounds))) * 0.5f;
    const lh_float_t cy =
        lh_cast_static(lh_float_t, lh_math_rect_get_y(lh_addr_of(bounds))) +
        lh_cast_static(lh_float_t, lh_math_rect_get_size_height(lh_addr_of(bounds))) * 0.5f;
    const lh_float_t angle =
        atan2f(lh_math_vec2_get_y(point) - cy, lh_math_vec2_get_x(point) - cx);
    const lh_float_t turns = lh_entity_knob_turns(angle);
    const lh_int_t before = lh_entity_range_get_value(lh_addr_of(self->range));
    lh_entity_range_set_from_pos(lh_addr_of(self->range),
                                 lh_cast_static(lh_int_t, turns * 1000.0f), 1000);
    if (lh_entity_range_get_value(lh_addr_of(self->range)) != before)
    {
        lh_entity_send_event(lh_ptr_rcast(lh_entity_t, self), LH_ENTITY_EVENT_CLICKED, lh_null);
    }
    lh_entity_knob_seat(self);
}

/* The arc stops one pixel short of each end. The disc there is the handle's
   own radius, so the square cut stays under the round end. */
lh_void
lh_entity_knob_band(lh_ui_canvas_t *canvas, lh_int_t cx, lh_int_t cy, lh_int_t outer, lh_int_t inner,
                    lh_int_t radius, lh_float_t start, lh_float_t end, lh_ui_color_t color)
{
    const lh_float_t mid = lh_cast_static(lh_float_t, inner + radius);
    const lh_float_t margin = mid > 0.0f ? 1.0f / mid : 0.0f;
    const lh_float_t ox = lh_cast_static(lh_float_t, cx);
    const lh_float_t oy = lh_cast_static(lh_float_t, cy);
    lh_float_t x;
    lh_float_t y;
    if (end - margin > start + margin)
    {
        lh_ui_canvas_fill_arc(canvas, cx, cy, outer, inner, start + margin, end - margin, color);
    }
    lh_entity_knob_point(ox, oy, mid, start, lh_addr_of(x), lh_addr_of(y));
    lh_ui_canvas_fill_disc(canvas, x, y, radius, color);
    lh_entity_knob_point(ox, oy, mid, end, lh_addr_of(x), lh_addr_of(y));
    lh_ui_canvas_fill_disc(canvas, x, y, radius, color);
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
    const lh_int_t cx = left + width / 2;
    const lh_int_t cy = top + height / 2;
    const lh_float_t start = lh_entity_knob_angle(0.0f);
    const lh_int_t outer = lh_entity_range_cap(lh_addr_of(bounds));
    lh_math_rect_t thumb;
    lh_int_t radius;
    lh_int_t inner;
    lh_ui_color_t mark;
    lh_ui_color_t track;
    lh_float_t turns;
    if (lh_ptr_is_null(style) || lh_ptr_is_null(self->thumb))
    {
        return;
    }
    thumb = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self->thumb));
    radius = lh_entity_range_cap(lh_addr_of(thumb));
    inner = outer - radius - radius;
    if (radius <= 0 || inner <= 0)
    {
        return;
    }
    turns = lh_entity_knob_value(self);
    mark = lh_ui_style_get_text_color(style);
    track = lh_ui_style_get_bg_color(style);
    lh_entity_knob_band(canvas, cx, cy, outer, inner, radius, start, lh_entity_knob_angle(1.0f),
                        track);
    if (turns > 0.0f)
    {
        lh_entity_knob_band(canvas, cx, cy, outer, inner, radius, start,
                            lh_entity_knob_angle(turns), mark);
    }
}

lh_void
lh_entity_knob_construct(lh_entity_t *self)
{
    lh_entity_knob_t *const knob = lh_ptr_rcast(lh_entity_knob_t, self);
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    lh_entity_range_reset(lh_addr_of(knob->range));
    knob->thumb =
        lh_ptr_rcast(lh_entity_circle_t, lh_entity_create(lh_addr_of(lh_entity_circle_class), self));
    lh_entity_add_flags(lh_ptr_rcast(lh_entity_t, knob->thumb), lh_entity_flags_event_bubble);
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, knob->thumb), lh_addr_of(knob->handle));
}

lh_void
lh_entity_knob_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_knob_t *const knob = lh_ptr_rcast(lh_entity_knob_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    if (code == LH_ENTITY_EVENT_DRAW)
    {
        const lh_ui_style_t *const style =
            lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, knob));
        if (lh_ptr_is_set(style))
        {
            lh_ui_style_set_bg_color(lh_addr_of(knob->handle), lh_ui_style_get_text_color(style));
        }
        lh_entity_knob_seat(knob);
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
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_knob_t),
                                lh_entity_knob_construct, lh_null, lh_entity_knob_on_event);
