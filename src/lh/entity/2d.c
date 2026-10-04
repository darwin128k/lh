#include <lh/entity/2d.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/entity/screen.h>
#include <lh/float/round.h>
#include <lh/null.h>
#include <lh/math.h>
#include <lh/math/quat.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#define lh_entity_2d_as_entity(self) lh_ptr_rcast(lh_entity_t, (self))

/* True when @p self has a positive width and height: a box that is painted,
 * hit and (unless it overflows) cuts its children. */
static lh_bool_t
lh_entity_2d_has_box(const lh_entity_2d_t *self)
{
    const lh_math_vec2_t size = lh_entity_2d_get_size(self);
    return lh_math_vec2_get_x(lh_addr_of(size)) > 0.0f && lh_math_vec2_get_y(lh_addr_of(size)) > 0.0f;
}

/* True when @p local, a point in the entity's own space, is inside its
 * `[0, width) x [0, height)`. */
static lh_bool_t
lh_entity_2d_has_local_point(lh_math_vec2_t size, lh_math_vec3_t local)
{
    return lh_math_vec3_get_x(lh_addr_of(local)) >= 0.0f &&
           lh_math_vec3_get_x(lh_addr_of(local)) < lh_math_vec2_get_x(lh_addr_of(size)) &&
           lh_math_vec3_get_y(lh_addr_of(local)) >= 0.0f &&
           lh_math_vec3_get_y(lh_addr_of(local)) < lh_math_vec2_get_y(lh_addr_of(size));
}

static lh_void
lh_entity_2d_construct(lh_entity_t *self)
{
    /* The memory is zeroed: position, angle, size and the style pointer
     * (none) are already 0. */
    lh_entity_2d_set_scale(lh_ptr_rcast(lh_entity_2d_t, self), lh_math_vec2_make(1.0f, 1.0f));
}

static lh_void
lh_entity_2d_event(lh_entity_t *self, lh_entity_event_t *event)
{
    switch (lh_entity_event_get_code(event))
    {
    case LH_ENTITY_EVENT_GET_LOCAL_MATRIX: {
        const lh_entity_2d_t *const entity = lh_ptr_rcast(const lh_entity_2d_t, self);
        const lh_math_vec2_t position = lh_entity_2d_get_position(entity);
        const lh_math_vec2_t scale = lh_entity_2d_get_scale(entity);
        const lh_math_mat4_t rotate = lh_math_mat4_from_quat(lh_math_quat_from_axis_angle(
            lh_math_vec3_make(0.0f, 0.0f, 1.0f), lh_entity_2d_get_angle(entity)));

        *lh_ptr_rcast(lh_math_mat4_t, lh_entity_event_get_param(event)) = lh_math_mat4_mul(
            lh_math_mat4_from_translation(lh_math_vec2_to_vec3_z0(position)),
            lh_math_mat4_mul(rotate, lh_math_mat4_from_scale(lh_math_vec2_to_vec3(scale, 1.0f))));
        lh_entity_event_stop(event);
        break;
    }
    case LH_ENTITY_EVENT_DELETE:
        /* The area it covered shows what is under it now. */
        lh_entity_invalidate(self);
        break;
    default:
        break;
    }
}

const lh_entity_class_t lh_entity_2d_class =
    lh_entity_class_initializer(&lh_entity_base_class, sizeof(lh_entity_2d_t), lh_entity_2d_construct,
                                lh_null, lh_entity_2d_event);

lh_math_vec2_t
lh_entity_2d_get_position(const lh_entity_2d_t *self)
{
    lh_assert_runtime_ref(self);
    return self->position;
}

lh_void
lh_entity_2d_set_position(lh_entity_2d_t *self, lh_math_vec2_t position)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_entity_2d_as_entity(self)); /* where it was */
    self->position = position;
    lh_entity_invalidate(lh_entity_2d_as_entity(self)); /* where it is now */
}

lh_float_t
lh_entity_2d_get_angle(const lh_entity_2d_t *self)
{
    lh_assert_runtime_ref(self);
    return self->angle;
}

lh_void
lh_entity_2d_set_angle(lh_entity_2d_t *self, lh_float_t angle)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_entity_2d_as_entity(self)); /* where it was */
    self->angle = angle;
    lh_entity_invalidate(lh_entity_2d_as_entity(self)); /* where it is now */
}

lh_math_vec2_t
lh_entity_2d_get_scale(const lh_entity_2d_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scale;
}

lh_void
lh_entity_2d_set_scale(lh_entity_2d_t *self, lh_math_vec2_t scale)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_entity_2d_as_entity(self)); /* where it was */
    self->scale = scale;
    lh_entity_invalidate(lh_entity_2d_as_entity(self)); /* where it is now */
}

lh_math_vec2_t
lh_entity_2d_get_size(const lh_entity_2d_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_void
lh_entity_2d_set_size(lh_entity_2d_t *self, lh_math_vec2_t size)
{
    lh_assert_runtime_ref(self);
    lh_entity_invalidate(lh_entity_2d_as_entity(self));
    self->size = size;
    lh_entity_invalidate(lh_entity_2d_as_entity(self));
}

const lh_ui_style_t *
lh_entity_2d_get_style(const lh_entity_2d_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_entity_2d_set_style(lh_entity_2d_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->style = style;
    lh_entity_invalidate(lh_entity_2d_as_entity(self)); /* same area, new pixels */
}

lh_math_mat4_t
lh_entity_2d_get_local_matrix(const lh_entity_2d_t *self)
{
    /* Asked as an event so the most derived spatial class answers. */
    lh_math_mat4_t local = lh_math_mat4_identity();
    lh_entity_notify(lh_cast_const(lh_entity_t *, lh_ptr_rcast(const lh_entity_t, self)),
                     LH_ENTITY_EVENT_GET_LOCAL_MATRIX, lh_addr_of(local));
    return local;
}

lh_math_mat4_t
lh_entity_2d_get_world_matrix(const lh_entity_2d_t *self)
{
    lh_math_mat4_t world = lh_entity_2d_get_local_matrix(self);
    for (const lh_entity_t *ancestor = lh_entity_get_parent(lh_ptr_rcast(const lh_entity_t, self));
         lh_ptr_is_set(ancestor); ancestor = lh_entity_get_parent(ancestor))
    {
        const lh_entity_2d_t *const spatial = lh_entity_cast(ancestor, lh_addr_of(lh_entity_2d_class));
        if (lh_ptr_is_set(spatial))
        {
            world = lh_math_mat4_mul(lh_entity_2d_get_local_matrix(spatial), world);
        }
    }
    return world;
}

lh_math_rect_t
lh_entity_2d_get_screen_bounds(const lh_entity_2d_t *self)
{
    const lh_math_vec2_t size = lh_entity_2d_get_size(self);
    if (!lh_entity_2d_has_box(self))
    {
        return lh_math_rect_make_empty();
    }

    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(self);
    const lh_float_t w = lh_math_vec2_get_x(lh_addr_of(size));
    const lh_float_t h = lh_math_vec2_get_y(lh_addr_of(size));
    const lh_math_vec3_t corners[4] = {
        lh_math_mat4_transform_point(world, lh_math_vec3_make(0.0f, 0.0f, 0.0f)),
        lh_math_mat4_transform_point(world, lh_math_vec3_make(w, 0.0f, 0.0f)),
        lh_math_mat4_transform_point(world, lh_math_vec3_make(0.0f, h, 0.0f)),
        lh_math_mat4_transform_point(world, lh_math_vec3_make(w, h, 0.0f)),
    };
    lh_math_vec3_t min = corners[0];
    lh_math_vec3_t max = corners[0];
    for (lh_int_t i = 1; i < 4; ++i)
    {
        lh_math_vec3_set_x(&min, lh_math_min(lh_math_vec3_get_x(lh_addr_of(min)),
                                             lh_math_vec3_get_x(lh_addr_of(corners)[i])));
        lh_math_vec3_set_y(&min, lh_math_min(lh_math_vec3_get_y(lh_addr_of(min)),
                                             lh_math_vec3_get_y(lh_addr_of(corners)[i])));
        lh_math_vec3_set_x(&max, lh_math_max(lh_math_vec3_get_x(lh_addr_of(max)),
                                             lh_math_vec3_get_x(lh_addr_of(corners)[i])));
        lh_math_vec3_set_y(&max, lh_math_max(lh_math_vec3_get_y(lh_addr_of(max)),
                                             lh_math_vec3_get_y(lh_addr_of(corners)[i])));
    }
    return lh_math_rect_from_min_max(lh_float_floor_to_int(lh_math_vec3_get_x(lh_addr_of(min))),
                                     lh_float_floor_to_int(lh_math_vec3_get_y(lh_addr_of(min))),
                                     lh_float_ceil_to_int(lh_math_vec3_get_x(lh_addr_of(max))),
                                     lh_float_ceil_to_int(lh_math_vec3_get_y(lh_addr_of(max))));
}

lh_bool_t
lh_entity_2d_contains(const lh_entity_2d_t *self, lh_math_vec2_t point)
{
    lh_assert_runtime_ref(self);
    if (!lh_entity_2d_has_box(self))
    {
        return lh_bool_false;
    }

    lh_math_mat4_t to_local;
    if (!lh_math_mat4_inverse(lh_entity_2d_get_world_matrix(self), lh_addr_of(to_local)))
    {
        return lh_bool_false; /* scaled to nothing: covers no area */
    }

    const lh_math_vec3_t local =
        lh_math_mat4_transform_point(to_local, lh_math_vec2_to_vec3_z0(point));
    return lh_entity_2d_has_local_point(lh_entity_2d_get_size(self), local);
}

lh_entity_t *
lh_entity_2d_find_at(lh_entity_t *root, lh_math_vec2_t point)
{
    if (lh_entity_has_flags(root, lh_entity_flags_hidden))
    {
        return lh_null;
    }

    const lh_entity_2d_t *const entity = lh_entity_cast(root, lh_addr_of(lh_entity_2d_class));
    const lh_bool_t has_box = lh_ptr_is_set(entity) && lh_entity_2d_has_box(entity);
    const lh_bool_t inside = has_box && lh_entity_2d_contains(entity, point);

    /* Children first, the youngest hit winning; the entity itself only when
     * no child is hit: that is the drawing order read backwards. A box that
     * cuts its children hides those outside it. No box is not a hit and does
     * not hide children. */
    lh_entity_t *hit = lh_null;
    if (!has_box || inside || lh_entity_has_flags(root, lh_entity_flags_overflow_visible))
    {
        lh_entity_foreach_child(child, root)
        {
            lh_entity_t *const child_hit = lh_entity_2d_find_at(child, point);
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

lh_void
lh_entity_2d_draw_background(const lh_entity_2d_t *self, lh_ui_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(canvas);

    const lh_ui_style_t *const style = lh_entity_2d_get_style(self);
    /* No style, transparent, or no box: the fill has nothing to do. Same as
     * LVGL skipping a background whose opacity is zero. */
    if (lh_ptr_is_null(style) || !lh_entity_2d_has_box(self))
    {
        return;
    }
    const lh_ui_color_t color = lh_ui_style_get_bg_color(style);
    const lh_math_vec2_t size = lh_entity_2d_get_size(self);
    if (color.a == 0U)
    {
        return;
    }

    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(self);
    const lh_math_vec4_t world_col_0 = lh_math_mat4_get_column(lh_addr_of(world), 0);
    const lh_math_vec4_t world_col_1 = lh_math_mat4_get_column(lh_addr_of(world), 1);
    if (lh_math_vec4_get_y(lh_addr_of(world_col_0)) == 0.0f &&
        lh_math_vec4_get_x(lh_addr_of(world_col_1)) == 0.0f)
    {
        /* Not rotated: the covered pixels are themselves a rectangle, so
         * this is one clipped fill, not a test per pixel. */
        const lh_math_vec3_t a =
            lh_math_mat4_transform_point(world, lh_math_vec3_make(0.0f, 0.0f, 0.0f));
        const lh_math_vec3_t b = lh_math_mat4_transform_point(world, lh_math_vec2_to_vec3_z0(size));
        const lh_int_t x0 = lh_float_ceil_to_int(
            lh_math_min(lh_math_vec3_get_x(lh_addr_of(a)), lh_math_vec3_get_x(lh_addr_of(b))) - 0.5f);
        const lh_int_t y0 = lh_float_ceil_to_int(
            lh_math_min(lh_math_vec3_get_y(lh_addr_of(a)), lh_math_vec3_get_y(lh_addr_of(b))) - 0.5f);
        const lh_int_t x1 = lh_float_ceil_to_int(
            lh_math_max(lh_math_vec3_get_x(lh_addr_of(a)), lh_math_vec3_get_x(lh_addr_of(b))) - 0.5f);
        const lh_int_t y1 = lh_float_ceil_to_int(
            lh_math_max(lh_math_vec3_get_y(lh_addr_of(a)), lh_math_vec3_get_y(lh_addr_of(b))) - 0.5f);
        lh_ui_canvas_fill_rect(canvas, lh_math_rect_make(x0, y0, x1 - x0, y1 - y0), color);
        return;
    }

    /* Rotated: test each pixel center of the bounds in the entity's space.
     * A pixel belongs to the box when its center does, so neighbors share
     * no pixel and leave no gap. */
    lh_math_mat4_t to_local;
    if (!lh_math_mat4_inverse(world, lh_addr_of(to_local)))
    {
        return;
    }
    const lh_math_rect_t clip = lh_ui_canvas_get_clip(canvas);
    const lh_math_rect_t bounds = lh_entity_2d_get_screen_bounds(self);
    const lh_math_rect_t area = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(bounds));
    const lh_math_coord_t y0 = lh_math_rect_get_y(lh_addr_of(area));
    const lh_math_coord_t y1 = y0 + lh_math_rect_get_size_height(lh_addr_of(area));
    const lh_math_coord_t x0 = lh_math_rect_get_x(lh_addr_of(area));
    const lh_math_coord_t x1 = x0 + lh_math_rect_get_size_width(lh_addr_of(area));
    for (lh_math_coord_t y = y0; y < y1; ++y)
    {
        for (lh_math_coord_t x = x0; x < x1; ++x)
        {
            const lh_math_vec3_t center = lh_math_vec3_make(lh_cast_static(lh_float_t, x) + 0.5f,
                                                            lh_cast_static(lh_float_t, y) + 0.5f, 0.0f);
            const lh_math_vec3_t local = lh_math_mat4_transform_point(to_local, center);
            if (lh_entity_2d_has_local_point(size, local))
            {
                lh_ui_canvas_blend_pixel(canvas, x, y, color);
            }
        }
    }
}
