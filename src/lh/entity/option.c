#include <lh/entity/option.h>
#include <lh/assert.h>
#include <lh/entity/group.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_option_line(lh_ui_canvas_t *canvas, lh_int_t x0, lh_int_t y0, lh_int_t x1, lh_int_t y1,
                      lh_ui_color_t color)
{
    const lh_int_t sx = x0 < x1 ? 1 : -1;
    const lh_int_t sy = y0 < y1 ? 1 : -1;
    lh_int_t dx = x0 < x1 ? x1 - x0 : x0 - x1;
    lh_int_t dy = y0 < y1 ? y0 - y1 : y1 - y0;
    lh_int_t err = dx + dy;
    for (;;)
    {
        lh_int_t next;
        lh_ui_canvas_blend_pixel(canvas, x0, y0, color);
        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        next = err * 2;
        if (next >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (next <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

lh_void
lh_entity_option_check(lh_ui_canvas_t *canvas, lh_int_t x, lh_int_t y, lh_int_t width,
                       lh_int_t height, lh_ui_color_t color)
{
    const lh_int_t x0 = x + width / 4;
    const lh_int_t y0 = y + height / 2;
    const lh_int_t x1 = x + width / 2 - 1;
    const lh_int_t y1 = y + height - height / 4;
    const lh_int_t x2 = x + width - width / 4;
    const lh_int_t y2 = y + height / 4;
    lh_entity_option_line(canvas, x0, y0, x1, y1, color);
    lh_entity_option_line(canvas, x0, y0 + 1, x1, y1 + 1, color);
    lh_entity_option_line(canvas, x1, y1, x2, y2, color);
    lh_entity_option_line(canvas, x1, y1 + 1, x2, y2 + 1, color);
}

lh_void
lh_entity_option_quiet(lh_entity_option_t *self, lh_bool_t on)
{
    if (self->on == on)
    {
        return;
    }
    self->on = on;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_void
lh_entity_option_alone(lh_entity_option_t *self)
{
    lh_entity_t *const parent = lh_entity_get_parent(lh_ptr_rcast(lh_entity_t, self));
    const lh_entity_group_t *group;
    if (lh_ptr_is_null(parent))
    {
        return;
    }
    group = lh_entity_cast(parent, lh_addr_of(lh_entity_group_class));
    if (lh_ptr_is_null(group) || group->mode != LH_ENTITY_GROUP_ONE)
    {
        return;
    }
    lh_entity_foreach_child(child, parent)
    {
        lh_entity_option_t *const other =
            lh_entity_cast(child, lh_addr_of(lh_entity_check_class));
        lh_entity_option_t *const as_switch =
            lh_entity_cast(child, lh_addr_of(lh_entity_switch_class));
        lh_entity_option_t *const as_toggle =
            lh_entity_cast(child, lh_addr_of(lh_entity_toggle_class));
        lh_entity_option_t *const option =
            lh_ptr_is_set(other) ? other : (lh_ptr_is_set(as_switch) ? as_switch : as_toggle);
        if (lh_ptr_is_set(option) && option != self)
        {
            lh_entity_option_quiet(option, lh_bool_false);
        }
    }
}

lh_void
lh_entity_option_construct(lh_entity_t *self, lh_int_t kind)
{
    lh_entity_option_t *const option = lh_ptr_rcast(lh_entity_option_t, self);
    option->kind = kind;
    lh_entity_add_flags(self, lh_entity_flags_own_background);
}

lh_void
lh_entity_check_construct(lh_entity_t *self)
{
    lh_entity_option_construct(self, LH_ENTITY_OPTION_CHECK);
}

lh_void
lh_entity_switch_construct(lh_entity_t *self)
{
    lh_entity_option_construct(self, LH_ENTITY_OPTION_SWITCH);
}

lh_void
lh_entity_toggle_construct(lh_entity_t *self)
{
    lh_entity_option_construct(self, LH_ENTITY_OPTION_TOGGLE);
}

lh_void
lh_entity_option_paint(const lh_entity_option_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_style_t *const style =
        lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_math_rect_t bounds =
        lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_int_t x = lh_math_rect_get_x(lh_addr_of(bounds));
    const lh_int_t y = lh_math_rect_get_y(lh_addr_of(bounds));
    const lh_int_t width = lh_math_rect_get_size_width(lh_addr_of(bounds));
    const lh_int_t height = lh_math_rect_get_size_height(lh_addr_of(bounds));
    lh_ui_color_t mark;
    lh_ui_color_t fill;
    if (lh_ptr_is_null(style) || width <= 0 || height <= 0)
    {
        return;
    }
    mark = lh_ui_style_get_text_color(style);
    fill = lh_ui_style_get_bg_color(style);
    if (self->kind == LH_ENTITY_OPTION_TOGGLE)
    {
        lh_ui_canvas_fill_rect(canvas, bounds, fill);
        if (self->on && height > 3)
        {
            lh_ui_canvas_fill_rect(canvas, lh_math_rect_make(x, y + height - 3, width, 3), mark);
        }
        return;
    }
    if (self->kind == LH_ENTITY_OPTION_SWITCH)
    {
        const lh_int_t radius = height / 2;
        const lh_ui_color_t track = self->on ? mark : fill;
        const lh_ui_color_t thumb = self->on ? fill : mark;
        lh_int_t thumb_radius;
        if (radius < 1)
        {
            return;
        }
        lh_ui_canvas_fill_round(canvas, bounds, radius, track);
        thumb_radius = radius - 2;
        if (thumb_radius < 1)
        {
            thumb_radius = 1;
        }
        lh_ui_canvas_fill_disc(canvas, self->on ? x + width - radius : x + radius, y + radius,
                              thumb_radius, thumb);
        return;
    }
    lh_ui_canvas_fill_round(canvas, bounds, 4, mark);
    if (!self->on && width > 2 && height > 2)
    {
        lh_ui_canvas_fill_round(canvas, lh_math_rect_make(x + 1, y + 1, width - 2, height - 2), 3,
                                fill);
        return;
    }
    if (self->on)
    {
        lh_entity_option_check(canvas, x, y, width, height, fill);
    }
}

lh_void
lh_entity_option_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_option_t *const option = lh_ptr_rcast(lh_entity_option_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    if (code == LH_ENTITY_EVENT_DRAW)
    {
        lh_entity_option_paint(option, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code != LH_ENTITY_EVENT_POINTER_UP)
    {
        return;
    }
    {
        const lh_math_vec2_t *const point =
            lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
        if (lh_ptr_is_null(point) ||
            !lh_entity_2d_contains(lh_ptr_rcast(const lh_entity_2d_t, self), *point))
        {
            return;
        }
    }
    lh_entity_option_set_on(option, option->on ? lh_bool_false : lh_bool_true);
    lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED, lh_entity_event_get_param(event));
}

const lh_entity_class_t lh_entity_check_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_option_t),
                                lh_entity_check_construct, lh_null, lh_entity_option_on_event);

const lh_entity_class_t lh_entity_switch_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_option_t),
                                lh_entity_switch_construct, lh_null, lh_entity_option_on_event);

const lh_entity_class_t lh_entity_toggle_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_option_t),
                                lh_entity_toggle_construct, lh_null, lh_entity_option_on_event);

lh_bool_t
lh_entity_option_is_on(const lh_entity_option_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on;
}

lh_void
lh_entity_option_set_on(lh_entity_option_t *self, lh_bool_t on)
{
    lh_assert_runtime_ref(self);
    lh_entity_option_quiet(self, on);
    if (on)
    {
        lh_entity_option_alone(self);
    }
}
