/**
 * @file web.c
 * @brief Implementation of `lh/gl/canvas/web.h`: the web canvas itself: its fields (level 1),
 * opening and closing a frame, its length, the glyph set and the backend table.
 */

#include <lh/gl/canvas/web.h>

#include <lh/core/assert.h>
#include <lh/core/memory.h>
#include <lh/core/runtime/allocator.h>

const lh_gl_web_buf_t *
lh_gl_canvas_web_get_buf(const lh_gl_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->buf);
}

lh_gl_web_buf_t *
lh_gl_canvas_web_get_buf_mut(lh_gl_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->buf);
}

lh_void
lh_gl_canvas_web_set_buf(lh_gl_canvas_web_t *self, lh_gl_web_buf_t buf)
{
    lh_assert_runtime_ref(self);
    self->buf = buf;
}

lh_uint_t
lh_gl_canvas_web_get_width(const lh_gl_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_gl_canvas_web_set_width(lh_gl_canvas_web_t *self, lh_uint_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_uint_t
lh_gl_canvas_web_get_height(const lh_gl_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_gl_canvas_web_set_height(lh_gl_canvas_web_t *self, lh_uint_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_uint_t
lh_gl_canvas_web_get_ops(const lh_gl_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ops;
}

lh_void
lh_gl_canvas_web_set_ops(lh_gl_canvas_web_t *self, lh_uint_t ops)
{
    lh_assert_runtime_ref(self);
    self->ops = ops;
}

lh_bool_t
lh_gl_canvas_web_is_comma(const lh_gl_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->comma;
}

lh_void
lh_gl_canvas_web_set_comma(lh_gl_canvas_web_t *self, lh_bool_t comma)
{
    lh_assert_runtime_ref(self);
    self->comma = comma;
}

lh_bool_t
lh_gl_canvas_web_is_glyphs_owned(const lh_gl_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glyphs_owned;
}

lh_void
lh_gl_canvas_web_set_glyphs_owned(lh_gl_canvas_web_t *self, lh_bool_t glyphs_owned)
{
    lh_assert_runtime_ref(self);
    self->glyphs_owned = glyphs_owned;
}

lh_gl_canvas_web_glyphs_t *
lh_gl_canvas_web_get_glyphs(const lh_gl_canvas_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glyphs;
}

lh_void
lh_gl_canvas_web_set_glyphs(lh_gl_canvas_web_t *self, lh_gl_canvas_web_glyphs_t *glyphs)
{
    lh_assert_runtime_ref(self);
    self->glyphs = glyphs;
}

lh_gl_canvas_web_glyphs_t *
lh_gl_canvas_web_get_glyphs_v(lh_gl_canvas_web_t *self)
{
    if (lh_gl_canvas_web_get_glyphs(self) == lh_null)
    {
        lh_gl_canvas_web_set_glyphs(
            self, lh_ptr_rcast(lh_gl_canvas_web_glyphs_t,
                               lh_runtime_allocator_alloc(
                                   sizeof(lh_ptr_deref(lh_gl_canvas_web_get_glyphs(self))))));
        lh_gl_canvas_web_set_glyphs_owned(
            self, (lh_gl_canvas_web_get_glyphs(self) != lh_null) ? lh_bool_true : lh_bool_false);
        lh_gl_canvas_web_glyphs_reset(lh_gl_canvas_web_get_glyphs(self));
    }
    return lh_gl_canvas_web_get_glyphs(self);
}

lh_void
lh_gl_canvas_web_free_glyphs(lh_gl_canvas_web_t *self)
{
    if (lh_gl_canvas_web_is_glyphs_owned(self))
    {
        lh_runtime_allocator_free(lh_gl_canvas_web_get_glyphs(self));
    }
    lh_gl_canvas_web_set_glyphs(self, lh_null);
    lh_gl_canvas_web_set_glyphs_owned(self, lh_bool_false);
}

lh_void
lh_gl_canvas_web_reset(lh_gl_canvas_web_t *self, lh_uint_t width, lh_uint_t height)
{
    lh_gl_web_buf_init(lh_gl_canvas_web_get_buf_mut(self), lh_null, 0);
    lh_gl_canvas_web_set_width(self, width);
    lh_gl_canvas_web_set_height(self, height);
    lh_gl_canvas_web_set_ops(self, 0);
    lh_gl_canvas_web_set_comma(self, lh_bool_false);
    lh_gl_canvas_web_set_glyphs(self, lh_null);
    lh_gl_canvas_web_set_glyphs_owned(self, lh_bool_false);
}

lh_str_cptr
lh_gl_canvas_web_bytes(const lh_gl_canvas_web_t *self)
{
    if (self == lh_null || lh_gl_web_buf_bytes(lh_gl_canvas_web_get_buf(self)) == lh_null)
    {
        return "";
    }
    return lh_ptr_ccast(lh_char_t, lh_gl_web_buf_bytes(lh_gl_canvas_web_get_buf(self)));
}

lh_usize_t
lh_gl_canvas_web_length(const lh_gl_canvas_web_t *self)
{
    if (self == lh_null)
    {
        return 0;
    }
    return lh_gl_web_buf_len(lh_gl_canvas_web_get_buf(self));
}

const lh_gl_canvas_backend_t lh_gl_canvas_backend_web = {
    /* begin */ lh_null,
    /* begin_area */ lh_null,
    /* end */ lh_null,
    /* clear */ lh_gl_canvas_web_clear,
    /* fill_rect */ lh_gl_canvas_web_fill_rect,
    /* fill_round_rect */ lh_gl_canvas_web_fill_round_rect,
    /* set_clip */ lh_gl_canvas_web_set_clip,
    /* fill_mask */ lh_gl_canvas_web_fill_mask,
    /* shadow */ lh_gl_canvas_web_shadow,
    /* blur */ lh_null,
    /* glass */ lh_null,
    /* fill_colors */ lh_null,
};

lh_bool_t
lh_gl_canvas_web_open_mem(lh_gl_canvas_web_t *self, lh_str_ptr buf, lh_usize_t cap, lh_uint_t width,
                          lh_uint_t height)
{
    if (self == lh_null || buf == lh_null || cap < 64u)
    {
        return lh_bool_false;
    }
    lh_memory_set(self, sizeof(lh_ptr_deref(self)), 0);
    lh_gl_canvas_web_reset(self, width, height);
    lh_gl_web_buf_init(lh_gl_canvas_web_get_buf_mut(self), lh_ptr_cast(lh_byte_t, buf), cap);
    lh_gl_canvas_web_put_head(self);
    return lh_gl_web_buf_ok(lh_gl_canvas_web_get_buf_mut(self));
}

lh_bool_t
lh_gl_canvas_web_close(lh_gl_canvas_web_t *self)
{
    if (self == lh_null)
    {
        return lh_bool_false;
    }
    lh_gl_canvas_web_put_lit(self, "]}");
    lh_gl_web_buf_finish(lh_gl_canvas_web_get_buf_mut(self));
    lh_gl_canvas_web_free_glyphs(self);
    return lh_gl_web_buf_ok(lh_gl_canvas_web_get_buf_mut(self));
}

lh_void
lh_gl_canvas_web_set_glyphs_v(lh_gl_canvas_web_t *self, lh_gl_canvas_web_glyphs_t *glyphs)
{
    if (self == lh_null)
    {
        return;
    }
    lh_gl_canvas_web_free_glyphs(self);
    lh_gl_canvas_web_set_glyphs(self, glyphs);
}
