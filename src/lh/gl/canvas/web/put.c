/**
 * @file put.c
 * @brief Implementation of `lh/gl/canvas/web.h`: writing a frame's JSON: numbers, hex, an op, a
 * colour, a rect, the head, a cover.
 */

#include <lh/gl/canvas/web.h>

#include <lh/core/math.h>
#include <lh/core/str/format/bytes.h>
#include <lh/core/str/format/sint.h>

lh_void
lh_gl_canvas_web_put_int(lh_gl_canvas_web_t *self, lh_sint_t v)
{
    lh_char_t buf[24];

    lh_gl_web_buf_lit(lh_gl_canvas_web_get_buf_mut(self), buf,
                      lh_str_ptr_format_sint(v, buf, sizeof(buf)));
}

lh_void
lh_gl_canvas_web_put_hex(lh_gl_canvas_web_t *self, lh_byte_cptr px, lh_usize_t n)
{
    lh_char_t buf[256];

    for (lh_usize_t i = 0; lh_math_lt(i, n); i = lh_math_add(i, lh_math_div(sizeof(buf), 2U)))
    {
        const lh_usize_t chunk = lh_math_min(lh_math_sub(n, i), lh_math_div(sizeof(buf), 2U));
        const lh_usize_t len =
            lh_str_ptr_format_bytes_hex(px + i, chunk, lh_bool_false, buf, sizeof(buf));

        lh_gl_web_buf_lit(lh_gl_canvas_web_get_buf_mut(self), buf, len);
    }
}

lh_void
lh_gl_canvas_web_begin_op(lh_gl_canvas_web_t *self)
{
    if (!lh_gl_web_buf_ok(lh_gl_canvas_web_get_buf_mut(self)))
    {
        return;
    }
    if (lh_gl_canvas_web_is_comma(self))
    {
        lh_gl_canvas_web_put_lit(self, ",");
    }
    lh_gl_canvas_web_set_comma(self, lh_bool_true);
    lh_gl_canvas_web_set_ops(self, lh_gl_canvas_web_get_ops(self) + 1);
}

lh_void
lh_gl_canvas_web_put_quad(lh_gl_canvas_web_t *self, lh_sint_t a, lh_sint_t b, lh_sint_t c,
                          lh_sint_t d)
{
    lh_gl_canvas_web_put_lit(self, "[");
    lh_gl_canvas_web_put_int(self, a);
    lh_gl_canvas_web_put_lit(self, ",");
    lh_gl_canvas_web_put_int(self, b);
    lh_gl_canvas_web_put_lit(self, ",");
    lh_gl_canvas_web_put_int(self, c);
    lh_gl_canvas_web_put_lit(self, ",");
    lh_gl_canvas_web_put_int(self, d);
    lh_gl_canvas_web_put_lit(self, "]");
}

lh_void
lh_gl_canvas_web_put_head(lh_gl_canvas_web_t *self)
{
    lh_gl_canvas_web_put_lit(self, "{\"kind\":\"lh_gl_canvas\",\"width\":");
    lh_gl_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_gl_canvas_web_get_width(self)));
    lh_gl_canvas_web_put_lit(self, ",\"height\":");
    lh_gl_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_gl_canvas_web_get_height(self)));
    lh_gl_canvas_web_put_lit(self, ",\"ops\":[");
}

lh_void
lh_gl_canvas_web_put_cover(lh_gl_canvas_web_t *self, const lh_gl_mask_t *mask, lh_s32_t w,
                           lh_s32_t h)
{
    for (lh_s32_t y = 0; y < h; ++y)
    {
        for (lh_s32_t x0 = 0; x0 < w; x0 += LH_GL_CANVAS_WEB_ATLAS_PIX)
        {
            const lh_s32_t x1 =
                (w - x0 > LH_GL_CANVAS_WEB_ATLAS_PIX) ? x0 + LH_GL_CANVAS_WEB_ATLAS_PIX : w;
            lh_byte_t row[LH_GL_CANVAS_WEB_ATLAS_PIX];

            lh_gl_mask_coverage_run(mask, x0, x1, y, row);
            lh_gl_canvas_web_put_hex(self, row, lh_cast_static(lh_usize_t, x1 - x0));
        }
    }
}

lh_void
lh_gl_canvas_web_put_color(lh_gl_canvas_web_t *self, const lh_gl_color_t *color)
{
    lh_gl_canvas_web_put_quad(self, lh_cast_static(lh_sint_t, lh_gl_color_get_r(color)),
                              lh_cast_static(lh_sint_t, lh_gl_color_get_g(color)),
                              lh_cast_static(lh_sint_t, lh_gl_color_get_b(color)),
                              lh_cast_static(lh_sint_t, lh_gl_color_get_a(color)));
}

lh_void
lh_gl_canvas_web_put_rect(lh_gl_canvas_web_t *self, const lh_gl_rect_t *rect)
{
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);

    lh_gl_canvas_web_put_quad(self, lh_cast_static(lh_sint_t, lh_gl_point_get_x(origin)),
                              lh_cast_static(lh_sint_t, lh_gl_point_get_y(origin)),
                              lh_cast_static(lh_sint_t, lh_gl_size_get_width(size)),
                              lh_cast_static(lh_sint_t, lh_gl_size_get_height(size)));
}
