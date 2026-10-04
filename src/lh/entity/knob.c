#include <lh/entity/knob.h>
#include <lh/entity.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#include <math.h>

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
    /* Screen y grows downward. Sweep from the lower left, through the top,
       to the lower right: about 0.75 of a turn. */
    lh_float_t turns = (angle + 3.14159265f) / (2.0f * 3.14159265f);
    const lh_int_t before = lh_entity_range_get_value(lh_addr_of(self->range));
    if (turns < 0.0f)
    {
        turns = 0.0f;
    }
    if (turns > 1.0f)
    {
        turns = 1.0f;
    }
    lh_entity_range_set_from_pos(lh_addr_of(self->range), (lh_int_t)(turns * 1000.0f), 1000);
    if (lh_entity_range_get_value(lh_addr_of(self->range)) != before)
    {
        lh_entity_send_event(lh_ptr_rcast(lh_entity_t, self), LH_ENTITY_EVENT_CLICKED, lh_null);
    }
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
        radius <= 0 ? 0.0f : (lh_float_t)lh_entity_range_to_pos(lh_addr_of(self->range), 1000) / 1000.0f;
    const lh_float_t angle = (0.75f + turns * 1.5f) * 3.14159265f;
    lh_ui_color_t needle;
    if (lh_ptr_is_null(style) || radius <= 0)
    {
        return;
    }
    needle = lh_ui_style_get_text_color(style);
    lh_ui_canvas_fill_disc(canvas, cx, cy, radius, lh_ui_style_get_bg_color(style));
    {
        lh_int_t step;
        for (step = radius / 5; step < radius; ++step)
        {
            const lh_int_t x = cx + (lh_int_t)(cosf(angle) * (lh_float_t)step);
            const lh_int_t y_at = cy + (lh_int_t)(sinf(angle) * (lh_float_t)step);
            lh_ui_canvas_blend_pixel(canvas, x, y_at, needle);
        }
    }
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
