#include <lh/entity/slider.h>
#include <lh/entity.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

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
    lh_entity_range_reset(lh_addr_of(lh_ptr_rcast(lh_entity_slider_t, self)->range));
}

lh_void
lh_entity_slider_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_slider_t *const slider = lh_ptr_rcast(lh_entity_slider_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    if (code == LH_ENTITY_EVENT_DRAW)
    {
        lh_entity_range_paint(lh_addr_of(slider->range),
                              lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
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
