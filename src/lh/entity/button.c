#include <lh/entity/button.h>
#include <lh/assert.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_button_paint(const lh_entity_button_t *self, lh_ui_canvas_t *canvas)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_ui_style_t *const style = lh_entity_2d_get_style(box);
    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(box);
    const lh_math_vec4_t origin = lh_math_mat4_get_column(lh_addr_of(world), 3);
    lh_ui_color_t color;
    if (self->radius <= 0 || lh_ptr_is_null(style))
    {
        return;
    }
    color = lh_ui_style_get_bg_color(style);
    if (color.a == 0U)
    {
        return;
    }
    lh_ui_canvas_set_draw_z(canvas, lh_math_vec4_get_z(lh_addr_of(origin)));
    lh_ui_canvas_fill_round(canvas, lh_entity_2d_get_screen_bounds(box), self->radius, color);
}

lh_void
lh_entity_button_show_pressed(lh_entity_button_t *self, lh_bool_t down)
{
    lh_entity_2d_t *const box = lh_ptr_rcast(lh_entity_2d_t, self);
    if (down)
    {
        if (lh_ptr_is_set(self->pressed_style) && lh_ptr_is_null(self->rest_style))
        {
            self->rest_style = lh_entity_2d_get_style(box);
            lh_entity_2d_set_style(box, self->pressed_style);
        }
        return;
    }
    if (lh_ptr_is_set(self->rest_style))
    {
        const lh_ui_style_t *const rest = self->rest_style;
        self->rest_style = lh_null;
        lh_entity_2d_set_style(box, rest);
    }
}

lh_void
lh_entity_button_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_button_t *const button = lh_ptr_rcast(lh_entity_button_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    lh_entity_screen_t *screen;

    if (code == LH_ENTITY_EVENT_DRAW && button->radius > 0)
    {
        lh_entity_button_paint(button,
                               lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_DELETE)
    {
        screen = lh_entity_cast(lh_entity_get_root(self), lh_addr_of(lh_entity_screen_class));
        if (lh_ptr_is_set(screen))
        {
            lh_entity_screen_clear_pressed(screen, self);
        }
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        button->pressed = lh_bool_true;
        lh_entity_button_show_pressed(button, lh_bool_true);
        if (button->repeat)
        {
            lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED, lh_entity_event_get_param(event));
        }
        return;
    }
    if (code == LH_ENTITY_EVENT_TICK && button->repeat && button->pressed)
    {
        lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED, lh_null);
        return;
    }
    if ((code == LH_ENTITY_EVENT_POINTER_UP || code == LH_ENTITY_EVENT_POINTER_CANCEL) &&
        button->pressed)
    {
        const lh_math_vec2_t *const point =
            lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
        const lh_bool_t inside =
            lh_ptr_is_set(point) &&
            lh_entity_2d_contains(lh_ptr_rcast(const lh_entity_2d_t, self), *point);
        button->pressed = lh_bool_false;
        lh_entity_button_show_pressed(button, lh_bool_false);
        /* A cancel arrives with no point, and the check above reads that as not
           inside: a press that was taken away rather than let go is not a
           click. The whole of a lost hold to a button, and it falls out of the
           rule instead of being a second one. */
        if (inside && !button->repeat)
        {
            lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED, lh_entity_event_get_param(event));
        }
    }
}

const lh_entity_class_t lh_entity_button_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_button_t), lh_null, lh_null,
                                lh_entity_button_on_event);

lh_bool_t
lh_entity_button_is_pressed(const lh_entity_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

const lh_ui_style_t *
lh_entity_button_get_pressed_style(const lh_entity_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed_style;
}

lh_void
lh_entity_button_set_pressed_style(lh_entity_button_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->pressed_style = style;
}

lh_bool_t
lh_entity_button_get_repeat(const lh_entity_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->repeat;
}

lh_void
lh_entity_button_set_repeat(lh_entity_button_t *self, lh_bool_t repeat)
{
    lh_assert_runtime_ref(self);
    self->repeat = repeat;
}

lh_int_t
lh_entity_button_get_radius(const lh_entity_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_entity_button_set_radius(lh_entity_button_t *self, lh_int_t radius)
{
    lh_entity_t *const entity = lh_ptr_rcast(lh_entity_t, self);
    lh_assert_runtime_ref(self);
    if (radius < 0)
    {
        radius = 0;
    }
    if (self->radius == radius)
    {
        return;
    }
    self->radius = radius;
    if (radius > 0)
    {
        lh_entity_add_flags(entity, lh_entity_flags_own_background);
    }
    else
    {
        lh_entity_clear_flags(entity, lh_entity_flags_own_background);
    }
    lh_entity_invalidate(entity);
}
