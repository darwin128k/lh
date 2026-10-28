#include <lh/ui/renderer3d.h>
#include <lh/assert.h>
#include <lh/math/float/finite.h>
#include <lh/math/float/round.h>
#include <lh/memory/std.h>
#include <lh/util/return.h>

/* Vertices a triangle may grow to while it is clipped by the six planes. */
enum
{
    lh_ui_renderer3d_clip_capacity = 16
};

lh_bool_t
lh_ui_renderer3d_begin(lh_ui_renderer3d_t *self, lh_ui_pixmap_t *target, lh_math_fscalar_t *depth,
                       lh_usize_t depth_count, const lh_ui_renderer3d_camera_t *camera,
                       lh_u32_t background_argb)
{
    lh_return_if(lh_null_eq(self), lh_bool_false);
    lh_memory_std_set(self, 0, sizeof *self);
    lh_return_if(lh_null_eq(target) || lh_null_eq(depth) || lh_null_eq(camera) ||
                     lh_ui_pixmap_get_format(target) != lh_ui_pixmap_format_argb8888,
                 lh_bool_false);
    const lh_s32_t width = lh_ui_pixmap_get_width(target);
    const lh_s32_t height = lh_ui_pixmap_get_height(target);
    lh_return_if(width < 1 || height < 1 || width > 8192 || height > 8192 ||
                     (lh_usize_t)width * (lh_usize_t)height > depth_count ||
                     !lh_ui_renderer3d_camera_is_usable(camera),
                 lh_bool_false);
    self->target = target;
    self->depth = depth;
    self->width = width;
    self->height = height;
    self->camera = *camera;
    return lh_ui_renderer3d_clear(self, background_argb);
}

lh_bool_t
lh_ui_renderer3d_clear(lh_ui_renderer3d_t *self, lh_u32_t background_argb)
{
    lh_assert_runtime_ref(self);
    for (lh_s32_t y = 0; y < self->height; y++)
    {
        lh_u32_t *row = lh_cast_reinterpret(lh_u32_t *, lh_ui_pixmap_get_row(self->target, y));
        if (lh_null_eq(row))
        {
            self->target = lh_null;
            return lh_bool_false;
        }
        for (lh_s32_t x = 0; x < self->width; x++)
        {
            row[x] = background_argb;
            self->depth[(lh_usize_t)y * self->width + x] = LH_FLOAT_T_MAX;
        }
    }
    return lh_bool_true;
}

lh_math_fscalar_t
lh_ui_renderer3d_get_plane_distance(const lh_ui_renderer3d_t *self, lh_math_vec3_t view,
                                    lh_ui_renderer3d_plane_t plane)
{
    lh_assert_runtime_ref(self);
    const lh_math_fscalar_t ty = self->camera.tangent;
    const lh_math_fscalar_t tx = ty * self->width / self->height;
    const lh_math_fscalar_t x = lh_math_vec3_get_x(&view);
    const lh_math_fscalar_t y = lh_math_vec3_get_y(&view);
    const lh_math_fscalar_t z = lh_math_vec3_get_z(&view);
    switch (plane)
    {
    case lh_ui_renderer3d_plane_near:
        return z - self->camera.near_plane;
    case lh_ui_renderer3d_plane_far:
        return self->camera.far_plane - z;
    case lh_ui_renderer3d_plane_left:
        return x + z * tx;
    case lh_ui_renderer3d_plane_right:
        return z * tx - x;
    case lh_ui_renderer3d_plane_bottom:
        return y + z * ty;
    default:
        return z * ty - y;
    }
}

lh_s32_t
lh_ui_renderer3d_clip_plane(const lh_ui_renderer3d_t *self, lh_ui_renderer3d_plane_t plane,
                            const lh_math_vec3_t *in, lh_s32_t count, lh_math_vec3_t *out,
                            lh_s32_t capacity)
{
    lh_return_if(count < 1, 0);
    lh_s32_t written = 0;
    lh_math_vec3_t previous = in[count - 1];
    lh_math_fscalar_t previous_distance =
        lh_ui_renderer3d_get_plane_distance(self, previous, plane);
    for (lh_s32_t i = 0; i < count; i++)
    {
        const lh_math_vec3_t current = in[i];
        const lh_math_fscalar_t distance =
            lh_ui_renderer3d_get_plane_distance(self, current, plane);
        if ((previous_distance >= 0) != (distance >= 0))
        {
            lh_return_if(written >= capacity, -1);
            lh_math_vec3_lerp(previous, current,
                              previous_distance / (previous_distance - distance),
                              &out[written++]);
        }
        if (distance >= 0)
        {
            lh_return_if(written >= capacity, -1);
            out[written++] = current;
        }
        previous = current;
        previous_distance = distance;
    }
    return written;
}

lh_void
lh_ui_renderer3d_project(const lh_ui_renderer3d_t *self, lh_math_vec3_t view,
                         lh_math_vec3_t *out)
{
    lh_assert_runtime_ref(self);
    const lh_math_fscalar_t focal = self->height / (2 * self->camera.tangent);
    const lh_math_fscalar_t z = lh_math_vec3_get_z(&view);
    lh_math_vec3_init(out, self->width * .5f + lh_math_vec3_get_x(&view) * focal / z,
                      self->height * .5f - lh_math_vec3_get_y(&view) * focal / z, 1 / z);
}

lh_math_fscalar_t
lh_ui_renderer3d_get_edge(lh_math_vec3_t a, lh_math_vec3_t b, lh_math_fscalar_t x,
                          lh_math_fscalar_t y)
{
    const lh_math_fscalar_t ax = lh_math_vec3_get_x(&a);
    const lh_math_fscalar_t ay = lh_math_vec3_get_y(&a);
    return (x - ax) * (lh_math_vec3_get_y(&b) - ay) - (y - ay) * (lh_math_vec3_get_x(&b) - ax);
}

lh_bool_t
lh_ui_renderer3d_put_fragment(lh_ui_renderer3d_t *self, lh_s32_t x, lh_s32_t y,
                              lh_math_fscalar_t depth, lh_u32_t color_argb)
{
    lh_assert_runtime_ref(self);
    const lh_usize_t i = (lh_usize_t)y * self->width + x;
    lh_return_ifn(depth < self->depth[i], lh_bool_false);
    self->depth[i] = depth;
    lh_cast_reinterpret(lh_u32_t *, lh_ui_pixmap_get_row(self->target, y))[x] = color_argb;
    return lh_bool_true;
}

lh_void
lh_ui_renderer3d_raster(lh_ui_renderer3d_t *self, lh_math_vec3_t a, lh_math_vec3_t b,
                        lh_math_vec3_t c, lh_u32_t color_argb)
{
    lh_assert_runtime_ref(self);
    const lh_math_fscalar_t area =
        lh_ui_renderer3d_get_edge(a, b, lh_math_vec3_get_x(&c), lh_math_vec3_get_y(&c));
    lh_return_if(!lh_math_float_is_finite(area) || lh_math_abs(area) < 1e-5f);
    const lh_math_fscalar_t ax = lh_math_vec3_get_x(&a);
    const lh_math_fscalar_t bx = lh_math_vec3_get_x(&b);
    const lh_math_fscalar_t cx = lh_math_vec3_get_x(&c);
    const lh_math_fscalar_t ay = lh_math_vec3_get_y(&a);
    const lh_math_fscalar_t by = lh_math_vec3_get_y(&b);
    const lh_math_fscalar_t cy = lh_math_vec3_get_y(&c);
    const lh_s32_t x0 =
        lh_math_max(0, lh_math_float_floor_to_int(lh_math_min(ax, lh_math_min(bx, cx))));
    const lh_s32_t x1 = lh_math_min(self->width - 1,
                                    lh_math_float_ceil_to_int(lh_math_max(ax, lh_math_max(bx, cx))));
    const lh_s32_t y0 =
        lh_math_max(0, lh_math_float_floor_to_int(lh_math_min(ay, lh_math_min(by, cy))));
    const lh_s32_t y1 = lh_math_min(self->height - 1,
                                    lh_math_float_ceil_to_int(lh_math_max(ay, lh_math_max(by, cy))));
    self->triangles++;
    for (lh_s32_t y = y0; y <= y1; y++)
    {
        for (lh_s32_t x = x0; x <= x1; x++)
        {
            const lh_math_fscalar_t wa = lh_ui_renderer3d_get_edge(b, c, x + .5f, y + .5f) / area;
            const lh_math_fscalar_t wb = lh_ui_renderer3d_get_edge(c, a, x + .5f, y + .5f) / area;
            const lh_math_fscalar_t wc = 1 - wa - wb;
            if (wa < -.00001f || wb < -.00001f || wc < -.00001f)
            {
                continue;
            }
            const lh_math_fscalar_t inverse_depth = wa * lh_math_vec3_get_z(&a) +
                                                    wb * lh_math_vec3_get_z(&b) +
                                                    wc * lh_math_vec3_get_z(&c);
            if (inverse_depth <= 0)
            {
                continue;
            }
            lh_ui_renderer3d_put_fragment(self, x, y, 1 / inverse_depth, color_argb);
        }
    }
}

lh_void
lh_ui_renderer3d_triangle(lh_ui_renderer3d_t *self, lh_math_vec3_t a, lh_math_vec3_t b,
                          lh_math_vec3_t c, lh_u32_t color_argb)
{
    lh_return_if(lh_null_eq(self) || lh_null_eq(self->target) || !lh_math_vec3_is_finite(a) ||
                 !lh_math_vec3_is_finite(b) || !lh_math_vec3_is_finite(c));
    lh_math_vec3_t polygon[2][lh_ui_renderer3d_clip_capacity];
    lh_ui_renderer3d_camera_to_view(&self->camera, a, &polygon[0][0]);
    lh_ui_renderer3d_camera_to_view(&self->camera, b, &polygon[0][1]);
    lh_ui_renderer3d_camera_to_view(&self->camera, c, &polygon[0][2]);
    lh_s32_t count = 3;
    lh_s32_t from = 0;
    for (lh_s32_t plane = 0; plane < lh_ui_renderer3d_plane_count && count > 0; plane++)
    {
        count = lh_ui_renderer3d_clip_plane(self, (lh_ui_renderer3d_plane_t)plane, polygon[from],
                                            count, polygon[1 - from],
                                            lh_ui_renderer3d_clip_capacity);
        lh_return_if(count < 0);
        from = 1 - from;
    }
    for (lh_s32_t i = 0; i < count; i++)
    {
        lh_ui_renderer3d_project(self, polygon[from][i], &polygon[from][i]);
    }
    for (lh_s32_t i = 1; i + 1 < count; i++)
    {
        lh_ui_renderer3d_raster(self, polygon[from][0], polygon[from][i], polygon[from][i + 1],
                                color_argb);
    }
}

lh_void
lh_ui_renderer3d_box(lh_ui_renderer3d_t *self, lh_math_vec3_t minimum, lh_math_vec3_t maximum,
                     lh_u32_t color_argb)
{
    lh_return_if(!lh_math_vec3_is_finite(minimum) || !lh_math_vec3_is_finite(maximum) ||
                 lh_math_vec3_get_x(&minimum) > lh_math_vec3_get_x(&maximum) ||
                 lh_math_vec3_get_y(&minimum) > lh_math_vec3_get_y(&maximum) ||
                 lh_math_vec3_get_z(&minimum) > lh_math_vec3_get_z(&maximum));
    lh_math_vec3_t corner[8];
    for (lh_s32_t i = 0; i < 8; i++)
    {
        lh_math_vec3_init(&corner[i], lh_math_vec3_get_x(i & 1 ? &maximum : &minimum),
                          lh_math_vec3_get_y(i & 2 ? &maximum : &minimum),
                          lh_math_vec3_get_z(i & 4 ? &maximum : &minimum));
    }
    static const lh_u8_t faces[12][3] = {{0, 1, 3}, {0, 3, 2}, {4, 6, 7}, {4, 7, 5},
                                         {0, 4, 5}, {0, 5, 1}, {2, 3, 7}, {2, 7, 6},
                                         {0, 2, 6}, {0, 6, 4}, {1, 5, 7}, {1, 7, 3}};
    for (lh_s32_t i = 0; i < 12; i++)
    {
        lh_ui_renderer3d_triangle(self, corner[faces[i][0]], corner[faces[i][1]],
                                  corner[faces[i][2]], color_argb);
    }
}

lh_u32_t
lh_ui_renderer3d_get_triangles(const lh_ui_renderer3d_t *self)
{
    return lh_null_ne(self) ? self->triangles : 0;
}
