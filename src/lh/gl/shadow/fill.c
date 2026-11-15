/**
 * @file fill.c
 * @brief Paint the soft shadow around an entity rectangle.
 */
#include <lh/gl/shadow/fill.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/memory/std.h>
#include <lh/gl/canvas.h>
#include <lh/gl/entity/class/initializer.h>

lh_gl_entity_t *
lh_gl_shadow_fill_get_entity_mut(lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_entity_t *
lh_gl_shadow_fill_get_entity(const lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_shadow_t *
lh_gl_shadow_fill_get_shadow(const lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shadow;
}

lh_void
lh_gl_shadow_fill_set_shadow(lh_gl_shadow_fill_t *self, const lh_gl_shadow_t *shadow)
{
    lh_assert_runtime_ref(self);
    self->shadow = shadow;
    lh_gl_shadow_fill_place(self);
}

const lh_gl_rect_t *
lh_gl_shadow_fill_get_box(const lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

lh_void
lh_gl_shadow_fill_set_box(lh_gl_shadow_fill_t *self, lh_gl_rect_t box)
{
    lh_assert_runtime_ref(self);
    self->box = box;
    lh_gl_shadow_fill_place(self);
}

lh_gl_scalar_t
lh_gl_shadow_fill_get_radius(const lh_gl_shadow_fill_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_gl_shadow_fill_set_radius(lh_gl_shadow_fill_t *self, lh_gl_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
    lh_gl_entity_invalidate(lh_gl_shadow_fill_get_entity_mut(self));
}

lh_void
lh_gl_shadow_fill_place(lh_gl_shadow_fill_t *self)
{
    const lh_gl_rect_t *box = lh_gl_shadow_fill_get_box(self);
    const lh_gl_shadow_t *shadow = lh_gl_shadow_fill_get_shadow(self);

    if (lh_ptr_is_null(shadow) || lh_gl_shadow_is_empty(shadow))
    {
        lh_gl_entity_set_rect(lh_gl_shadow_fill_get_entity_mut(self), lh_ptr_deref(box));
        return;
    }
    lh_gl_scalar_t spread = lh_gl_shadow_get_spread(shadow);
    lh_gl_scalar_t shift_x = lh_gl_shadow_get_offset_x(shadow);
    lh_gl_scalar_t shift_y = lh_gl_shadow_get_offset_y(shadow);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(box);
    lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(box));
    lh_gl_scalar_t height = lh_gl_size_get_height(lh_gl_rect_get_size(box));
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), lh_gl_point_get_x(origin) + lh_math_min(0, shift_x) - spread,
                    lh_gl_point_get_y(origin) + lh_math_min(0, shift_y) - spread,
                    width + lh_math_abs(shift_x) + spread * 2, height + lh_math_abs(shift_y) + spread * 2);
    lh_gl_entity_set_rect(lh_gl_shadow_fill_get_entity_mut(self), bounds);
}

const lh_gl_entity_class_t *
lh_gl_shadow_fill_get_class(void)
{
    static const lh_gl_entity_class_t shadow_fill_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_entity, lh_gl_shadow_fill_draw, lh_gl_shadow_fill_pick,
        lh_bool_false);

    return lh_addr_of(shadow_fill_class);
}

lh_void
lh_gl_shadow_fill_init(lh_gl_shadow_fill_t *self, lh_gl_rect_t rect, const lh_gl_shadow_t *shadow)
{
    lh_memory_std_set(self, 0, sizeof(lh_gl_shadow_fill_t));
    lh_gl_entity_init(lh_gl_shadow_fill_get_entity_mut(self), rect, lh_gl_shadow_fill_get_class());
    lh_gl_shadow_fill_set_box(self, rect);
    lh_gl_shadow_fill_set_shadow(self, shadow);
    lh_gl_shadow_fill_set_radius(self, 0);
}

lh_void
lh_gl_shadow_fill_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_gl_shadow_t *shadow =
        lh_gl_shadow_fill_get_shadow(lh_ptr_ccast(lh_gl_shadow_fill_t, entity));

    if (lh_ptr_is_null(shadow) || lh_gl_shadow_is_empty(shadow))
    {
        return;
    }
    const lh_gl_rect_t *area = lh_gl_entity_get_rect(entity);
    lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(area));
    lh_gl_scalar_t height = lh_gl_size_get_height(lh_gl_rect_get_size(area));

    if (width <= 0 || height <= 0)
    {
        return;
    }
    const lh_gl_rect_t *box = lh_gl_shadow_fill_get_box(lh_ptr_ccast(lh_gl_shadow_fill_t, entity));
    const lh_gl_point_t *entity_origin = lh_gl_rect_get_origin(area);
    const lh_gl_point_t *box_origin = lh_gl_rect_get_origin(box);
    lh_gl_rect_t local;

    lh_gl_rect_init(lh_addr_of(local), lh_gl_point_get_x(box_origin) - lh_gl_point_get_x(entity_origin),
                    lh_gl_point_get_y(box_origin) - lh_gl_point_get_y(entity_origin),
                    lh_gl_size_get_width(lh_gl_rect_get_size(box)),
                    lh_gl_size_get_height(lh_gl_rect_get_size(box)));
    lh_gl_scalar_t radius =
        lh_gl_shadow_fill_get_radius(lh_ptr_ccast(lh_gl_shadow_fill_t, entity));
    lh_gl_scalar_t corner = lh_gl_shadow_clamp_radius(lh_addr_of(local), radius);
    lh_gl_scalar_t box_left = lh_gl_point_get_x(box_origin) - lh_gl_point_get_x(entity_origin);
    lh_gl_scalar_t box_top = lh_gl_point_get_y(box_origin) - lh_gl_point_get_y(entity_origin);
    lh_gl_scalar_t box_width = lh_gl_size_get_width(lh_gl_rect_get_size(box));
    lh_gl_scalar_t box_height = lh_gl_size_get_height(lh_gl_rect_get_size(box));
    lh_gl_scalar_t center_x = box_left + box_width / 2;
    lh_gl_scalar_t center_y = box_top + box_height / 2;
    lh_gl_scalar_t half_width = box_width / 2;
    lh_gl_scalar_t half_height = box_height / 2;
    lh_gl_scalar_t shift_x = lh_gl_shadow_get_offset_x(shadow);
    lh_gl_scalar_t shift_y = lh_gl_shadow_get_offset_y(shadow);
    lh_gl_scalar_t spread = lh_gl_shadow_get_spread(shadow);
    lh_gl_color_channel_t peak = lh_gl_color_get_a(lh_gl_shadow_get_color(shadow));
    lh_gl_color_channel_t ramp[257];
    lh_bool_t use_ramp = spread > 0 && spread <= 128 ? lh_bool_true : lh_bool_false;

    if (use_ramp == lh_bool_true)
    {
        for (lh_gl_scalar_t distance = -spread; distance <= spread; ++distance)
        {
            ramp[distance + spread] = lh_gl_shadow_falloff(distance, spread, peak);
        }
    }
    lh_gl_color_channel_t red = lh_gl_color_get_r(lh_gl_shadow_get_color(shadow));
    lh_gl_color_channel_t green = lh_gl_color_get_g(lh_gl_shadow_get_color(shadow));
    lh_gl_color_channel_t blue = lh_gl_color_get_b(lh_gl_shadow_get_color(shadow));

    for (lh_gl_scalar_t y = 0; y < height; ++y)
    {
        lh_gl_color_t chunk[128];
        lh_gl_scalar_t run_x = 0;
        lh_gl_scalar_t run_n = 0;
        lh_gl_scalar_t dy_box = lh_math_abs(y - center_y) - (half_height - corner);
        lh_gl_scalar_t dy_shift =
            lh_math_abs(y - (center_y + shift_y)) - (half_height - corner);

        if (corner > 0 && shift_x == 0 && dy_box <= 0 && dy_shift <= 0)
        {
            lh_gl_scalar_t edge = center_x - half_width;
            lh_gl_scalar_t from = edge - spread;
            lh_gl_scalar_t to = edge;

            if (from < 0)
            {
                from = 0;
            }
            if (to >= width)
            {
                to = width - 1;
            }
            for (lh_gl_scalar_t side = 0; side < 2; ++side)
            {
                if (side == 1)
                {
                    edge = center_x + half_width;
                    from = edge;
                    to = edge + spread;
                    if (from < 0)
                    {
                        from = 0;
                    }
                    if (to >= width)
                    {
                        to = width - 1;
                    }
                }
                run_n = 0;
                for (lh_gl_scalar_t x = from; x <= to; ++x)
                {
                    lh_gl_scalar_t distance = side == 0 ? edge - x : x - edge;
                    lh_gl_color_channel_t coverage;

                    if (use_ramp == lh_bool_true)
                    {
                        if (distance < -spread)
                        {
                            distance = -spread;
                        }
                        if (distance > spread)
                        {
                            distance = spread;
                        }
                        coverage = ramp[distance + spread];
                    }
                    else
                    {
                        coverage = lh_gl_shadow_falloff(distance, spread, peak);
                    }
                    if (coverage == 0)
                    {
                        if (run_n > 0)
                        {
                            lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                            run_n = 0;
                        }
                        continue;
                    }
                    if (run_n == 128)
                    {
                        lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                        run_n = 0;
                    }
                    if (run_n == 0)
                    {
                        run_x = x;
                    }
                    lh_gl_color_t *ink = lh_ptr_add_by_offset(
                        lh_gl_color_t, chunk, (lh_uaddr_t)run_n * sizeof(lh_gl_color_t));

                    lh_gl_color_init(ink, red, green, blue, coverage);
                    run_n = run_n + 1;
                }
                if (run_n > 0)
                {
                    lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                }
            }
            continue;
        }
        if (corner > 0 && shift_x == 0 && dy_box > 0 && dy_shift > 0)
        {
            lh_gl_scalar_t inner = half_width - corner;
            lh_gl_scalar_t mid_left = center_x - inner;
            lh_gl_scalar_t mid_right = center_x + inner;

            if (dy_box - corner >= 0)
            {
                lh_gl_scalar_t distance = dy_shift - corner;
                lh_gl_color_channel_t coverage;
                lh_gl_scalar_t from = mid_left;
                lh_gl_scalar_t to = mid_right;

                if (use_ramp == lh_bool_true)
                {
                    if (distance < -spread)
                    {
                        distance = -spread;
                    }
                    if (distance > spread)
                    {
                        distance = spread;
                    }
                    coverage = ramp[distance + spread];
                }
                else
                {
                    coverage = lh_gl_shadow_falloff(distance, spread, peak);
                }
                if (from < 0)
                {
                    from = 0;
                }
                if (to >= width)
                {
                    to = width - 1;
                }
                if (coverage != 0 && from <= to)
                {
                    lh_gl_color_t ink;

                    lh_gl_color_init(lh_addr_of(ink), red, green, blue, coverage);
                    lh_gl_rect_t band;

                    lh_gl_rect_init(lh_addr_of(band), from, y, to - from + 1, 1);
                    lh_gl_canvas_fill_rect(canvas, lh_addr_of(band), lh_addr_of(ink));
                }
            }
            lh_u64_t ay2_box = lh_cast_static(lh_u64_t, dy_box) * lh_cast_static(lh_u64_t, dy_box);
            lh_u64_t ay2_shift =
                lh_cast_static(lh_u64_t, dy_shift) * lh_cast_static(lh_u64_t, dy_shift);
            lh_u64_t corner_square =
                lh_cast_static(lh_u64_t, corner) * lh_cast_static(lh_u64_t, corner);
            lh_gl_scalar_t root_shift = 0;
            lh_gl_scalar_t x_begin = mid_left - (spread + corner + 2);

            if (x_begin < 0)
            {
                x_begin = 0;
            }
            run_n = 0;
            for (lh_gl_scalar_t side = 0; side < 2; ++side)
            {
                lh_gl_scalar_t x_from = side == 0 ? x_begin : mid_right + 1;
                lh_gl_scalar_t x_to = side == 0 ? mid_left : width;

                root_shift = 0;
                run_n = 0;
                for (lh_gl_scalar_t x = x_from; x < x_to; ++x)
                {
                    lh_gl_scalar_t dx = side == 0 ? mid_left - x : x - mid_right;
                    lh_u64_t dx2 = lh_cast_static(lh_u64_t, dx) * lh_cast_static(lh_u64_t, dx);
                    lh_u64_t square_shift = dx2 + ay2_shift;

                    while (root_shift > 0 &&
                           lh_cast_static(lh_u64_t, root_shift) * root_shift > square_shift)
                    {
                        root_shift = root_shift - 1;
                    }
                    while (lh_cast_static(lh_u64_t, root_shift + 1) * (root_shift + 1) <=
                           square_shift)
                    {
                        root_shift = root_shift + 1;
                    }
                    if (dx2 + ay2_box < corner_square)
                    {
                        if (side == 0)
                        {
                            break;
                        }
                        continue;
                    }
                    lh_gl_scalar_t distance = root_shift - corner;

                    if (distance >= spread)
                    {
                        if (side == 1)
                        {
                            break;
                        }
                        continue;
                    }
                    lh_gl_color_channel_t coverage;

                    if (use_ramp == lh_bool_true)
                    {
                        if (distance < -spread)
                        {
                            distance = -spread;
                        }
                        coverage = ramp[distance + spread];
                    }
                    else
                    {
                        coverage = lh_gl_shadow_falloff(distance, spread, peak);
                    }
                    if (coverage == 0)
                    {
                        continue;
                    }
                    if (run_n == 128)
                    {
                        lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                        run_n = 0;
                    }
                    if (run_n == 0)
                    {
                        run_x = x;
                    }
                    lh_gl_color_t *ink = lh_ptr_add_by_offset(
                        lh_gl_color_t, chunk, (lh_uaddr_t)run_n * sizeof(lh_gl_color_t));

                    lh_gl_color_init(ink, red, green, blue, coverage);
                    run_n = run_n + 1;
                }
                if (run_n > 0)
                {
                    lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                }
            }
            continue;
        }
        if (corner > 0 && shift_x == 0)
        {
            lh_gl_scalar_t left_end = 0;
            lh_gl_scalar_t right_begin = 0;

            lh_gl_shadow_row_outside(center_x, half_width, dy_box, corner, width,
                                     lh_addr_of(left_end), lh_addr_of(right_begin));
            for (lh_gl_scalar_t side = 0; side < 2; ++side)
            {
                lh_gl_scalar_t x_from = side == 0 ? 0 : right_begin;
                lh_gl_scalar_t x_to = side == 0 ? left_end : width;

                run_n = 0;
                for (lh_gl_scalar_t x = x_from; x < x_to; ++x)
                {
                    lh_gl_scalar_t distance =
                        lh_gl_shadow_distance_at(x, y, center_x + shift_x, center_y + shift_y,
                                                 half_width, half_height, corner);
                    lh_gl_color_channel_t coverage;

                    if (use_ramp == lh_bool_true)
                    {
                        if (distance < -spread)
                        {
                            distance = -spread;
                        }
                        if (distance > spread)
                        {
                            distance = spread;
                        }
                        coverage = ramp[distance + spread];
                    }
                    else
                    {
                        coverage = lh_gl_shadow_falloff(distance, spread, peak);
                    }
                    if (coverage == 0)
                    {
                        if (run_n > 0)
                        {
                            lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                            run_n = 0;
                        }
                        continue;
                    }
                    if (run_n == 128)
                    {
                        lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                        run_n = 0;
                    }
                    if (run_n == 0)
                    {
                        run_x = x;
                    }
                    lh_gl_color_t *ink = lh_ptr_add_by_offset(
                        lh_gl_color_t, chunk, (lh_uaddr_t)run_n * sizeof(lh_gl_color_t));

                    lh_gl_color_init(ink, red, green, blue, coverage);
                    run_n = run_n + 1;
                }
                if (run_n > 0)
                {
                    lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                }
            }
            continue;
        }

        for (lh_gl_scalar_t x = 0; x < width; ++x)
        {
            lh_gl_color_channel_t coverage = 0;

            if (corner == 0)
            {
                if (x >= box_left && x < box_left + box_width && y >= box_top &&
                    y < box_top + box_height)
                {
                    coverage = 0;
                }
                else
                {
                    coverage = 1;
                }
            }
            else if (lh_gl_shadow_distance_at(x, y, center_x, center_y, half_width, half_height,
                                              corner) < 0)
            {
                coverage = 0;
            }
            else
            {
                coverage = 1;
            }
            if (coverage != 0)
            {
                lh_gl_scalar_t distance =
                    lh_gl_shadow_distance_at(x, y, center_x + shift_x, center_y + shift_y,
                                             half_width, half_height, corner);

                if (use_ramp == lh_bool_true)
                {
                    if (distance < -spread)
                    {
                        distance = -spread;
                    }
                    if (distance > spread)
                    {
                        distance = spread;
                    }
                    coverage = ramp[distance + spread];
                }
                else
                {
                    coverage = lh_gl_shadow_falloff(distance, spread, peak);
                }
            }

            if (coverage == 0)
            {
                if (run_n > 0)
                {
                    lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                    run_n = 0;
                }
                continue;
            }
            if (run_n == 128)
            {
                lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
                run_n = 0;
            }
            if (run_n == 0)
            {
                run_x = x;
            }
            lh_gl_color_init(lh_ptr_add_by_offset(lh_gl_color_t, chunk,
                                                  (lh_uaddr_t)run_n * sizeof(lh_gl_color_t)),
                             red, green, blue, coverage);
            run_n = run_n + 1;
        }
        if (run_n > 0)
        {
            lh_gl_canvas_fill_colors(canvas, run_x, y, chunk, run_n);
        }
    }
}

lh_bool_t
lh_gl_shadow_fill_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop)
{
    LH_ATTRIBUTE_UNUSED(entity);
    LH_ATTRIBUTE_UNUSED(point);
    LH_ATTRIBUTE_UNUSED(slop);
    return lh_bool_false;
}
