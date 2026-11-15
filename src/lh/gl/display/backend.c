#include <lh/gl/display/backend.h>
#include <lh/core/assert.h>

lh_void
lh_gl_display_backend_begin(lh_ptr context)
{
    lh_gl_display_set_strip_y(lh_ptr_rcast(lh_gl_display_t, context), lh_gl_scalar(0));
}

lh_void
lh_gl_display_backend_begin_area(lh_ptr context, const lh_gl_rect_t *area)
{
    lh_assert_runtime_ref(area);
    lh_gl_display_set_strip_y(lh_ptr_rcast(lh_gl_display_t, context),
                              lh_gl_point_get_y(lh_gl_rect_get_origin(area)));
}

lh_void
lh_gl_display_backend_end(lh_ptr context, const lh_gl_rects_t *drawn)
{
    lh_gl_display_present(lh_ptr_rcast(lh_gl_display_t, context), drawn);
}

lh_void
lh_gl_display_backend_clear(lh_ptr context, const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_clear(lh_gl_display_get_sw_mut(lh_ptr_rcast(lh_gl_display_t, context)), color);
}

lh_void
lh_gl_display_backend_fill_rect(lh_ptr context, const lh_gl_rect_t *rect,
                                const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_fill_rect(lh_gl_display_get_sw_mut(lh_ptr_rcast(lh_gl_display_t, context)), rect,
                              color);
}

lh_bool_t
lh_gl_display_backend_fill_round_rect(lh_ptr context, const lh_gl_rect_t *rect,
                                      lh_gl_scalar_t radius, const lh_gl_color_t *color)
{
    return lh_gl_canvas_sw_fill_round_rect(
        lh_gl_display_get_sw_mut(lh_ptr_rcast(lh_gl_display_t, context)), rect, radius, color);
}

lh_void
lh_gl_display_backend_use_clip(lh_ptr context, const lh_gl_canvas_clip_t *clip)
{
    lh_gl_canvas_sw_use_clip(lh_gl_display_get_sw_mut(lh_ptr_rcast(lh_gl_display_t, context)), clip);
}

lh_bool_t
lh_gl_display_backend_fill_mask(lh_ptr context, const lh_gl_point_t *origin,
                                const lh_gl_mask_t *mask, const lh_gl_color_t *color)
{
    return lh_gl_canvas_sw_fill_mask(
        lh_gl_display_get_sw_mut(lh_ptr_rcast(lh_gl_display_t, context)), origin, mask, color);
}

lh_bool_t
lh_gl_display_backend_shadow(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                             const lh_gl_shadow_t *shadow)
{
    return lh_gl_canvas_sw_shadow(lh_gl_display_get_sw_mut(lh_ptr_rcast(lh_gl_display_t, context)),
                                  rect, radius, shadow);
}

lh_bool_t
lh_gl_display_backend_blur(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t blur_radius,
                           lh_u8_t *scratch, lh_usize_t bytes)
{
    return lh_gl_canvas_sw_blur(lh_gl_display_get_sw_mut(lh_ptr_rcast(lh_gl_display_t, context)),
                                rect, blur_radius, scratch, bytes);
}

lh_bool_t
lh_gl_display_backend_glass(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t corner,
                            lh_gl_scalar_t blur_radius, const lh_gl_color_t *tint,
                            lh_u8_t *scratch, lh_usize_t bytes)
{
    return lh_gl_canvas_sw_glass(lh_gl_display_get_sw_mut(lh_ptr_rcast(lh_gl_display_t, context)),
                                 rect, corner, blur_radius, tint, scratch, bytes);
}

lh_bool_t
lh_gl_display_backend_fill_colors(lh_ptr context, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                                  const lh_gl_color_t *colors, lh_gl_axis_t axis)
{
    return lh_gl_canvas_sw_fill_colors(
        lh_gl_display_get_sw_mut(lh_ptr_rcast(lh_gl_display_t, context)), x, y, w, h, colors, axis);
}

const lh_gl_canvas_backend_t lh_gl_display_backend = {
    lh_gl_display_backend_begin,
    lh_gl_display_backend_begin_area,
    lh_gl_display_backend_end,
    lh_gl_display_backend_clear,
    lh_gl_display_backend_fill_rect,
    lh_gl_display_backend_fill_round_rect,
    lh_gl_display_backend_use_clip,
    lh_gl_display_backend_fill_mask,
    lh_gl_display_backend_shadow,
    lh_gl_display_backend_blur,
    lh_gl_display_backend_glass,
    lh_gl_display_backend_fill_colors,
};

lh_void
lh_gl_display_backend_bind(lh_gl_display_t *display)
{
    lh_assert_runtime_ref(display);
    lh_gl_canvas_t *canvas = lh_gl_display_get_canvas_mut(display);
    /* init resets the canvas: carry its size and the effects' scratch over. */
    const lh_gl_size_t size = lh_ptr_deref(lh_gl_canvas_get_size(canvas));
    lh_u8_t *scratch = lh_gl_canvas_get_scratch(canvas);
    const lh_usize_t bytes = lh_gl_canvas_get_scratch_bytes(canvas);
    lh_gl_canvas_init(canvas, lh_addr_of(lh_gl_display_backend), display);
    lh_gl_canvas_set_size(canvas, size);
    lh_gl_canvas_use_scratch(canvas, scratch, bytes);
}
