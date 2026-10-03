#include <lh/entity/rect.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/entity/screen.h>
#include <lh/float/round.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/util/addr.h>
#include <lh/math.h>
#include <lh/util/ptr.h>

#define lh_entity_rect_as_entity(self) lh_ptr_rcast(lh_entity_t, (self))
#define lh_entity_rect_as_const_2d(self) lh_ptr_rcast(const lh_entity_2d_t, (self))

/* True when @p local, a point in the rectangle's own space, is inside its
 * `[0, width) x [0, height)`. */
static lh_bool_t
lh_entity_rect_has_local_point(lh_math_vec2_t size, lh_math_vec3_t local)
{
    return local.x >= 0.0f && local.x < size.x && local.y >= 0.0f && local.y < size.y;
}

/* Fill @p self's area on @p canvas with its color. A pixel belongs to the
 * rectangle when its center does, so neighbors share no pixel and leave no
 * gap. */
static lh_void
lh_entity_rect_draw(const lh_entity_rect_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_color_t color = lh_entity_rect_get_color(self);
    const lh_math_vec2_t size = lh_entity_rect_get_size(self);
    if (color.a == 0U || size.x <= 0.0f || size.y <= 0.0f)
    {
        return;
    }

    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(lh_entity_rect_as_const_2d(self));
    if (world.columns[0].y == 0.0f && world.columns[1].x == 0.0f)
    {
        /* Not rotated: the covered pixels form a rectangle themselves. */
        const lh_math_vec3_t a = lh_math_mat4_transform_point(world, lh_math_vec3_make(0.0f, 0.0f, 0.0f));
        const lh_math_vec3_t b = lh_math_mat4_transform_point(world, lh_math_vec3_make(size.x, size.y, 0.0f));
        const lh_int_t x0 = lh_float_ceil_to_int(lh_math_min(a.x, b.x) - 0.5f);
        const lh_int_t y0 = lh_float_ceil_to_int(lh_math_min(a.y, b.y) - 0.5f);
        const lh_int_t x1 = lh_float_ceil_to_int(lh_math_max(a.x, b.x) - 0.5f);
        const lh_int_t y1 = lh_float_ceil_to_int(lh_math_max(a.y, b.y) - 0.5f);
        lh_ui_canvas_fill_rect(canvas, lh_math_rect_make(x0, y0, x1 - x0, y1 - y0), color);
        return;
    }

    /* Rotated: test each pixel center of the bounds in the rectangle's space. */
    lh_math_mat4_t to_local;
    if (!lh_math_mat4_inverse(world, lh_addr_of(to_local)))
    {
        return;
    }
    const lh_math_rect_t clip = lh_ui_canvas_get_clip(canvas);
    const lh_math_rect_t bounds = lh_entity_rect_get_screen_bounds(self);
    const lh_math_rect_t area = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(bounds));
    for (lh_int_t y = area.origin.y; y < area.origin.y + area.size.height; ++y)
    {
        for (lh_int_t x = area.origin.x; x < area.origin.x + area.size.width; ++x)
        {
            const lh_math_vec3_t center = lh_math_vec3_make(lh_cast_static(lh_float_t, x) + 0.5f,
                                                  lh_cast_static(lh_float_t, y) + 0.5f, 0.0f);
            const lh_math_vec3_t local = lh_math_mat4_transform_point(to_local, center);
            if (lh_entity_rect_has_local_point(size, local))
            {
                lh_ui_canvas_blend_pixel(canvas, x, y, color);
            }
        }
    }
}

static lh_void
lh_entity_rect_event(lh_entity_t *self, lh_entity_event_t *event)
{
    switch (lh_entity_event_get_code(event))
    {
    case LH_ENTITY_EVENT_DRAW:
        lh_entity_rect_draw(lh_ptr_rcast(const lh_entity_rect_t, self),
                            lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
        break;
    case LH_ENTITY_EVENT_DELETE:
        lh_entity_invalidate(self); /* the area it covered shows what is under it now */
        break;
    default:
        break;
    }
}

const lh_entity_class_t lh_entity_rect_class = lh_entity_class_initializer(
    &lh_entity_2d_class, sizeof(lh_entity_rect_t), lh_null, lh_null, lh_entity_rect_event);

lh_math_vec2_t
lh_entity_rect_get_size(const lh_entity_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_void
lh_entity_rect_set_size(lh_entity_rect_t *self, lh_math_vec2_t size)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_entity_rect_as_entity(self));
    self->size = size;
    lh_entity_invalidate(lh_entity_rect_as_entity(self));
}

lh_ui_color_t
lh_entity_rect_get_color(const lh_entity_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->color;
}

lh_void
lh_entity_rect_set_color(lh_entity_rect_t *self, lh_ui_color_t color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
    lh_entity_invalidate(lh_entity_rect_as_entity(self)); /* same area, new pixels */
}

lh_math_rect_t
lh_entity_rect_get_screen_bounds(const lh_entity_rect_t *self)
{
    const lh_math_vec2_t size = lh_entity_rect_get_size(self);
    if (size.x <= 0.0f || size.y <= 0.0f)
    {
        return lh_math_rect_zero();
    }

    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(lh_entity_rect_as_const_2d(self));
    const lh_math_vec3_t corners[4] = {
        lh_math_mat4_transform_point(world, lh_math_vec3_make(0.0f, 0.0f, 0.0f)),
        lh_math_mat4_transform_point(world, lh_math_vec3_make(size.x, 0.0f, 0.0f)),
        lh_math_mat4_transform_point(world, lh_math_vec3_make(0.0f, size.y, 0.0f)),
        lh_math_mat4_transform_point(world, lh_math_vec3_make(size.x, size.y, 0.0f)),
    };
    lh_math_vec3_t min = corners[0];
    lh_math_vec3_t max = corners[0];
    for (lh_int_t i = 1; i < 4; ++i)
    {
        min.x = lh_math_min(min.x, corners[i].x);
        min.y = lh_math_min(min.y, corners[i].y);
        max.x = lh_math_max(max.x, corners[i].x);
        max.y = lh_math_max(max.y, corners[i].y);
    }
    return lh_math_rect_from_min_max(lh_float_floor_to_int(min.x), lh_float_floor_to_int(min.y),
                                   lh_float_ceil_to_int(max.x), lh_float_ceil_to_int(max.y));
}

lh_bool_t
lh_entity_rect_contains(const lh_entity_rect_t *self, lh_math_vec2_t point)
{
    lh_math_mat4_t to_local;
    if (!lh_math_mat4_inverse(lh_entity_2d_get_world_matrix(lh_entity_rect_as_const_2d(self)),
                         lh_addr_of(to_local)))
    {
        return lh_bool_false; /* scaled to nothing: covers no area */
    }

    const lh_math_vec3_t local = lh_math_mat4_transform_point(to_local, lh_math_vec3_make(point.x, point.y, 0.0f));
    return lh_entity_rect_has_local_point(lh_entity_rect_get_size(self), local);
}

lh_entity_t *
lh_entity_rect_find_at(lh_entity_t *root, lh_math_vec2_t point)
{
    if (lh_entity_has_flags(root, lh_entity_flags_hidden))
    {
        return lh_null;
    }

    const lh_entity_rect_t *const rect = lh_entity_cast(root, lh_addr_of(lh_entity_rect_class));
    const lh_bool_t inside = lh_ptr_is_set(rect) && lh_entity_rect_contains(rect, point);

    /* Children first, the youngest hit winning; the entity itself only when
     * no child is hit: that is the drawing order read backwards. A rectangle
     * that cuts its children hides those outside it. */
    lh_entity_t *hit = lh_null;
    if (lh_ptr_is_null(rect) || inside ||
        lh_entity_has_flags(root, lh_entity_flags_overflow_visible))
    {
        lh_entity_foreach_child(child, root)
        {
            lh_entity_t *const child_hit = lh_entity_rect_find_at(child, point);
            if (lh_ptr_is_set(child_hit))
            {
                hit = child_hit;
            }
        }
    }
    if (lh_ptr_is_set(hit))
    {
        return hit;
    }
    return inside ? root : lh_null;
}
