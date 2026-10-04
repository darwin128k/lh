#include <lh/entity/circle.h>
#include <lh/assert.h>
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
    lh_math_vec4_t origin;
    lh_math_rect_t bounds;
    lh_int_t left;
    lh_int_t top;
    lh_int_t width;
    lh_int_t height;
    lh_int_t radius;
    lh_int_t cx;
    lh_int_t cy;

    if (lh_ptr_is_null(style) || lh_ptr_is_null(canvas))
    {
        return;
    }
    color = lh_ui_style_get_bg_color(style);
    if (color.a == 0U)
    {
        return;
    }

    world = lh_entity_2d_get_world_matrix(lh_ptr_rcast(const lh_entity_2d_t, self));
    origin = lh_math_mat4_get_column(lh_addr_of(world), 3);
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    left = lh_math_rect_get_x(lh_addr_of(bounds));
    top = lh_math_rect_get_y(lh_addr_of(bounds));
    width = lh_math_rect_get_size_width(lh_addr_of(bounds));
    height = lh_math_rect_get_size_height(lh_addr_of(bounds));
    radius = width < height ? width : height;
    radius /= 2;
    if (radius <= 0)
    {
        return;
    }
    cx = left + width / 2;
    cy = top + height / 2;
    lh_ui_canvas_set_draw_z(canvas, lh_math_vec4_get_z(lh_addr_of(origin)));
    lh_ui_canvas_fill_disc(canvas, cx, cy, radius, color);
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
    if (code == LH_ENTITY_EVENT_POINTER_UP && circle->pressed)
    {
        const lh_math_vec2_t *const point =
            lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
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
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_circle_t),
                                lh_entity_circle_construct, lh_null, lh_entity_circle_on_event);

lh_bool_t
lh_entity_circle_is_pressed(const lh_entity_circle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}
