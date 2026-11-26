/**
 * @file gradient.c
 * @brief Implementation of `lh/gl/gradient.h`.
 */

#include <lh/gl/gradient.h>

#include <lh/core/assert.h>

lh_void
lh_gl_gradient_get_box_px(const lh_gl_rect_t *rect, lh_math_rect_t *out)
{
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);

    lh_math_rect_init(out, lh_gl_scalar_floor_s32(lh_gl_point_get_x(origin)),
                      lh_gl_scalar_floor_s32(lh_gl_point_get_y(origin)),
                      lh_gl_scalar_floor_s32(lh_gl_size_get_width(size)),
                      lh_gl_scalar_floor_s32(lh_gl_size_get_height(size)));
}

lh_void
lh_gl_gradient_init_stops(lh_pixel_gradient_t *self, const lh_gl_color_t *colors,
                          lh_byte_cptr fracs, lh_pixel_gradient_stop_count_t num_stops)
{
    lh_assert_runtime_ref(colors);
    lh_u32_t argb[LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS];
    const lh_pixel_gradient_stop_count_t count =
        num_stops < LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS
            ? num_stops
            : lh_cast_static(lh_pixel_gradient_stop_count_t,
                             LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS);

    for (lh_pixel_gradient_stop_count_t i = 0; i < count; ++i)
    {
        argb[i] = lh_gl_color_get_argb(lh_addr_of(colors[i]));
    }
    lh_pixel_gradient_init_stops(self, argb, fracs, num_stops);
}

lh_void
lh_gl_gradient_color_at(const lh_pixel_gradient_t *self, lh_byte_t frac, lh_gl_color_t *out)
{
    lh_gl_color_init_argb(out, lh_pixel_gradient_get_color_at(self, frac));
}

lh_byte_t
lh_gl_gradient_frac_at(const lh_pixel_gradient_t *self, const lh_gl_rect_t *rect, lh_s32_t x,
                       lh_s32_t y)
{
    lh_math_rect_t box;

    lh_gl_gradient_get_box_px(rect, lh_addr_of(box));
    return lh_pixel_gradient_get_frac_at(self, lh_addr_of(box), x, y);
}
