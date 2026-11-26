/**
 * @file draw.c
 * @brief Implementation of `lh/gl/canvas/web.h`: the slots the backend table points at: clear, the
 * fills, the clip, the shadow, the mask.
 */

#include <lh/gl/canvas/web.h>

#include <lh/core/attribute/unused.h>

lh_void
lh_gl_canvas_web_clear(lh_ptr context, const lh_gl_color_t *color)
{
    lh_gl_canvas_web_t *self = lh_ptr_rcast(lh_gl_canvas_web_t, context);

    lh_gl_canvas_web_begin_op(self);
    lh_gl_canvas_web_put_lit(self, "{\"op\":\"clear\",\"color\":");
    lh_gl_canvas_web_put_color(self, color);
    lh_gl_canvas_web_put_lit(self, "}");
}

lh_void
lh_gl_canvas_web_fill_rect(lh_ptr context, const lh_gl_rect_t *rect, const lh_gl_color_t *color)
{
    lh_gl_canvas_web_t *self = lh_ptr_rcast(lh_gl_canvas_web_t, context);

    lh_gl_canvas_web_begin_op(self);
    lh_gl_canvas_web_put_lit(self, "{\"op\":\"fill_rect\",\"rect\":");
    lh_gl_canvas_web_put_rect(self, rect);
    lh_gl_canvas_web_put_lit(self, ",\"color\":");
    lh_gl_canvas_web_put_color(self, color);
    lh_gl_canvas_web_put_lit(self, "}");
}

lh_bool_t
lh_gl_canvas_web_fill_round_rect(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                                 const lh_gl_color_t *color)
{
    lh_gl_canvas_web_t *self = lh_ptr_rcast(lh_gl_canvas_web_t, context);

    lh_gl_canvas_web_begin_op(self);
    lh_gl_canvas_web_put_lit(self, "{\"op\":\"fill_round_rect\",\"rect\":");
    lh_gl_canvas_web_put_rect(self, rect);
    lh_gl_canvas_web_put_lit(self, ",\"radius\":");
    lh_gl_canvas_web_put_int(self, lh_cast_static(lh_sint_t, radius));
    lh_gl_canvas_web_put_lit(self, ",\"color\":");
    lh_gl_canvas_web_put_color(self, color);
    lh_gl_canvas_web_put_lit(self, "}");
    return lh_bool_true;
}

lh_void
lh_gl_canvas_web_set_clip(lh_ptr context, const lh_gl_canvas_clip_t *clip)
{
    LH_ATTRIBUTE_UNUSED(context);
    LH_ATTRIBUTE_UNUSED(clip);
}

lh_bool_t
lh_gl_canvas_web_shadow(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                        const lh_gl_shadow_t *shadow)
{
    lh_gl_canvas_web_t *self = lh_ptr_rcast(lh_gl_canvas_web_t, context);
    lh_gl_color_t color;
    lh_gl_shadow_copy_color(shadow, lh_addr_of(color));

    lh_gl_canvas_web_begin_op(self);
    lh_gl_canvas_web_put_lit(self, "{\"op\":\"shadow\",\"rect\":");
    lh_gl_canvas_web_put_rect(self, rect);
    lh_gl_canvas_web_put_lit(self, ",\"radius\":");
    lh_gl_canvas_web_put_int(self, lh_cast_static(lh_sint_t, radius));
    lh_gl_canvas_web_put_lit(self, ",\"spread\":");
    lh_gl_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_gl_shadow_get_spread(shadow)));
    lh_gl_canvas_web_put_lit(self, ",\"offset\":[");
    lh_gl_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_gl_shadow_get_offset_x(shadow)));
    lh_gl_canvas_web_put_lit(self, ",");
    lh_gl_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_gl_shadow_get_offset_y(shadow)));
    lh_gl_canvas_web_put_lit(self, "],\"color\":");
    lh_gl_canvas_web_put_color(self, lh_addr_of(color));
    lh_gl_canvas_web_put_lit(self, "}");
    return lh_bool_true;
}

lh_bool_t
lh_gl_canvas_web_fill_mask(lh_ptr context, const lh_gl_point_t *origin, const lh_gl_mask_t *mask,
                           const lh_gl_color_t *color)
{
    lh_gl_canvas_web_t *self = lh_ptr_rcast(lh_gl_canvas_web_t, context);
    const lh_s32_t w = lh_gl_mask_get_width(mask);
    const lh_s32_t h = lh_gl_mask_get_height(mask);
    const lh_uint_t n =
        (w > 0 && h > 0) ? lh_cast_static(lh_uint_t, w) * lh_cast_static(lh_uint_t, h) : 0u;
    lh_bool_t is_new = lh_bool_true;
    lh_sint_t id = -1;

    lh_gl_canvas_web_begin_op(self);
    lh_gl_canvas_web_put_lit(self, "{\"op\":\"fill_mask\",\"origin\":[");
    lh_gl_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_gl_point_get_x(origin)));
    lh_gl_canvas_web_put_lit(self, ",");
    lh_gl_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_gl_point_get_y(origin)));
    lh_gl_canvas_web_put_lit(self, "],\"size\":[");
    lh_gl_canvas_web_put_int(self, w);
    lh_gl_canvas_web_put_lit(self, ",");
    lh_gl_canvas_web_put_int(self, h);
    lh_gl_canvas_web_put_lit(self, "],\"color\":");
    lh_gl_canvas_web_put_color(self, color);
    if (n == 0u || n > lh_cast_static(lh_uint_t, LH_GL_CANVAS_WEB_ATLAS_PIX))
    {
        lh_gl_canvas_web_put_lit(self, ",\"cover\":\"");
        if (n != 0u)
        {
            lh_gl_canvas_web_put_cover(self, mask, w, h);
        }
        lh_gl_canvas_web_put_lit(self, "\"}");
        return lh_bool_true;
    }
    lh_byte_t px[LH_GL_CANVAS_WEB_ATLAS_PIX];
    for (lh_s32_t y = 0; y < h; ++y)
    {
        lh_gl_mask_coverage_run(mask, 0, w, y,
                                px + lh_cast_static(lh_uint_t, y) * lh_cast_static(lh_uint_t, w));
    }
    id = lh_gl_canvas_web_intern(self, px, w, h, lh_addr_of(is_new));
    if (id >= 0)
    {
        lh_gl_canvas_web_put_lit(self, ",\"m\":");
        lh_gl_canvas_web_put_int(self, id);
    }
    if (id < 0 || is_new)
    {
        lh_gl_canvas_web_put_lit(self, ",\"cover\":\"");
        lh_gl_canvas_web_put_hex(self, px, n);
        lh_gl_canvas_web_put_lit(self, "\"");
    }
    lh_gl_canvas_web_put_lit(self, "}");
    return lh_bool_true;
}
