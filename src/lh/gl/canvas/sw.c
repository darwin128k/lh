/**
 * @file sw.c
 * @brief Implementation of `lh/gl/canvas/sw.h`: the software canvas itself: its fields (level 1),
 * init, the pixmap, the limit, the clip, and the backend table.
 */

#include <lh/gl/canvas/sw.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

const lh_gl_pixmap_t *
lh_gl_canvas_sw_get_pixmap(const lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pixmap);
}

lh_gl_canvas_sw_t *
lh_gl_canvas_sw_from(lh_ptr context)
{
    lh_gl_canvas_sw_t *self = lh_ptr_rcast(lh_gl_canvas_sw_t, context);

    lh_assert_runtime_ref(self);
    return self;
}

/* No begin_area: the software backend has no buffer of its own to size, so it
   draws whatever pixmap it was given and a partial frame falls back to a whole
   target one. A caller that owns a strip buffer gives it one per frame. */
const lh_gl_canvas_backend_t lh_gl_canvas_backend_sw = {
    lh_null,
    lh_null,
    lh_null,
    lh_gl_canvas_sw_clear,
    lh_gl_canvas_sw_fill_rect,
    lh_gl_canvas_sw_fill_round_rect,
    lh_gl_canvas_sw_use_clip,
    lh_gl_canvas_sw_fill_mask,
    lh_gl_canvas_sw_shadow,
    lh_gl_canvas_sw_blur,
    lh_gl_canvas_sw_glass,
    lh_gl_canvas_sw_fill_colors,
};

lh_gl_pixmap_t *
lh_gl_canvas_sw_get_pixmap_mut(lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pixmap);
}

lh_gl_rect_t *
lh_gl_canvas_sw_get_limit_mut(lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->limit);
}

lh_void
lh_gl_canvas_sw_set_limit(lh_gl_canvas_sw_t *self, lh_gl_rect_t limit)
{
    lh_assert_runtime_ref(self);
    self->limit = limit;
}

const lh_gl_canvas_clip_t *
lh_gl_canvas_sw_get_clip(const lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}

lh_gl_canvas_clip_t *
lh_gl_canvas_sw_get_clip_mut(lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}

const lh_gl_rect_t *
lh_gl_canvas_sw_get_limit(const lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->limit);
}

lh_void
lh_gl_canvas_sw_set_pixmap(lh_gl_canvas_sw_t *self, lh_gl_pixmap_t pixmap)
{
    lh_assert_runtime_ref(self);
    self->pixmap = pixmap;
}

lh_void
lh_gl_canvas_sw_set_clip(lh_gl_canvas_sw_t *self, lh_gl_canvas_clip_t clip)
{
    lh_assert_runtime_ref(self);
    self->clip = clip;
}

lh_void
lh_gl_canvas_sw_use_pixmap(lh_gl_canvas_sw_t *self, const lh_gl_pixmap_t *pixmap)
{
    lh_assert_runtime_ref(pixmap);
    lh_gl_canvas_sw_set_pixmap(self, lh_ptr_deref(pixmap));
    lh_gl_pixmap_get_bounds(pixmap, lh_gl_canvas_sw_get_limit_mut(self));
    lh_gl_canvas_clip_init_empty(lh_gl_canvas_sw_get_clip_mut(self));
}

lh_void
lh_gl_canvas_sw_copy_limit(const lh_gl_canvas_sw_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gl_canvas_sw_get_limit(self));
}

lh_bool_t
lh_gl_canvas_sw_is_rounded(const lh_gl_canvas_sw_t *self)
{
    return lh_gl_canvas_clip_get_round_count(lh_gl_canvas_sw_get_clip(self)) > 0U ? lh_bool_true
                                                                                  : lh_bool_false;
}

lh_void
lh_gl_canvas_sw_use_clip(lh_ptr context, const lh_gl_canvas_clip_t *clip)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    lh_gl_rect_t bounds;
    lh_gl_pixmap_get_bounds(lh_gl_canvas_sw_get_pixmap_mut(self), lh_addr_of(bounds));

    lh_gl_canvas_clip_init_empty(lh_gl_canvas_sw_get_clip_mut(self));
    lh_gl_canvas_sw_set_limit(self, bounds);
    lh_return_if(lh_null_eq(clip));
    lh_gl_canvas_sw_set_clip(self, lh_ptr_deref(clip));
    lh_gl_rect_intersection(lh_addr_of(bounds), lh_gl_canvas_clip_get_rect(clip),
                            lh_gl_canvas_sw_get_limit_mut(self));
}

lh_void
lh_gl_canvas_sw_init(lh_gl_canvas_sw_t *self)
{
    lh_gl_pixmap_t empty;

    lh_gl_pixmap_init_empty(lh_addr_of(empty));
    lh_gl_canvas_sw_use_pixmap(self, lh_addr_of(empty));
}
