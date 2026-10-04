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

/* Inverse of the local x/y axes in the screen plane, plus the world z of a
 * point on the local z = 0 plane. The view ray at (sx, sy) is that screen
 * point at every depth. */
typedef struct lh_entity_2d_plane
{
    lh_float_t inv00;
    lh_float_t inv01;
    lh_float_t inv10;
    lh_float_t inv11;
    lh_float_t origin_x;
    lh_float_t origin_y;
    lh_float_t z_x;
    lh_float_t z_y;
    lh_float_t z_origin;
    lh_bool_t ok;
} lh_entity_2d_plane_t;

static lh_entity_2d_plane_t
lh_entity_2d_plane_from_world(const lh_math_mat4_t *world)
{
    const lh_math_vec4_t column0 = lh_math_mat4_get_column(world, 0);
    const lh_math_vec4_t column1 = lh_math_mat4_get_column(world, 1);
    const lh_math_vec4_t origin = lh_math_mat4_get_column(world, 3);
    const lh_float_t m00 = lh_math_vec4_get_x(lh_addr_of(column0));
    const lh_float_t m10 = lh_math_vec4_get_y(lh_addr_of(column0));
    const lh_float_t m01 = lh_math_vec4_get_x(lh_addr_of(column1));
    const lh_float_t m11 = lh_math_vec4_get_y(lh_addr_of(column1));
    const lh_float_t det = m00 * m11 - m01 * m10;

    lh_entity_2d_plane_t plane;
    plane.origin_x = lh_math_vec4_get_x(lh_addr_of(origin));
    plane.origin_y = lh_math_vec4_get_y(lh_addr_of(origin));
    plane.z_x = lh_math_vec4_get_z(lh_addr_of(column0));
    plane.z_y = lh_math_vec4_get_z(lh_addr_of(column1));
    plane.z_origin = lh_math_vec4_get_z(lh_addr_of(origin));
    /* Edge-on to the view: the plane covers no area. */
    if (det > -1.0e-8f && det < 1.0e-8f)
    {
        plane.inv00 = 0.0f;
        plane.inv01 = 0.0f;
        plane.inv10 = 0.0f;
        plane.inv11 = 0.0f;
        plane.ok = lh_bool_false;
        return plane;
    }
    const lh_float_t inv_det = 1.0f / det;
    plane.inv00 = m11 * inv_det;
    plane.inv01 = -m01 * inv_det;
    plane.inv10 = -m10 * inv_det;
    plane.inv11 = m00 * inv_det;
    plane.ok = lh_bool_true;
    return plane;
}

/* True when the view ray at (sx, sy) crosses the box. Writes the world z. */
static lh_bool_t
lh_entity_2d_plane_hit(const lh_entity_2d_plane_t *plane, lh_math_vec2_t size, lh_float_t sx,
                       lh_float_t sy, lh_float_t *z_out)
{
    if (!plane->ok)
    {
        return lh_bool_false;
    }
    const lh_float_t dx = sx - plane->origin_x;
    const lh_float_t dy = sy - plane->origin_y;
    const lh_math_vec3_t local = lh_math_vec3_make(plane->inv00 * dx + plane->inv01 * dy,
                                                   plane->inv10 * dx + plane->inv11 * dy, 0.0f);
    if (!lh_entity_2d_has_local_point(size, local))
    {
        return lh_bool_false;
    }
    *z_out = plane->z_x * lh_math_vec3_get_x(lh_addr_of(local)) +
             plane->z_y * lh_math_vec3_get_y(lh_addr_of(local)) + plane->z_origin;
    return lh_bool_true;
}

lh_bool_t
lh_entity_2d_contains(const lh_entity_2d_t *self, lh_math_vec2_t point)
{
    lh_assert_runtime_ref(self);
    if (!lh_entity_2d_has_box(self))
    {
        return lh_bool_false;
    }

    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(self);
    const lh_entity_2d_plane_t plane = lh_entity_2d_plane_from_world(lh_addr_of(world));
    lh_float_t z = 0.0f;
    return lh_entity_2d_plane_hit(lh_addr_of(plane), lh_entity_2d_get_size(self),
                                  lh_math_vec2_get_x(lh_addr_of(point)),
                                  lh_math_vec2_get_y(lh_addr_of(point)), lh_addr_of(z));
}

/* A hit and how close it is. A larger z is closer. */
typedef struct lh_entity_2d_pick
{
    lh_entity_t *entity;
    lh_float_t z;
} lh_entity_2d_pick_t;

static lh_entity_2d_pick_t
lh_entity_2d_pick(lh_entity_t *node, lh_math_vec2_t point)
{
    const lh_entity_2d_pick_t none = {lh_null, 0.0f};
    if (lh_entity_has_flags(node, lh_entity_flags_hidden))
    {
        return none;
    }

    const lh_entity_2d_t *const entity = lh_entity_cast(node, lh_addr_of(lh_entity_2d_class));
    const lh_bool_t has_box = lh_ptr_is_set(entity) && lh_entity_2d_has_box(entity);
    lh_float_t z = 0.0f;
    lh_bool_t inside = lh_bool_false;
    if (has_box)
    {
        const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(entity);
        const lh_entity_2d_plane_t plane = lh_entity_2d_plane_from_world(lh_addr_of(world));
        inside = lh_entity_2d_plane_hit(lh_addr_of(plane), lh_entity_2d_get_size(entity),
                                        lh_math_vec2_get_x(lh_addr_of(point)),
                                        lh_math_vec2_get_y(lh_addr_of(point)), lh_addr_of(z));
    }

    /* Children that the box cuts away are not searched. No box does not
     * hide children. Among hits, a larger z wins; equal z keeps the later
     * one, which is the younger sibling and, against the parent, the child. */
    lh_entity_2d_pick_t best = none;
    if (!has_box || inside || lh_entity_has_flags(node, lh_entity_flags_overflow_visible))
    {
        lh_entity_foreach_child(child, node)
        {
            const lh_entity_2d_pick_t child_pick = lh_entity_2d_pick(child, point);
            if (lh_ptr_is_set(child_pick.entity) &&
                (lh_ptr_is_null(best.entity) || child_pick.z >= best.z))
            {
                best = child_pick;
            }
        }
    }
    /* Strictly closer than the best child, so an equal child stays on top. */
    if (inside && (lh_ptr_is_null(best.entity) || z > best.z))
    {
        best.entity = node;
        best.z = z;
    }
    return best;
}

lh_entity_t *
lh_entity_2d_find_at(lh_entity_t *root, lh_math_vec2_t point)
{
    return lh_entity_2d_pick(root, point).entity;
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
    const lh_math_vec4_t world_origin = lh_math_mat4_get_column(lh_addr_of(world), 3);
    if (lh_math_vec4_get_y(lh_addr_of(world_col_0)) == 0.0f &&
        lh_math_vec4_get_z(lh_addr_of(world_col_0)) == 0.0f &&
        lh_math_vec4_get_x(lh_addr_of(world_col_1)) == 0.0f &&
        lh_math_vec4_get_z(lh_addr_of(world_col_1)) == 0.0f)
    {
        /* Parallel to the screen and not rotated in it: the covered pixels
         * are themselves a rectangle, all at one depth. */
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
        lh_ui_canvas_set_draw_z(canvas, lh_math_vec4_get_z(lh_addr_of(world_origin)));
        lh_ui_canvas_fill_rect(canvas, lh_math_rect_make(x0, y0, x1 - x0, y1 - y0), color);
        return;
    }

    /* Tilted or rotated in the plane: a pixel belongs to the box when its
     * center's view ray crosses the box, so neighbors share no pixel. */
    const lh_entity_2d_plane_t plane = lh_entity_2d_plane_from_world(lh_addr_of(world));
    if (!plane.ok)
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
            lh_float_t z = 0.0f;
            if (lh_entity_2d_plane_hit(lh_addr_of(plane), size,
                                       lh_cast_static(lh_float_t, x) + 0.5f,
                                       lh_cast_static(lh_float_t, y) + 0.5f, lh_addr_of(z)))
            {
                lh_ui_canvas_set_draw_z(canvas, z);
                lh_ui_canvas_blend_pixel(canvas, x, y, color);
            }
        }
    }
}
