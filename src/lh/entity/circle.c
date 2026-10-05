#include <lh/entity/circle.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_circle_paint(lh_entity_circle_t *self, lh_ui_canvas_t *canvas)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_ui_style_t *const style = lh_entity_2d_get_style(box);
    lh_ui_color_t color;
    lh_math_mat4_t world;
    lh_math_vec4_t column;
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    const lh_float_t width = lh_math_vec2_get_x(lh_addr_of(size));
    const lh_float_t height = lh_math_vec2_get_y(lh_addr_of(size));
    const lh_int_t across = lh_cast_static(lh_int_t, width < height ? width : height);
    const lh_int_t radius = across / 2;

    if (lh_ptr_is_null(style) || lh_ptr_is_null(canvas) || radius <= 0)
    {
        return;
    }
    color = lh_ui_style_get_bg_color(style);
    if (color.a == 0U)
    {
        return;
    }

    world = lh_entity_2d_get_world_matrix(lh_ptr_rcast(const lh_entity_2d_t, self));
    column = lh_math_mat4_get_column(lh_addr_of(world), 3);
    lh_ui_canvas_set_draw_z(canvas, lh_math_vec4_get_z(lh_addr_of(column)));
    lh_ui_canvas_fill_disc(canvas, lh_math_vec4_get_x(lh_addr_of(column)) + width * 0.5f,
                           lh_math_vec4_get_y(lh_addr_of(column)) + height * 0.5f, radius, color);
}

lh_void
lh_entity_circle_construct(lh_entity_t *self)
{
    lh_entity_add_flags(self, lh_entity_flags_own_background);
}

lh_void
lh_entity_circle_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_circle_t *const circle = lh_ptr_rcast(lh_entity_circle_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);

    if (code == LH_ENTITY_EVENT_DRAW)
    {
        lh_entity_circle_paint(circle,
                               lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_DELETE)
    {
        lh_entity_screen_t *const screen =
            lh_entity_cast(lh_entity_get_root(self), lh_addr_of(lh_entity_screen_class));
        if (lh_ptr_is_set(screen))
        {
            lh_entity_screen_clear_pressed(screen, self);
        }
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        circle->pressed = lh_bool_true;
        return;
    }
    if ((code == LH_ENTITY_EVENT_POINTER_UP || code == LH_ENTITY_EVENT_POINTER_CANCEL) &&
        circle->pressed)
    {
        const lh_math_vec2_t *const point =
            lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
        /* A cancel has no point, which reads as not inside, so a press that was
           taken away is let go without becoming a click. */
        const lh_bool_t inside =
            lh_ptr_is_set(point) &&
            lh_entity_2d_contains(lh_ptr_rcast(const lh_entity_2d_t, self), *point);
        circle->pressed = lh_bool_false;
        if (inside)
        {
            lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED, lh_entity_event_get_param(event));
        }
    }
}

const lh_entity_class_t lh_entity_circle_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_circle_t),
                                lh_entity_circle_construct, lh_null, lh_entity_circle_on_event);

lh_bool_t
lh_entity_circle_is_pressed(const lh_entity_circle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}
