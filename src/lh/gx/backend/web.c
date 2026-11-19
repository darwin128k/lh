/**
 * @file web.c
 * @brief Implementation of `lh/gx/backend/web.h`.
 */

#include <lh/gx/backend/web.h>

#include <lh/core/char/map.h>
#include <lh/core/util/return.h>

lh_web_json_writer_t *
lh_gx_backend_web_get_writer_mut(const lh_gx_backend_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->writer;
}

lh_void
lh_gx_backend_web_set_writer(lh_gx_backend_web_t *self, lh_web_json_writer_t *writer)
{
    lh_assert_runtime_ref(self);
    self->writer = writer;
}

lh_usize_t
lh_gx_backend_web_get_count(const lh_gx_backend_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_gx_backend_web_set_count(lh_gx_backend_web_t *self, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_void
lh_gx_backend_web_init(lh_gx_backend_web_t *self, lh_web_json_writer_t *writer)
{
    lh_gx_backend_web_set_writer(self, writer);
    lh_gx_backend_web_set_count(self, 0U);
}

lh_void
lh_gx_backend_web_begin(lh_gx_backend_web_t *self)
{
    lh_web_json_writer_put_char(lh_gx_backend_web_get_writer_mut(self), lh_char_map_lbracket);
    lh_gx_backend_web_set_count(self, 0U);
}

lh_void
lh_gx_backend_web_end(lh_gx_backend_web_t *self)
{
    lh_web_json_writer_put_char(lh_gx_backend_web_get_writer_mut(self), lh_char_map_rbracket);
}

lh_void
lh_gx_backend_web_begin_command(lh_gx_backend_web_t *self, lh_str_view_t name)
{
    lh_web_json_writer_t *writer = lh_gx_backend_web_get_writer_mut(self);
    const lh_usize_t count = lh_gx_backend_web_get_count(self);

    if (!lh_math_is_zero(count))
    {
        lh_web_json_writer_put_char(writer, lh_char_map_comma);
    }
    lh_gx_backend_web_set_count(self, lh_math_add_one(count));
    lh_web_json_writer_put_char(writer, lh_char_map_lbracket);
    lh_web_json_writer_put_string(writer, name);
}

lh_void
lh_gx_backend_web_end_command(lh_gx_backend_web_t *self)
{
    lh_web_json_writer_put_char(lh_gx_backend_web_get_writer_mut(self), lh_char_map_rbracket);
}

lh_void
lh_gx_backend_web_put_rect(lh_gx_backend_web_t *self, const lh_gx_rect_t *rect)
{
    lh_web_json_writer_t *writer = lh_gx_backend_web_get_writer_mut(self);
    lh_math_rect_t pixels;

    lh_gx_rect_to_rect(rect, lh_addr_of(pixels));
    const lh_math_point_t *origin = lh_math_rect_get_origin(lh_addr_of(pixels));
    const lh_math_size_t *size = lh_math_rect_get_size(lh_addr_of(pixels));

    lh_web_json_writer_put_char(writer, lh_char_map_comma);
    lh_web_json_writer_put_char(writer, lh_char_map_lbracket);
    lh_web_json_writer_put_sint(writer, lh_math_point_get_x(origin));
    lh_web_json_writer_put_char(writer, lh_char_map_comma);
    lh_web_json_writer_put_sint(writer, lh_math_point_get_y(origin));
    lh_web_json_writer_put_char(writer, lh_char_map_comma);
    lh_web_json_writer_put_sint(writer, lh_math_size_get_width(size));
    lh_web_json_writer_put_char(writer, lh_char_map_comma);
    lh_web_json_writer_put_sint(writer, lh_math_size_get_height(size));
    lh_web_json_writer_put_char(writer, lh_char_map_rbracket);
}

lh_void
lh_gx_backend_web_put_uint(lh_gx_backend_web_t *self, lh_u32_t value)
{
    lh_web_json_writer_t *writer = lh_gx_backend_web_get_writer_mut(self);

    lh_web_json_writer_put_char(writer, lh_char_map_comma);
    lh_web_json_writer_put_uint(writer, value);
}

lh_void
lh_gx_backend_web_put_mask(lh_gx_backend_web_t *self, const lh_gx_pixmap_t *mask)
{
    lh_web_json_writer_t *writer = lh_gx_backend_web_get_writer_mut(self);
    const lh_usize_t width = lh_cast_static(lh_usize_t, lh_gx_pixmap_get_width(mask));

    lh_web_json_writer_put_char(writer, lh_char_map_comma);
    lh_web_json_writer_put_char(writer, lh_char_map_dquot);
    for (lh_u32_t row = 0U; row < lh_gx_pixmap_get_height(mask); ++row)
    {
        lh_web_json_writer_put_hex(writer, lh_gx_pixmap_get_address(mask, 0U, row), width);
    }
    lh_web_json_writer_put_char(writer, lh_char_map_dquot);
}

lh_void
lh_gx_backend_web_put_color(lh_gx_backend_web_t *self, lh_gx_color_t color)
{
    lh_gx_backend_web_put_uint(self, color);
}

lh_void
lh_gx_backend_web_put_gradient(lh_gx_backend_web_t *self, const lh_gx_brush_t *brush)
{
    const lh_pixel_gradient_t *gradient = lh_gx_brush_get_gradient(brush);
    lh_web_json_writer_t *writer = lh_gx_backend_web_get_writer_mut(self);

    lh_web_json_writer_put_char(writer, lh_char_map_comma);
    lh_web_json_writer_put_char(writer, lh_char_map_lbracket);
    lh_web_json_writer_put_string(writer, lh_str_view_lit("gradient"));
    lh_gx_backend_web_put_rect(self, lh_gx_brush_get_box(brush));
    lh_gx_backend_web_put_uint(self,
                               lh_cast_static(lh_u32_t, lh_pixel_gradient_get_shape(gradient)));
    lh_gx_backend_web_put_uint(self,
                               lh_cast_static(lh_u32_t, lh_pixel_gradient_get_angle(gradient)));
    for (lh_pixel_gradient_stop_count_t i = 0; i < lh_pixel_gradient_get_stops_count(gradient); ++i)
    {
        const lh_pixel_gradient_stop_t *stop = lh_pixel_gradient_get_stop(gradient, i);

        lh_gx_backend_web_put_uint(self, lh_pixel_gradient_stop_get_frac(stop));
        lh_gx_backend_web_put_color(self, lh_pixel_gradient_stop_get_color(stop));
    }
    lh_web_json_writer_put_char(writer, lh_char_map_rbracket);
}

lh_void
lh_gx_backend_web_put_brush(lh_gx_backend_web_t *self, const lh_gx_brush_t *brush)
{
    if (lh_gx_brush_is_gradient(brush))
    {
        lh_gx_backend_web_put_gradient(self, brush);
        return;
    }
    lh_gx_backend_web_put_color(self, lh_gx_brush_get_color(brush));
}

lh_void
lh_gx_backend_web_fill_rect(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_rect_t *rect,
                            const lh_gx_brush_t *brush)
{
    lh_gx_backend_web_t *self = lh_ptr_rcast(lh_gx_backend_web_t, context);

    lh_return_if(!lh_gx_rect_intersects(clip, rect));
    lh_gx_backend_web_begin_command(self, lh_str_view_lit("fill_rect"));
    lh_gx_backend_web_put_rect(self, clip);
    lh_gx_backend_web_put_rect(self, rect);
    lh_gx_backend_web_put_brush(self, brush);
    lh_gx_backend_web_end_command(self);
}

lh_void
lh_gx_backend_web_fill_mask(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_pixmap_t *mask,
                            lh_gx_point_t at, const lh_gx_brush_t *brush)
{
    lh_gx_backend_web_t *self = lh_ptr_rcast(lh_gx_backend_web_t, context);
    lh_gx_rect_t placed;

    lh_gx_rect_init(lh_addr_of(placed), lh_gx_point_get_x(lh_addr_of(at)),
                    lh_gx_point_get_y(lh_addr_of(at)),
                    lh_cast_static(lh_gx_scalar_t, lh_gx_pixmap_get_width(mask)),
                    lh_cast_static(lh_gx_scalar_t, lh_gx_pixmap_get_height(mask)));
    lh_return_if(!lh_gx_rect_intersects(clip, lh_addr_of(placed)));
    lh_gx_backend_web_begin_command(self, lh_str_view_lit("fill_mask"));
    lh_gx_backend_web_put_rect(self, clip);
    lh_gx_backend_web_put_rect(self, lh_addr_of(placed));
    lh_gx_backend_web_put_brush(self, brush);
    lh_gx_backend_web_put_mask(self, mask);
    lh_gx_backend_web_end_command(self);
}

lh_void
lh_gx_backend_web_canvas_init(lh_gx_canvas_t *table)
{
    lh_gx_canvas_init(table);
    lh_gx_canvas_set_fill_rect(table, lh_gx_backend_web_fill_rect);
    lh_gx_canvas_set_fill_mask(table, lh_gx_backend_web_fill_mask);
}
