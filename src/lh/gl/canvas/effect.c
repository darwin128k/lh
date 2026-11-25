/**
 * @file effect.c
 * @brief Implementation of `lh/gl/canvas.h`: the effects: a shadow, a blur, glass.
 */

#include <lh/gl/canvas.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/gl/radius.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_canvas_shadow_fallback(lh_gl_canvas_t *self, const lh_gl_rect_t *bounds,
                             const lh_gl_rect_t *target, lh_gl_scalar_t radius,
                             const lh_gl_shadow_t *shadow)
{
    lh_gl_rect_t cut;

    /* Only what the clip lets through is worth a pixel each, and a frame drawn in
       strips clips it once per strip. */
    lh_gl_canvas_state_cut(lh_gl_canvas_get_state_mut(self), bounds, lh_addr_of(cut));
    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(cut)));
    const lh_gl_point_t *cut_origin = lh_gl_rect_get_origin(lh_addr_of(cut));
    const lh_gl_size_t *cut_size = lh_gl_rect_get_size(lh_addr_of(cut));
    lh_s32_t x1 = lh_gl_point_get_x(cut_origin) + lh_gl_size_get_width(cut_size);
    lh_s32_t y1 = lh_gl_point_get_y(cut_origin) + lh_gl_size_get_height(cut_size);
    lh_pixel_shadow_t shape;

    lh_gl_shadow_get_pixel_shadow(shadow, target, radius, lh_addr_of(shape));
    for (lh_s32_t y = lh_gl_point_get_y(cut_origin); y < y1; ++y)
    {
        const lh_s32_t row = lh_pixel_shadow_get_row(lh_addr_of(shape), y);

        for (lh_s32_t x = lh_gl_point_get_x(cut_origin); x < x1; ++x)
        {
            const lh_byte_t alpha = lh_pixel_shadow_get_alpha(lh_addr_of(shape), row, x, y);
            lh_gl_rect_t pixel;
            lh_gl_color_t color;

            if (alpha == 0)
            {
                continue;
            }
            lh_gl_shadow_copy_color(shadow, lh_addr_of(color));
            lh_gl_color_set_a(lh_addr_of(color), alpha);
            lh_gl_rect_init(lh_addr_of(pixel), lh_gl_scalar_of(x), lh_gl_scalar_of(y),
                            lh_gl_scalar(1), lh_gl_scalar(1));
            lh_gl_canvas_fill_target_rect(self, lh_addr_of(pixel), lh_addr_of(color));
        }
    }
}

lh_bool_t
lh_gl_canvas_blur(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, lh_gl_scalar_t blur_radius)
{
    lh_assert_runtime_ref(rect);
    lh_return_if(blur_radius <= 0, lh_bool_false);
    lh_return_if(lh_null_eq(lh_gl_canvas_get_backend(self)) ||
                     lh_null_eq(lh_gl_canvas_backend_get_blur_fn(lh_gl_canvas_get_backend(self))),
                 lh_bool_false);
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state_mut(self), rect, lh_addr_of(target));
    lh_return_if(!lh_gl_canvas_shows(self, lh_addr_of(target)), lh_bool_false);
    /* A blur reads the pixels it is about to blur, so unlike a shadow it cannot
       be cut to the area: in a frame drawn in strips the pixels across the line
       are not in the buffer at all, and blurring what is there would leave a
       visible seam every strip. It says it cannot be drawn instead. */
    lh_return_if(!lh_gl_canvas_can_send_whole(self, lh_addr_of(target)), lh_bool_false);
    lh_gl_canvas_add_damage(self, lh_addr_of(target));
    return lh_gl_canvas_backend_get_blur_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self), lh_addr_of(target), blur_radius,
        lh_gl_canvas_get_scratch(self), lh_gl_canvas_get_scratch_bytes(self));
}

lh_bool_t
lh_gl_canvas_glass(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, lh_gl_scalar_t corner,
                   lh_gl_scalar_t blur_radius, const lh_gl_color_t *tint)
{
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(tint);
    lh_return_if(lh_null_eq(lh_gl_canvas_get_backend(self)) ||
                     lh_null_eq(lh_gl_canvas_backend_get_glass_fn(lh_gl_canvas_get_backend(self))),
                 lh_bool_false);
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state_mut(self), rect, lh_addr_of(target));
    lh_return_if(!lh_gl_canvas_shows(self, lh_addr_of(target)), lh_bool_false);
    /* Glass is a blur plus a tint, so it needs the whole rect in the buffer for
       the same reason, and the tint is not put down without it: a tint on its own
       is a fill, and a fill that only looks like glass is worse than none. */
    lh_return_if(!lh_gl_canvas_can_send_whole(self, lh_addr_of(target)), lh_bool_false);
    lh_gl_canvas_add_damage(self, lh_addr_of(target));
    return lh_gl_canvas_backend_get_glass_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self), lh_addr_of(target),
        lh_gl_radius_clamp(lh_addr_of(target), corner), blur_radius, tint,
        lh_gl_canvas_get_scratch(self), lh_gl_canvas_get_scratch_bytes(self));
}

lh_bool_t
lh_gl_canvas_shadow(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                    const lh_gl_shadow_t *shadow)
{
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(shadow);
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state_mut(self), rect, lh_addr_of(target));
    lh_gl_scalar_t outset = lh_gl_shadow_get_outset(shadow, lh_addr_of(target));
    lh_return_if(outset <= 0, lh_bool_false);
    lh_gl_scalar_t corner = lh_gl_radius_clamp(lh_addr_of(target), radius);
    lh_gl_rect_t bounds;
    lh_gl_rect_inset(lh_addr_of(target), lh_gl_scalar_of(lh_math_neg(outset)),
                     lh_gl_scalar_of(lh_math_neg(outset)), lh_addr_of(bounds));
    lh_gl_canvas_add_damage(self, lh_addr_of(bounds));
    /* No `can_send_whole` here, unlike a rounded fill: a shadow reads no pixels, it
       is the box and the shadow alone, so every area that clips it draws its own
       slice and the picture is the same whether the frame is drawn whole or in
       strips. The effect that does need the whole area in one buffer is the blur. */
    lh_return_if(!lh_gl_canvas_shows(self, lh_addr_of(bounds)), lh_bool_false);

    if (!lh_null_eq(lh_gl_canvas_get_backend(self)) &&
        !lh_null_eq(lh_gl_canvas_backend_get_shadow_fn(lh_gl_canvas_get_backend(self))) &&
        lh_gl_canvas_backend_get_shadow_fn(lh_gl_canvas_get_backend(self))(
            lh_gl_canvas_get_context(self), lh_addr_of(target), corner, shadow))
    {
        return lh_bool_true;
    }
    lh_gl_canvas_shadow_fallback(self, lh_addr_of(bounds), lh_addr_of(target), corner, shadow);
    return lh_bool_true;
}
