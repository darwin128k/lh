#include <lh/entity/scroll.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_scroll_paint(const lh_entity_scroll_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_style_t *const style =
        lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_math_rect_t bounds;
    lh_int_t length;
    lh_int_t thumb;
    lh_int_t travel;
    lh_int_t origin;
    lh_int_t vertical;
    lh_ui_color_t color;
    if (lh_ptr_is_null(style))
    {
        return;
    }
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    vertical = lh_math_rect_get_size_height(lh_addr_of(bounds)) >
               lh_math_rect_get_size_width(lh_addr_of(bounds));
    length = vertical != 0 ? lh_math_rect_get_size_height(lh_addr_of(bounds))
                           : lh_math_rect_get_size_width(lh_addr_of(bounds));
    lh_ui_canvas_fill_rect(canvas, bounds, lh_ui_style_get_bg_color(style));
    thumb = length / 4;
    if (self->page > 0 && self->range.maximum > self->range.minimum)
    {
        const lh_int_t whole = self->range.maximum - self->range.minimum + self->page;
        thumb = (lh_int_t)(((lh_sllong_t)self->page * (lh_sllong_t)length) / (lh_sllong_t)whole);
    }
    if (thumb < 8)
    {
        thumb = 8;
    }
    if (thumb > length)
    {
        thumb = length;
    }
    travel = length - thumb;
    origin = lh_entity_range_to_pos(lh_addr_of(self->range), travel);
    if (vertical != 0)
    {
        bounds = lh_math_rect_make(lh_math_rect_get_x(lh_addr_of(bounds)),
                                   lh_math_rect_get_y(lh_addr_of(bounds)) + origin,
                                   lh_math_rect_get_size_width(lh_addr_of(bounds)), thumb);
    }
    else
    {
        bounds = lh_math_rect_make(lh_math_rect_get_x(lh_addr_of(bounds)) + origin,
                                   lh_math_rect_get_y(lh_addr_of(bounds)), thumb,
                                   lh_math_rect_get_size_height(lh_addr_of(bounds)));
    }
    color = lh_ptr_is_set(self->thumb) ? lh_ui_style_get_bg_color(self->thumb)
                                       : lh_ui_style_get_text_color(style);
    lh_ui_canvas_fill_rect(canvas, bounds, color);
}

lh_void
lh_entity_scroll_apply(lh_entity_scroll_t *self, const lh_math_vec2_t *point)
{
    const lh_math_rect_t bounds =
        lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_math_vec2_t size = lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self));
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
lh_entity_scroll_construct(lh_entity_t *self)
{
    lh_entity_scroll_t *const scroll = lh_ptr_rcast(lh_entity_scroll_t, self);
    lh_entity_range_reset(lh_addr_of(scroll->range));
    scroll->page = 10;
}

lh_void
lh_entity_scroll_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_scroll_t *const scroll = lh_ptr_rcast(lh_entity_scroll_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    if (code == LH_ENTITY_EVENT_DRAW)
    {
        lh_entity_scroll_paint(scroll, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        scroll->dragging = lh_bool_true;
        lh_entity_scroll_apply(scroll, lh_ptr_rcast(const lh_math_vec2_t,
                                                    lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_MOVE && scroll->dragging)
    {
        lh_entity_scroll_apply(scroll, lh_ptr_rcast(const lh_math_vec2_t,
                                                    lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_UP)
    {
        scroll->dragging = lh_bool_false;
    }
}

const lh_entity_class_t lh_entity_scroll_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_scroll_t),
                                lh_entity_scroll_construct, lh_null, lh_entity_scroll_on_event);

lh_int_t
lh_entity_scroll_get_page(const lh_entity_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->page;
}

lh_void
lh_entity_scroll_set_page(lh_entity_scroll_t *self, lh_int_t page)
{
    lh_assert_runtime_ref(self);
    self->page = page < 1 ? 1 : page;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

const lh_ui_style_t *
lh_entity_scroll_get_thumb(const lh_entity_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thumb;
}

lh_void
lh_entity_scroll_set_thumb(lh_entity_scroll_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->thumb = style;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}
