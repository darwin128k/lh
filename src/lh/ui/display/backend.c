#include <lh/ui/display/backend.h>
#include <lh/core/assert.h>

lh_void
lh_ui_display_backend_begin(lh_ptr context)
{
    lh_ui_display_set_strip_y(lh_ptr_rcast(lh_ui_display_t, context), lh_ui_scalar(0));
}

lh_void
lh_ui_display_backend_begin_area(lh_ptr context, const lh_ui_rect_t *area)
{
    lh_assert_runtime_ref(area);
    lh_ui_display_set_strip_y(lh_ptr_rcast(lh_ui_display_t, context),
                              lh_ui_point_get_y(lh_ui_rect_get_origin(area)));
}

lh_void
lh_ui_display_backend_end(lh_ptr context, const lh_ui_rects_t *drawn)
{
    lh_ui_display_present(lh_ptr_rcast(lh_ui_display_t, context), drawn);
}

lh_void
lh_ui_display_backend_clear(lh_ptr context, const lh_ui_color_t *color)
{
    lh_ui_canvas_sw_clear(lh_ui_display_get_sw_mut(lh_ptr_rcast(lh_ui_display_t, context)), color);
}

lh_void
lh_ui_display_backend_fill_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                const lh_ui_color_t *color)
{
    lh_ui_canvas_sw_fill_rect(lh_ui_display_get_sw_mut(lh_ptr_rcast(lh_ui_display_t, context)), rect,
                              color);
}

lh_bool_t
lh_ui_display_backend_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                      lh_ui_scalar_t radius, const lh_ui_color_t *color)
{
    return lh_ui_canvas_sw_fill_round_rect(
        lh_ui_display_get_sw_mut(lh_ptr_rcast(lh_ui_display_t, context)), rect, radius, color);
}

lh_void
lh_ui_display_backend_use_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip)
{
    lh_ui_canvas_sw_use_clip(lh_ui_display_get_sw_mut(lh_ptr_rcast(lh_ui_display_t, context)), clip);
}

lh_bool_t
lh_ui_display_backend_fill_mask(lh_ptr context, const lh_ui_point_t *origin,
                                const lh_ui_mask_t *mask, const lh_ui_color_t *color)
{
    return lh_ui_canvas_sw_fill_mask(
        lh_ui_display_get_sw_mut(lh_ptr_rcast(lh_ui_display_t, context)), origin, mask, color);
}

lh_bool_t
lh_ui_display_backend_shadow(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                             const lh_ui_shadow_t *shadow)
{
    return lh_ui_canvas_sw_shadow(lh_ui_display_get_sw_mut(lh_ptr_rcast(lh_ui_display_t, context)),
                                  rect, radius, shadow);
}

lh_bool_t
lh_ui_display_backend_blur(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius,
                           lh_u8_t *scratch, lh_usize_t bytes)
{
    return lh_ui_canvas_sw_blur(lh_ui_display_get_sw_mut(lh_ptr_rcast(lh_ui_display_t, context)),
                                rect, blur_radius, scratch, bytes);
}

lh_bool_t
lh_ui_display_backend_glass(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t corner,
                            lh_ui_scalar_t blur_radius, const lh_ui_color_t *tint,
                            lh_u8_t *scratch, lh_usize_t bytes)
{
    return lh_ui_canvas_sw_glass(lh_ui_display_get_sw_mut(lh_ptr_rcast(lh_ui_display_t, context)),
                                 rect, corner, blur_radius, tint, scratch, bytes);
}

lh_bool_t
lh_ui_display_backend_fill_colors(lh_ptr context, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                                  const lh_ui_color_t *colors, lh_ui_axis_t axis)
{
    return lh_ui_canvas_sw_fill_colors(
        lh_ui_display_get_sw_mut(lh_ptr_rcast(lh_ui_display_t, context)), x, y, w, h, colors, axis);
}

const lh_ui_canvas_backend_t lh_ui_display_backend = {
    lh_ui_display_backend_begin,
    lh_ui_display_backend_begin_area,
    lh_ui_display_backend_end,
    lh_ui_display_backend_clear,
    lh_ui_display_backend_fill_rect,
    lh_ui_display_backend_fill_round_rect,
    lh_ui_display_backend_use_clip,
    lh_ui_display_backend_fill_mask,
    lh_ui_display_backend_shadow,
    lh_ui_display_backend_blur,
    lh_ui_display_backend_glass,
    lh_ui_display_backend_fill_colors,
};

lh_void
lh_ui_display_backend_bind(lh_ui_display_t *display)
{
    lh_assert_runtime_ref(display);
    lh_ui_canvas_t *canvas = lh_ui_display_get_canvas_mut(display);
    /* init resets the canvas: carry its size and the effects' scratch over. */
    const lh_ui_size_t size = lh_ptr_deref(lh_ui_canvas_get_size(canvas));
    lh_u8_t *scratch = lh_ui_canvas_get_scratch(canvas);
    const lh_usize_t bytes = lh_ui_canvas_get_scratch_bytes(canvas);
    lh_ui_canvas_init(canvas, lh_addr_of(lh_ui_display_backend), display);
    lh_ui_canvas_set_size(canvas, size);
    lh_ui_canvas_use_scratch(canvas, scratch, bytes);
}
