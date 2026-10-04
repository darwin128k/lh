#include <lh/entity/circle.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/entity/screen.h>
#include <lh/float/round.h>
#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_byte_t
lh_entity_circle_coverage(lh_int_t x, lh_int_t y, lh_int_t cx, lh_int_t cy, lh_int_t radius)
{
    const lh_int_t limit = (radius * 16) * (radius * 16);
    lh_int_t hits;
    lh_int_t sx;
    lh_int_t sy;
    lh_int_t dx;
    lh_int_t dy;

    dx = x - cx;
    dy = y - cy;
    if (dx < 0)
    {
        dx = -dx;
    }
    if (dy < 0)
    {
        dy = -dy;
    }
    if (dx > radius + 1 || dy > radius + 1)
    {
        return 0;
    }

    hits = 0;
    for (sy = 0; sy < 8; ++sy)
    {
        const lh_int_t sample_y = (y * 16 + sy * 2 + 1) - cy * 16;
        for (sx = 0; sx < 8; ++sx)
        {
            const lh_int_t sample_x = (x * 16 + sx * 2 + 1) - cx * 16;
            if (sample_x * sample_x + sample_y * sample_y <= limit)
            {
                ++hits;
            }
        }
    }
    return lh_cast_static(lh_byte_t, (hits * 255) / 64);
}

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
    lh_int_t x;
    lh_int_t y;
    lh_uint_t argb;

    if (lh_ptr_is_null(style) || lh_ptr_is_null(canvas))
    {
        return;
    }
    color = lh_ui_style_get_bg_color(style);
    argb = lh_ui_color_to_argb(color);
    if ((argb >> 24) == 0U)
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
    for (y = top; y < top + height; ++y)
    {
        for (x = left; x < left + width; ++x)
        {
            const lh_byte_t coverage = lh_entity_circle_coverage(x, y, cx, cy, radius);
            const lh_uint_t alpha = ((argb >> 24) * coverage) / 255U;
            if (alpha == 0U)
            {
                continue;
            }
            lh_ui_canvas_blend_pixel(canvas, x, y,
                                     lh_ui_color_from_argb((argb & 0x00FFFFFFU) | (alpha << 24)));
        }
    }
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
