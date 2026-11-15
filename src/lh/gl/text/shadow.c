/**
 * @file shadow.c
 * @brief Paint a line's glyphs as a shifted, blurred shadow.
 */

#include <lh/gl/text/shadow.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/math.h>
#include <lh/core/memory/std.h>
#include <lh/gl/blur.h>
#include <lh/gl/canvas.h>
#include <lh/gl/entity/class/initializer.h>
#include <lh/gl/pixmap.h>

lh_gl_entity_t *
lh_gl_text_shadow_get_entity_mut(lh_gl_text_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_entity_t *
lh_gl_text_shadow_get_entity(const lh_gl_text_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_text_t *
lh_gl_text_shadow_get_line(const lh_gl_text_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line;
}

lh_void
lh_gl_text_shadow_set_line(lh_gl_text_shadow_t *self, const lh_gl_text_t *line)
{
    lh_assert_runtime_ref(self);
    self->line = line;
    lh_gl_text_shadow_place(self);
}

const lh_gl_shadow_t *
lh_gl_text_shadow_get_shadow(const lh_gl_text_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shadow;
}

lh_void
lh_gl_text_shadow_set_shadow(lh_gl_text_shadow_t *self, const lh_gl_shadow_t *shadow)
{
    lh_assert_runtime_ref(self);
    self->shadow = shadow;
    lh_gl_text_shadow_place(self);
}

lh_gl_color_t *
lh_gl_text_shadow_get_scratch(const lh_gl_text_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch;
}

lh_usize_t
lh_gl_text_shadow_get_scratch_count(const lh_gl_text_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch_count;
}

lh_void
lh_gl_text_shadow_set_scratch(lh_gl_text_shadow_t *self, lh_gl_color_t *scratch, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->scratch = scratch;
    self->scratch_count = count;
}

lh_bool_t
lh_gl_text_shadow_get_shown(const lh_gl_text_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shown;
}

lh_void
lh_gl_text_shadow_set_shown(lh_gl_text_shadow_t *self, lh_bool_t shown)
{
    lh_assert_runtime_ref(self);
    self->shown = shown;
    lh_gl_entity_invalidate(lh_gl_text_shadow_get_entity_mut(self));
}

lh_usize_t
lh_gl_text_shadow_scratch_count(const lh_gl_rect_t *rect, lh_gl_scalar_t spread)
{
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);
    lh_gl_scalar_t width = lh_gl_size_get_width(size);
    lh_gl_scalar_t height = lh_gl_size_get_height(size);

    if (lh_gl_rect_is_empty(rect))
    {
        return 0U;
    }
    lh_usize_t pixels = lh_type_cast(lh_usize_t, width) * lh_type_cast(lh_usize_t, height);

    if (spread <= 0)
    {
        return pixels;
    }
    return pixels + lh_gl_blur_scratch_count(rect, spread);
}

lh_void
lh_gl_text_shadow_place(lh_gl_text_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    const lh_gl_text_t *line = lh_gl_text_shadow_get_line(self);

    if (lh_ptr_is_null(line))
    {
        lh_gl_rect_t empty;

        lh_gl_rect_init(lh_addr_of(empty), 0, 0, 0, 0);
        lh_gl_entity_set_rect(lh_gl_text_shadow_get_entity_mut(self), empty);
        return;
    }
    const lh_gl_rect_t *box = lh_gl_entity_get_rect(lh_gl_text_get_entity(line));
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(box);
    const lh_gl_size_t *size = lh_gl_rect_get_size(box);
    const lh_gl_shadow_t *shadow = lh_gl_text_shadow_get_shadow(self);
    lh_gl_scalar_t spread = 0;
    lh_gl_scalar_t shift_x = 0;
    lh_gl_scalar_t shift_y = 0;

    if (lh_ptr_is_set(shadow))
    {
        spread = lh_gl_shadow_get_spread(shadow);
        shift_x = lh_gl_shadow_get_offset_x(shadow);
        shift_y = lh_gl_shadow_get_offset_y(shadow);
    }
    if (spread < 0)
    {
        spread = 0;
    }
    lh_gl_scalar_t left = lh_gl_point_get_x(origin) + lh_math_min(shift_x, 0) - spread;
    lh_gl_scalar_t top = lh_gl_point_get_y(origin) + lh_math_min(shift_y, 0) - spread;
    lh_gl_scalar_t wide = lh_gl_size_get_width(size) + lh_math_abs(shift_x) + spread + spread;
    lh_gl_scalar_t high = lh_gl_size_get_height(size) + lh_math_abs(shift_y) + spread + spread;
    lh_gl_rect_t fitted;

    lh_gl_rect_init(lh_addr_of(fitted), left, top, wide, high);
    lh_gl_entity_set_rect(lh_gl_text_shadow_get_entity_mut(self), fitted);
}

const lh_gl_entity_class_t *
lh_gl_text_shadow_get_class(void)
{
    static const lh_gl_entity_class_t text_shadow_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_entity, lh_gl_text_shadow_draw, lh_gl_text_shadow_pick,
        lh_bool_false);

    return lh_addr_of(text_shadow_class);
}

lh_void
lh_gl_text_shadow_init(lh_gl_text_shadow_t *self, const lh_gl_text_t *line,
                       const lh_gl_shadow_t *shadow, lh_gl_color_t *scratch,
                       lh_usize_t scratch_count)
{
    lh_gl_rect_t empty;

    lh_assert_runtime_ref(self);
    lh_gl_rect_init(lh_addr_of(empty), 0, 0, 0, 0);
    lh_gl_entity_init(lh_gl_text_shadow_get_entity_mut(self), empty, lh_gl_text_shadow_get_class());
    lh_gl_text_shadow_set_scratch(self, scratch, scratch_count);
    lh_gl_text_shadow_set_shown(self, lh_bool_true);
    lh_gl_text_shadow_set_line(self, lh_null);
    lh_gl_text_shadow_set_shadow(self, shadow);
    lh_gl_text_shadow_set_line(self, line);
}

lh_void
lh_gl_text_shadow_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_gl_text_shadow_t *self = lh_ptr_ccast(lh_gl_text_shadow_t, entity);
    const lh_gl_text_t *line = lh_gl_text_shadow_get_line(self);

    if (lh_gl_text_shadow_get_shown(self) == lh_bool_false || lh_ptr_is_null(line) ||
        lh_gl_text_get_shown(line) == lh_bool_false)
    {
        return;
    }
    const lh_gl_shadow_t *shadow = lh_gl_text_shadow_get_shadow(self);

    if (lh_ptr_is_null(shadow))
    {
        return;
    }
    const lh_gl_rect_t *area = lh_gl_entity_get_rect(entity);
    lh_gl_scalar_t spread = lh_gl_shadow_get_spread(shadow);

    if (spread < 0)
    {
        spread = 0;
    }
    if (lh_ptr_is_null(lh_gl_text_shadow_get_scratch(self)) ||
        lh_gl_text_shadow_get_scratch_count(self) < lh_gl_text_shadow_scratch_count(area, spread))
    {
        return;
    }
    const lh_gl_size_t *size = lh_gl_rect_get_size(area);
    lh_gl_scalar_t width = lh_gl_size_get_width(size);
    lh_gl_scalar_t height = lh_gl_size_get_height(size);

    if (width <= 0 || height <= 0)
    {
        return;
    }
    lh_gl_color_t *pixels = lh_gl_text_shadow_get_scratch(self);
    lh_usize_t pixel_count = lh_type_cast(lh_usize_t, width) * lh_type_cast(lh_usize_t, height);

    lh_memory_std_set(pixels, 0, pixel_count * sizeof(lh_gl_color_t));
    lh_gl_size_t grid;

    lh_gl_size_init(lh_addr_of(grid), width, height);
    lh_gl_pixmap_t ink;

    lh_gl_pixmap_init(lh_addr_of(ink), pixels, grid);
    lh_gl_point_t zero;

    lh_gl_point_init(lh_addr_of(zero), 0, 0);
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), 0, 0, width, height);
    lh_gl_canvas_t local;

    lh_gl_canvas_init(lh_addr_of(local), zero, bounds);
    lh_gl_canvas_set_target(lh_addr_of(local), lh_addr_of(ink));
    lh_gl_canvas_set_backend(lh_addr_of(local), lh_gl_pixmap_get_canvas_backend());
    const lh_gl_rect_t *box = lh_gl_entity_get_rect(lh_gl_text_get_entity(line));
    const lh_gl_point_t *from = lh_gl_rect_get_origin(box);
    const lh_gl_point_t *at = lh_gl_rect_get_origin(area);

    lh_gl_text_paint(line, lh_addr_of(local), lh_gl_shadow_get_color(shadow),
                     lh_gl_point_get_x(from) - lh_gl_point_get_x(at) +
                         lh_gl_shadow_get_offset_x(shadow),
                     lh_gl_point_get_y(from) - lh_gl_point_get_y(at) +
                         lh_gl_shadow_get_offset_y(shadow));
    if (spread > 0)
    {
        lh_gl_color_t *blur = lh_ptr_add_by_offset(lh_gl_color_t, pixels,
                                                   pixel_count * sizeof(lh_gl_color_t));

        lh_gl_blur_rect(lh_addr_of(ink), lh_addr_of(bounds), spread, blur);
    }
    lh_gl_pixmap_t *dest = lh_ptr_cast(lh_gl_pixmap_t, lh_gl_canvas_get_target(canvas));
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(canvas);
    lh_gl_scalar_t x0 = lh_gl_point_get_x(origin);
    lh_gl_scalar_t y0 = lh_gl_point_get_y(origin);

    for (lh_gl_scalar_t row = 0; row < height; ++row)
    {
        lh_uaddr_t step = lh_type_cast(lh_uaddr_t, row) * lh_type_cast(lh_uaddr_t, width);
        const lh_gl_color_t *samples = lh_ptr_add_by_offset(const lh_gl_color_t, pixels,
                                                            step * sizeof(lh_gl_color_t));

        lh_gl_pixmap_fill_colors(dest, lh_gl_canvas_get_clip(canvas), x0, y0 + row, samples, width);
    }
}

lh_bool_t
lh_gl_text_shadow_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop)
{
    LH_ATTRIBUTE_UNUSED(entity);
    LH_ATTRIBUTE_UNUSED(point);
    LH_ATTRIBUTE_UNUSED(slop);
    return lh_bool_false;
}
