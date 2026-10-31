/**
 * @file web.c
 * @brief Implementation of `lh/ui/canvas/web.h`.
 */
#include <lh/assert.h>

#include <lh/ui/canvas/web.h>

#include <lh/cast/const.h>
#include <lh/hash/fnv.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/runtime/allocator.h>
#include <lh/str/format/bytes.h>
#include <lh/str/format/sint.h>

lh_void
lh_ui_canvas_web_put_int(lh_ui_canvas_web_t *self, lh_sint_t v)
{
    lh_char_t buf[24];

    lh_ui_web_buf_lit(lh_ui_canvas_web_get_buf_mut(self), buf,
                      lh_str_ptr_format_sint(v, buf, sizeof(buf)));
}

lh_void
lh_ui_canvas_web_put_quad(lh_ui_canvas_web_t *self, lh_sint_t a, lh_sint_t b, lh_sint_t c,
                          lh_sint_t d)
{
    lh_ui_canvas_web_put_lit(self, "[");
    lh_ui_canvas_web_put_int(self, a);
    lh_ui_canvas_web_put_lit(self, ",");
    lh_ui_canvas_web_put_int(self, b);
    lh_ui_canvas_web_put_lit(self, ",");
    lh_ui_canvas_web_put_int(self, c);
    lh_ui_canvas_web_put_lit(self, ",");
    lh_ui_canvas_web_put_int(self, d);
    lh_ui_canvas_web_put_lit(self, "]");
}

lh_void
lh_ui_canvas_web_put_hex(lh_ui_canvas_web_t *self, lh_byte_cptr px, lh_usize_t n)
{
    lh_char_t buf[256];

    for (lh_usize_t i = 0; lh_math_lt(i, n); i = lh_math_add(i, lh_math_div(sizeof(buf), 2U)))
    {
        const lh_usize_t chunk = lh_math_min(lh_math_sub(n, i), lh_math_div(sizeof(buf), 2U));
        const lh_usize_t len =
            lh_str_ptr_format_bytes_hex(px + i, chunk, lh_bool_false, buf, sizeof(buf));

        lh_ui_web_buf_lit(lh_ui_canvas_web_get_buf_mut(self), buf, len);
    }
}

lh_void
lh_ui_canvas_web_begin_op(lh_ui_canvas_web_t *self)
{
    if (!lh_ui_web_buf_ok(lh_ui_canvas_web_get_buf_mut(self)))
    {
        return;
    }
    if (lh_ui_canvas_web_is_comma(self))
    {
        lh_ui_canvas_web_put_lit(self, ",");
    }
    lh_ui_canvas_web_set_comma(self, lh_bool_true);
    lh_ui_canvas_web_set_ops(self, lh_ui_canvas_web_get_ops(self) + 1);
}

lh_void
lh_ui_canvas_web_put_color(lh_ui_canvas_web_t *self, const lh_ui_color_t *color)
{
    lh_ui_canvas_web_put_quad(self, lh_cast_static(lh_sint_t, lh_ui_color_get_r(color)),
                              lh_cast_static(lh_sint_t, lh_ui_color_get_g(color)),
                              lh_cast_static(lh_sint_t, lh_ui_color_get_b(color)),
                              lh_cast_static(lh_sint_t, lh_ui_color_get_a(color)));
}

lh_void
lh_ui_canvas_web_put_rect(lh_ui_canvas_web_t *self, const lh_ui_rect_t *rect)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size(rect);

    lh_ui_canvas_web_put_quad(self, lh_cast_static(lh_sint_t, lh_ui_point_get_x(origin)),
                              lh_cast_static(lh_sint_t, lh_ui_point_get_y(origin)),
                              lh_cast_static(lh_sint_t, lh_ui_size_get_width(size)),
                              lh_cast_static(lh_sint_t, lh_ui_size_get_height(size)));
}

/** Cover identity is process-wide so `m` does not reshuffle when the tree
 * changes. `self->glyphs` is only which ids the reader already holds. */
static lh_ui_canvas_web_atlas_t g_web_atlas;

lh_ui_canvas_web_glyphs_t *
lh_ui_canvas_web_get_glyphs_v(lh_ui_canvas_web_t *self)
{
    if (lh_ui_canvas_web_get_glyphs(self) == lh_null)
    {
        lh_ui_canvas_web_set_glyphs(
            self, lh_ptr_rcast(lh_ui_canvas_web_glyphs_t,
                               lh_runtime_allocator_alloc(
                                   sizeof(lh_ptr_deref(lh_ui_canvas_web_get_glyphs(self))))));
        lh_ui_canvas_web_set_glyphs_owned(
            self, (lh_ui_canvas_web_get_glyphs(self) != lh_null) ? lh_bool_true : lh_bool_false);
        lh_ui_canvas_web_glyphs_reset(lh_ui_canvas_web_get_glyphs(self));
    }
    return lh_ui_canvas_web_get_glyphs(self);
}

lh_void
lh_ui_canvas_web_free_glyphs(lh_ui_canvas_web_t *self)
{
    if (lh_ui_canvas_web_is_glyphs_owned(self))
    {
        lh_runtime_allocator_free(lh_ui_canvas_web_get_glyphs(self));
    }
    lh_ui_canvas_web_set_glyphs(self, lh_null);
    lh_ui_canvas_web_set_glyphs_owned(self, lh_bool_false);
}

lh_sint_t
lh_ui_canvas_web_intern(lh_ui_canvas_web_t *self, lh_byte_cptr px, lh_s32_t w, lh_s32_t h,
                        lh_bool_t *is_new)
{
    const lh_uint_t n = lh_cast_static(lh_uint_t, w) * lh_cast_static(lh_uint_t, h);
    /* The size seeds the hash: two covers of the same bytes in another shape differ. */
    const lh_u32_t seed = lh_bit_xor(
        LH_HASH_FNV1A_32_OFFSET,
        lh_math_add(lh_math_mul(lh_cast_static(lh_u32_t, w), 131U), lh_cast_static(lh_u32_t, h)));
    const lh_u32_t hash = lh_hash_fnv1a_32_update(seed, px, n);
    lh_ui_canvas_web_atlas_t *atlas = lh_addr_of(g_web_atlas);
    lh_ui_canvas_web_glyphs_t *glyphs;
    lh_uint_t slot = 0;
    lh_sint_t id;

    lh_ptr_deref(is_new) = lh_bool_true;
    id = lh_ui_canvas_web_atlas_find(atlas, px, n, w, h, hash, lh_addr_of(slot));
    if (id < 0)
    {
        id = lh_ui_canvas_web_atlas_add(atlas, px, n, w, h, hash, slot);
        if (id < 0)
        {
            return -1;
        }
    }
    glyphs = lh_ui_canvas_web_get_glyphs_v(self);
    if (glyphs == lh_null)
    {
        return id;
    }
    if (lh_ui_canvas_web_glyphs_get_sent(glyphs)[id] != 0u)
    {
        lh_ptr_deref(is_new) = lh_bool_false;
    }
    lh_ui_canvas_web_glyphs_get_sent_mut(glyphs)[id] = 1u;
    return id;
}

lh_void
lh_ui_canvas_web_reset(lh_ui_canvas_web_t *self, lh_uint_t width, lh_uint_t height)
{
    lh_ui_web_buf_init(lh_ui_canvas_web_get_buf_mut(self), lh_null, 0);
    lh_ui_canvas_web_set_width(self, width);
    lh_ui_canvas_web_set_height(self, height);
    lh_ui_canvas_web_set_ops(self, 0);
    lh_ui_canvas_web_set_comma(self, lh_bool_false);
    lh_ui_canvas_web_set_glyphs(self, lh_null);
    lh_ui_canvas_web_set_glyphs_owned(self, lh_bool_false);
}

lh_void
lh_ui_canvas_web_put_head(lh_ui_canvas_web_t *self)
{
    lh_ui_canvas_web_put_lit(self, "{\"kind\":\"lh_ui_canvas\",\"width\":");
    lh_ui_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_ui_canvas_web_get_width(self)));
    lh_ui_canvas_web_put_lit(self, ",\"height\":");
    lh_ui_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_ui_canvas_web_get_height(self)));
    lh_ui_canvas_web_put_lit(self, ",\"ops\":[");
}

lh_void
lh_ui_canvas_web_clear(lh_ptr context, const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);

    lh_ui_canvas_web_begin_op(self);
    lh_ui_canvas_web_put_lit(self, "{\"op\":\"clear\",\"color\":");
    lh_ui_canvas_web_put_color(self, color);
    lh_ui_canvas_web_put_lit(self, "}");
}

lh_void
lh_ui_canvas_web_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);

    lh_ui_canvas_web_begin_op(self);
    lh_ui_canvas_web_put_lit(self, "{\"op\":\"fill_rect\",\"rect\":");
    lh_ui_canvas_web_put_rect(self, rect);
    lh_ui_canvas_web_put_lit(self, ",\"color\":");
    lh_ui_canvas_web_put_color(self, color);
    lh_ui_canvas_web_put_lit(self, "}");
}

lh_bool_t
lh_ui_canvas_web_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                 const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);

    lh_ui_canvas_web_begin_op(self);
    lh_ui_canvas_web_put_lit(self, "{\"op\":\"fill_round_rect\",\"rect\":");
    lh_ui_canvas_web_put_rect(self, rect);
    lh_ui_canvas_web_put_lit(self, ",\"radius\":");
    lh_ui_canvas_web_put_int(self, lh_cast_static(lh_sint_t, radius));
    lh_ui_canvas_web_put_lit(self, ",\"color\":");
    lh_ui_canvas_web_put_color(self, color);
    lh_ui_canvas_web_put_lit(self, "}");
    return lh_bool_true;
}

lh_void
lh_ui_canvas_web_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip)
{
    (void)context;
    (void)clip;
}

lh_bool_t
lh_ui_canvas_web_shadow(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                        const lh_ui_shadow_t *shadow)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);
    lh_ui_color_t color;
    lh_ui_shadow_copy_color(shadow, lh_addr_of(color));

    lh_ui_canvas_web_begin_op(self);
    lh_ui_canvas_web_put_lit(self, "{\"op\":\"shadow\",\"rect\":");
    lh_ui_canvas_web_put_rect(self, rect);
    lh_ui_canvas_web_put_lit(self, ",\"radius\":");
    lh_ui_canvas_web_put_int(self, lh_cast_static(lh_sint_t, radius));
    lh_ui_canvas_web_put_lit(self, ",\"spread\":");
    lh_ui_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_ui_shadow_get_spread(shadow)));
    lh_ui_canvas_web_put_lit(self, ",\"offset\":[");
    lh_ui_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_ui_shadow_get_offset_x(shadow)));
    lh_ui_canvas_web_put_lit(self, ",");
    lh_ui_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_ui_shadow_get_offset_y(shadow)));
    lh_ui_canvas_web_put_lit(self, "],\"color\":");
    lh_ui_canvas_web_put_color(self, lh_addr_of(color));
    lh_ui_canvas_web_put_lit(self, "}");
    return lh_bool_true;
}

lh_void
lh_ui_canvas_web_put_cover(lh_ui_canvas_web_t *self, const lh_ui_mask_t *mask, lh_s32_t w,
                           lh_s32_t h)
{

    for (lh_s32_t y = 0; y < h; ++y)
    {
        for (lh_s32_t x0 = 0; x0 < w; x0 += LH_UI_CANVAS_WEB_ATLAS_PIX)
        {
            const lh_s32_t x1 =
                (w - x0 > LH_UI_CANVAS_WEB_ATLAS_PIX) ? x0 + LH_UI_CANVAS_WEB_ATLAS_PIX : w;
            lh_byte_t row[LH_UI_CANVAS_WEB_ATLAS_PIX];

            lh_ui_mask_coverage_run(mask, x0, x1, y, row);
            lh_ui_canvas_web_put_hex(self, row, lh_cast_static(lh_usize_t, x1 - x0));
        }
    }
}

lh_bool_t
lh_ui_canvas_web_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                           const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);
    const lh_s32_t w = lh_ui_mask_get_width(mask);
    const lh_s32_t h = lh_ui_mask_get_height(mask);
    const lh_uint_t n =
        (w > 0 && h > 0) ? lh_cast_static(lh_uint_t, w) * lh_cast_static(lh_uint_t, h) : 0u;
    lh_byte_t px[LH_UI_CANVAS_WEB_ATLAS_PIX];
    lh_bool_t is_new = lh_bool_true;
    lh_sint_t id = -1;
    lh_s32_t y;

    lh_ui_canvas_web_begin_op(self);
    lh_ui_canvas_web_put_lit(self, "{\"op\":\"fill_mask\",\"origin\":[");
    lh_ui_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_ui_point_get_x(origin)));
    lh_ui_canvas_web_put_lit(self, ",");
    lh_ui_canvas_web_put_int(self, lh_cast_static(lh_sint_t, lh_ui_point_get_y(origin)));
    lh_ui_canvas_web_put_lit(self, "],\"size\":[");
    lh_ui_canvas_web_put_int(self, w);
    lh_ui_canvas_web_put_lit(self, ",");
    lh_ui_canvas_web_put_int(self, h);
    lh_ui_canvas_web_put_lit(self, "],\"color\":");
    lh_ui_canvas_web_put_color(self, color);
    if (n == 0u || n > lh_cast_static(lh_uint_t, LH_UI_CANVAS_WEB_ATLAS_PIX))
    {
        lh_ui_canvas_web_put_lit(self, ",\"cover\":\"");
        if (n != 0u)
        {
            lh_ui_canvas_web_put_cover(self, mask, w, h);
        }
        lh_ui_canvas_web_put_lit(self, "\"}");
        return lh_bool_true;
    }
    for (y = 0; y < h; ++y)
    {
        lh_ui_mask_coverage_run(mask, 0, w, y,
                                px + lh_cast_static(lh_uint_t, y) * lh_cast_static(lh_uint_t, w));
    }
    id = lh_ui_canvas_web_intern(self, px, w, h, lh_addr_of(is_new));
    if (id >= 0)
    {
        lh_ui_canvas_web_put_lit(self, ",\"m\":");
        lh_ui_canvas_web_put_int(self, id);
    }
    if (id < 0 || is_new)
    {
        lh_ui_canvas_web_put_lit(self, ",\"cover\":\"");
        lh_ui_canvas_web_put_hex(self, px, n);
        lh_ui_canvas_web_put_lit(self, "\"");
    }
    lh_ui_canvas_web_put_lit(self, "}");
    return lh_bool_true;
}

const lh_ui_canvas_backend_t lh_ui_canvas_backend_web = {
    /* begin */ lh_null,
    /* begin_area */ lh_null,
    /* end */ lh_null,
    /* clear */ lh_ui_canvas_web_clear,
    /* fill_rect */ lh_ui_canvas_web_fill_rect,
    /* fill_round_rect */ lh_ui_canvas_web_fill_round_rect,
    /* set_clip */ lh_ui_canvas_web_set_clip,
    /* fill_mask */ lh_ui_canvas_web_fill_mask,
    /* shadow */ lh_ui_canvas_web_shadow,
    /* blur */ lh_null,
    /* glass */ lh_null,
    /* fill_colors */ lh_null,
};

lh_bool_t
lh_ui_canvas_web_open_mem(lh_ui_canvas_web_t *self, lh_str_ptr buf, lh_usize_t cap, lh_uint_t width,
                          lh_uint_t height)
{
    if (self == lh_null || buf == lh_null || cap < 64u)
    {
        return lh_bool_false;
    }
    lh_memory_set(self, sizeof(lh_ptr_deref(self)), 0);
    lh_ui_canvas_web_reset(self, width, height);
    lh_ui_web_buf_init(lh_ui_canvas_web_get_buf_mut(self), lh_ptr_cast(lh_byte_t, buf), cap);
    lh_ui_canvas_web_put_head(self);
    return lh_ui_web_buf_ok(lh_ui_canvas_web_get_buf_mut(self));
}

lh_bool_t
lh_ui_canvas_web_close(lh_ui_canvas_web_t *self)
{
    if (self == lh_null)
    {
        return lh_bool_false;
    }
    lh_ui_canvas_web_put_lit(self, "]}");
    lh_ui_web_buf_finish(lh_ui_canvas_web_get_buf_mut(self));
    lh_ui_canvas_web_free_glyphs(self);
    return lh_ui_web_buf_ok(lh_ui_canvas_web_get_buf_mut(self));
}

lh_void
lh_ui_canvas_web_set_glyphs_v(lh_ui_canvas_web_t *self, lh_ui_canvas_web_glyphs_t *glyphs)
{
    if (self == lh_null)
    {
        return;
    }
    lh_ui_canvas_web_free_glyphs(self);
    lh_ui_canvas_web_set_glyphs(self, glyphs);
}

lh_str_cptr
lh_ui_canvas_web_bytes(const lh_ui_canvas_web_t *self)
{
    if (self == lh_null || lh_ui_web_buf_bytes(lh_ui_canvas_web_get_buf(self)) == lh_null)
    {
        return "";
    }
    return lh_ptr_ccast(lh_char_t, lh_ui_web_buf_bytes(lh_ui_canvas_web_get_buf(self)));
}

lh_usize_t
lh_ui_canvas_web_length(const lh_ui_canvas_web_t *self)
{
    if (self == lh_null)
    {
        return 0;
    }
    return lh_ui_web_buf_len(lh_ui_canvas_web_get_buf(self));
}

const lh_ui_web_buf_t *
lh_ui_canvas_web_get_buf(const lh_ui_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->buf);
}

lh_ui_web_buf_t *
lh_ui_canvas_web_get_buf_mut(lh_ui_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->buf);
}

lh_void
lh_ui_canvas_web_set_buf(lh_ui_canvas_web_t *self, lh_ui_web_buf_t buf)
{
    lh_assert_runtime_ref(self);
    self->buf = buf;
}

lh_uint_t
lh_ui_canvas_web_get_width(const lh_ui_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_ui_canvas_web_set_width(lh_ui_canvas_web_t *self, lh_uint_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_uint_t
lh_ui_canvas_web_get_height(const lh_ui_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_ui_canvas_web_set_height(lh_ui_canvas_web_t *self, lh_uint_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_uint_t
lh_ui_canvas_web_get_ops(const lh_ui_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ops;
}

lh_void
lh_ui_canvas_web_set_ops(lh_ui_canvas_web_t *self, lh_uint_t ops)
{
    lh_assert_runtime_ref(self);
    self->ops = ops;
}

lh_bool_t
lh_ui_canvas_web_is_comma(const lh_ui_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->comma;
}

lh_void
lh_ui_canvas_web_set_comma(lh_ui_canvas_web_t *self, lh_bool_t comma)
{
    lh_assert_runtime_ref(self);
    self->comma = comma;
}

lh_bool_t
lh_ui_canvas_web_is_glyphs_owned(const lh_ui_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glyphs_owned;
}

lh_void
lh_ui_canvas_web_set_glyphs_owned(lh_ui_canvas_web_t *self, lh_bool_t glyphs_owned)
{
    lh_assert_runtime_ref(self);
    self->glyphs_owned = glyphs_owned;
}

lh_ui_canvas_web_glyphs_t *
lh_ui_canvas_web_get_glyphs(const lh_ui_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glyphs;
}

lh_void
lh_ui_canvas_web_set_glyphs(lh_ui_canvas_web_t *self, lh_ui_canvas_web_glyphs_t *glyphs)
{
    lh_assert_runtime_ref(self);
    self->glyphs = glyphs;
}
